## Build using the command below

``` qt-cmake -S . -B build -G Ninja```
OR

```cmake --build build         ```
## Remove build files with
Remove-Item -Recurse -Force build


## If Above command Does not work try below
```cmake -S . -B build -G Ninja `
    -DCMAKE_C_COMPILER=path/to/mingw_64/bin/gcc.exe `
    -DCMAKE_CXX_COMPILER=path/to/mingw_64/bin/g++.exe```

## Install with 
```cmake --build build```