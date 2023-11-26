#!/usr/bin/env python3
#
# cppcheck addon for naming conventions
#
# Example usage (variable name must start with lowercase, function name must start with uppercase):
# $ cppcheck --dump path-to-src/
# $ python addons/naming.py --var='[a-z].*' --function='[A-Z].*' path-to-src/*.dump
#

import cppcheckdata
import sys
import re

g_dictReObj = {}
g_setReportPos = set()
g_setFunctionWhiteList = set()

RE_VARNAME = None
RE_MEMVAR = None
RE_GVAR = None
RE_CLASS = None
RE_CONSTNAME = None
RE_PRIVATE_MEMBER_VARIABLE = None
RE_FUNCTIONNAME = None
RE_EXCLUDE_LIST = None
JSON_FILE = None

def validate_regex(expr):
    try:
        re.compile(expr)
    except re.error:
        print('Error: "{}" is not a valid regular expression.'.format(expr))
        exit(1)


def parse_arg():
    global g_setFunctionWhiteList 

    global RE_VARNAME
    global RE_MEMVAR 
    global RE_GVAR 
    global RE_CLASS 
    global RE_CONSTNAME 
    global RE_PRIVATE_MEMBER_VARIABLE 
    global RE_FUNCTIONNAME 
    global RE_EXCLUDE_LIST 
    global JSON_FILE 

    for arg in sys.argv[1:]:
        if arg[:6] == '--var=':
            RE_VARNAME = arg[6:]
            validate_regex(RE_VARNAME)
        elif arg.startswith("--memvar="):
            RE_MEMVAR = arg[arg.find('=')+1:]
            validate_regex(RE_MEMVAR)
        elif arg.startswith("--gvar="):
            RE_GVAR = arg[arg.find('=')+1:]
            validate_regex(RE_GVAR)
        elif arg.startswith("--class="):
            RE_CLASS = arg[arg.find('=')+1:]
            validate_regex(RE_CLASS)
        elif arg.startswith('--const='):
            RE_CONSTNAME = arg[arg.find('=')+1:]
            validate_regex(RE_CONSTNAME)
        elif arg.startswith('--private-member-variable='):
            RE_PRIVATE_MEMBER_VARIABLE = arg[arg.find('=')+1:]
            validate_regex(RE_PRIVATE_MEMBER_VARIABLE)
        elif arg[:11] == '--function=':
            RE_FUNCTIONNAME = arg[11:]
            validate_regex(RE_FUNCTIONNAME)
        elif arg.startswith("--json="):
            JSON_FILE = arg[arg.find('=')+1:]
            
    if JSON_FILE:
        import os
        import json
        assert(os.path.exists(JSON_FILE))

        with open(JSON_FILE, "r") as fp:
            JsonCfg = json.load(fp)
            RE_VARNAME = JsonCfg.get('var', None)
            RE_MEMVAR = JsonCfg.get('memvar', None)
            RE_GVAR = JsonCfg.get('gvar', None)
            RE_CLASS = JsonCfg.get('class', None)
            RE_FUNCTIONNAME = JsonCfg.get('function', None)
            RE_EXCLUDE_LIST = JsonCfg.get('exclude', None)

            if RE_VARNAME is not None:
                validate_regex(RE_VARNAME)
            if RE_MEMVAR is not None:
                validate_regex(RE_MEMVAR)
            if RE_GVAR is not None:
                validate_regex(RE_GVAR)
            if RE_CLASS is not None:
                validate_regex(RE_CLASS)
            if RE_FUNCTIONNAME is not None:
                validate_regex(RE_FUNCTIONNAME)
            if RE_EXCLUDE_LIST is not None:
                for szExclude in RE_EXCLUDE_LIST:
                    validate_regex(szExclude)

            listFunction = JsonCfg.get("function_whitelist", [])
            for szFunction in listFunction:
                g_setFunctionWhiteList.add(szFunction)

def GetRe(szRe):
    global g_dictReObj
    if szRe not in g_dictReObj:
        g_dictReObj[szRe] = re.compile(szRe)

    return g_dictReObj[szRe]

def Match(szRe, szStr):
    return GetRe(szRe).match(szStr)

def IsExcludeLocation(szPath):
    if RE_EXCLUDE_LIST is None:
        return False
    
    for szExclude in RE_EXCLUDE_LIST:
        if Match(szExclude, szPath):
            return True
    
    return False

def reportError(token, severity, msg, errorId):
    global g_setReportPos
    if IsExcludeLocation(token.file):
        return

    szKey = "%s:%d:%d:%s" %(token.file, token.linenr, token.column, errorId)
    if szKey in g_setReportPos:
        return

    g_setReportPos.add(szKey)
    cppcheckdata.reportError(token, severity, msg, 'naming', errorId)


def check_file():
    for arg in sys.argv[1:]:
        if not arg.endswith('.dump'):
            continue
        # print('Checking ' + arg + '...')
        data = cppcheckdata.CppcheckData(arg)

        for cfg in data.iterconfigurations():
            # print('Checking %s, config %s...' % (arg, cfg.name))
            dictToken = {}
            for TokenObj in cfg.tokenlist:
                dictToken[TokenObj.Id] = TokenObj

            if RE_VARNAME:
                for var in cfg.variables:
                    if var.nameToken and not var.isConst:
                        szRe = None
                        if var.access in ('Private', 'Public', 'Protected'):
                            szRe = RE_MEMVAR
                        elif var.access in ('Global', 'Namespace'):
                            szRe = RE_GVAR
                        else:
                            szRe = RE_VARNAME
                        if szRe and not Match(szRe, var.nameToken.str):
                            reportError(var.nameToken, 'style', 'Variable ' + var.nameToken.str + ' violates naming convention. ' + szRe, 'varName')
            if RE_CONSTNAME:
                for var in cfg.variables:
                    if var.access == 'Private':
                        continue
                    if var.nameToken and var.isConst:
                        res = Match(RE_CONSTNAME, var.nameToken.str)
                        if not res:
                            reportError(var.typeStartToken, 'style', 'Constant ' +
                                        var.nameToken.str + ' violates naming convention', 'constname')
            if RE_PRIVATE_MEMBER_VARIABLE:
                for var in cfg.variables:
                    if (var.access is None) or var.access != 'Private':
                        continue
                    res = Match(RE_PRIVATE_MEMBER_VARIABLE, var.nameToken.str)
                    if not res:
                        reportError(var.typeStartToken, 'style', 'Private member variable ' +
                                    var.nameToken.str + ' violates naming convention', 'privateMemberVariable')
            if RE_CLASS:
                for scope in cfg.scopes:
                    if scope.type == "Class":
                        res = Match(RE_CLASS, scope.className)
                        if not res:
                            reportError(scope.bodyStart, 'style', 'class name' +
                                        scope.className + ' violates naming convention. ' + RE_CLASS, 'className')

            if RE_FUNCTIONNAME:
                for function in cfg.functions:
                    if function.type == "OperatorEqual":
                        continue
                    if function.name in g_setFunctionWhiteList:
                        continue
                    res = Match(RE_FUNCTIONNAME, function.name)
                    if not res:
                        TokenObj = dictToken[function.tokenDefId]
                        reportError(TokenObj, 'style', 'Function ' + function.name + ' violates naming convention. ' + RE_FUNCTIONNAME, 'functionName')



def main():
    parse_arg()
    check_file()
    sys.exit(cppcheckdata.EXIT_CODE)
    pass

if __name__ == "__main__":
    main()
