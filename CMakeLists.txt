cmake_minimum_required(VERSION 3.20)
project(srcAttributor)

# enable ctest
enable_testing()

# find needed libraries
find_package(LibXml2 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wno-reorder -Wunused-variable -Wunused-parameter -Wnarrowing -Werror=narrowing -Wno-error=all")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${LIBXML2_INCLUDE_DIR}
                    external
                    src/hpp)

# sub-dirs containing a CMakeLists.txt file
add_subdirectory(src)
#add_subdirectory(test) # @..todo..@

# Download the CLI11 parser into directory named "external".
set(CMAKE_EXTERNAL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external) # Creates folder if doesn't exists.
# Name of file is needed since content of file itself are being downloaded.
file(DOWNLOAD https://github.com/CLIUtils/CLI11/releases/download/v2.3.2/CLI11.hpp ${CMAKE_EXTERNAL_SOURCE_DIR}/CLI11.hpp)