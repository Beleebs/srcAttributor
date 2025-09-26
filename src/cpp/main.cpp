#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "json.hpp"
#include "xml.hpp"

using std::vector; using std::map;

int main(int argc, char** argv) {

    // setup CLI11 Command line input
    std::string inputFile = "", outputFile = "";
    CLI::App app{"srcAttributor (srcSlice addon)"};
    app.add_option("-i, --input", inputFile, "Name of JSON input file.")
        ->required()
        ->type_name("");
    app.add_option("-o, --output", outputFile, "Name of the srcML file to write into")
        ->required()
        ->type_name("");

    CLI11_PARSE(app, argc, argv);

    // create a vector of slice profiles
    vector<SliceProfile> slices;

    // begin parsing
    std::ifstream file(inputFile);
    json j;
    file >> j;

    // check to see if it was successfully loaded
    if (j == NULL) {
        std::cerr << "Failed to open json file." << std::endl;
        exit(1);
    }

    // Gets all slice profiles from the json, pushes into slices
    getSliceProfiles(j, slices);

    /* debug print
    std::cout << std::endl;
    for (auto& singleProfile : slices) {
        singleProfile.print();
        std::cout << std::endl;
    }
    */

    // EXECUTE XML PHASE!!!!!!!!!!
    SliceProfileHandler sph(slices);
    insertAttributes(sph, outputFile);
    
}