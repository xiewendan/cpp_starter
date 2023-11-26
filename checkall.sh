echo ==========format-all
find sources -iname "*.h" -o -iname "*.cpp" | xargs clang-format -style=file -i

echo ""
echo "==========cpp-check-name"
cppcheck --report-progress -j 100 --quiet --force --dump sources/

find sources -iname "*.cpp.dump" | xargs python3 tools/cppcheck/addons/naming.py --json=cppcheckname.json  
find sources -iname "*.cpp.dump" | xargs rm

echo ""
echo "==========cpplint"
find sources -iname "*.h" -o -iname "*.cpp" | xargs python tools/cpplint.py --quiet