# srcAttributor: a srcSlice attributing tool
## How to use:

You will need to create a build directory to run the application
```
mkdir build
cd build/
cmake ..
make
```

Program executable will be placed in /build/bin.

To use the program:
```
./srcattributor -i [json file input path] -o [srcML file output path]
```

* The .json file includes slice profiles, with their slice information
* srcAttributor reads in these slice profiles, then evaluates them with the srcML file
* attributes for the different slice profiles are inserted into correlating elements 
