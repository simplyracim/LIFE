The ideal folder structure would look like this :

my_cpp_project/
├── makefile
├── readme.md
├── LICENSE to be added
├── .gitignore
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── application.cpp
│   │   └── application.h
│   └── utils/
│       ├── helper_functions.cpp
│       └── helper_functions.h
├── include/
│   ├── my_project/
│   │   ├── application.h
│   │   └── helper_functions.h
├── build/
├── tests/
│   └── unit_tests.cpp
├── doc/
│   └── design_document.md
└── external/
    └── some_library/