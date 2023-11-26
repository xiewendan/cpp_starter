cppcheck --quiet --force --dump $1
python3 tools/cppcheck/addons/naming.py --json=cppcheckname.json $1.dump
rm $1.dump

python tools/cpplint.py --quiet $1