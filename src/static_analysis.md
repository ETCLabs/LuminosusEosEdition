# Static Analysis

## Google cpplint.py

Checks for Google Styleguide
Example usage:
```
./cpplint.py core/Matrix.cpp
```

## clang-tidy

Example usage:
```
clang-tidy core/*.cpp -config='' -- -std=c++11 -I/home/tim/Qt/5.10.0/gcc_64/include/QtCore -I/home/tim/Qt/5.10.0/gcc_64/include/QtQml -I/home/tim/Qt/5.10.0/gcc_64/include/QtNetwork -I/home/tim/Qt/5.10.0/gcc_64/include/QtQuick -I/home/tim/Qt/5.10.0/gcc_64/include/QtMultimedia -I/home/tim/Qt/5.10.0/gcc_64/include/QtGui -I/home/tim/Qt/5.10.0/gcc_64/include/QtWidgets -I/home/tim/Qt/5.10.0/gcc_64/include -I. -fPIC
```
