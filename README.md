[TOC]

# 1. 1 📌 introduction

initialize a cpp project with google format, naming check and cpplint check.

# 2. 2 ✨ features
* format 
* naming check
* cpplint

# 3. 3 ✅ requirements
* python3

* vscode plugin
  > list in .vscode/extensions.json
  * ms-vscode.cpptools: cpp format
  * ms-vscode.cpptools-extension-pack：include cmake and cmake tools extension
  * jbenden.c-cpp-flylint: cpplint

* cppcheck 

* clang-format

# 4. 4 📦 installation
* python
  ~~~
  apt install python3
  ~~~
  ~~~
  apt install python
  ~~~
  > python is use to run tools/cpplint.py
* cppcheck 
  ~~~
  apt install cppcheck
  ~~~

* clang-format
  ~~~
  apt install clang-format
  ~~~

* vscode plugin
  > when open the project, vscode will suggest you install recommend plugin


# 5. 5 ⚙️ configuration

* format：.clang-format
  > in different linux platform, clang-format version is diff, .clang-format is different, so you ought to generate on the platform
  ~~~
  clang-format -style=google -dump-config > .clang-format
  ~~~

* naming check：cppcheckname.json

* cpplint：CPPLINT.cfg


# 6. 6 💻 usage

* check_select.sh file_path：will check naming and cpplint by current file
* checkall.sh：check all the file in sources. it will format, check naming and cpplint


> there is some rule not check by cpplint, it list in book effect c++, i summerize in effect_cpp.xmind
