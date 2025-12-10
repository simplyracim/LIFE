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


    I have a C++ app that simulates conway's game of life based on user-inputted .txt files, the program also produces output.txt files from the simulation. I'm using SFML for graphics

Should I separate user facing files (The .exe, SFML .dlls and .txt files) from the source file ? (unitary tests, .obj files, .cpp and headers)I'm thinking about putting user-facing files in a separate folder like output/ or something. can someone tell me if the following reasoning is correct ?

The program creates most of the files in output/ (.exe ...ect), but the folder already has some files by default (.dlls, predefined example states for conway's game of life)

Should I push the whole filesystem to github ? or just the user-facing files ? is that the difference between "open-source" projects and "closed-source" projects ?