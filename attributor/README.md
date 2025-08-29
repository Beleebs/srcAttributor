# srcAttributor

## :hammer_and_wrench: Building srcAttributor
To use you will need to install the following dependencies:
- libxml (tree, parser, xmlstring)
- nlohmann/json (used for json parsing)
- openssl (sha1 hashing)

```bash
sudo apt install \
nlohmann-json3-dev \
libxml2-dev \
libcurl4-openssl-dev
```

**NOTE**
To use this tool you should have both srcML and srcSlice installed.

Additionally, make sure all xml files ran through srcml are of the format: "originalCodeFile.cpp.xml"
For example, the file "MazeGame.cpp" should have the corresponding xml file "MazeGame.cpp.xml"

## Using srcAttributor
```bash
mkdir build
cd build
cmake ../
make
```

To run srcAttributor after building:
```bash
srcml file.cpp -o file.cpp.xml --position --hash
srcslice -i file.cpp.xml -o file.json
./bin/srcAttributor -i file.json
```

It should hopefully work. Hail Mary.