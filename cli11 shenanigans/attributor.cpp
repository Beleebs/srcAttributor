// Ben Leber
// 8/18/2025
// CLI11 Learning Process for srcAttributor

#include <iostream>
#include "CLI11.hpp"

// I think I want the input to go along the lines of:
// ./attributor -i [JSON file] -o [XML file]

int main(int argc, char **argv) {
    CLI::App app{"srcAttributor is a property generator based on srcSlice outputs.", "srcAttributor"};

    int p = 0;
    std::string fileIn, fileOut;
    app.add_option("-i,--input", fileIn, "JSON File Input")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-o,--output", fileOut, "XML File Output")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    return 0;
}