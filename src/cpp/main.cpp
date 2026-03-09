#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "json.hpp"
#include "xml.hpp"

using std::vector; using std::map;

int main(int argc, char** argv) {

    // setup CLI11 Command line input
    std::string inputFile = "", outputFile = "";
    bool showSlices = false;
    CLI::App app{"srcAttributor (srcSlice addon)"};
    app.add_option("-i, --input", inputFile, "Name of JSON input file.")
        ->required()
        ->type_name("");
    app.add_option("-o, --output", outputFile, "Name of the srcML file to write into.")
        ->required()
        ->type_name("");
    app.add_flag("-s, --slices", showSlices, "(Optional) Displays used slice profiles with attribute information.");
    

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
    // debug print
    if (showSlices) {
        for (auto& s : slices) {
            s.print();
        }
    }

    // Create handler
    SliceProfileHandler sph(slices);

    // open xml file
    const char* xmlFile = outputFile.c_str();
    xmlDocPtr doc = xmlReadFile(xmlFile, NULL, XML_PARSE_BIG_LINES);
    xmlNodePtr root = xmlDocGetRootElement(doc);

    // create dictionary
    std::unordered_map<SliceLine, xmlNodePtr> locations; 
    std::string unitFile = "";

    std::cout << "Creating Dictionary..." << std::endl;
    createNodeDictonary(slices, root, locations, unitFile);
    std::cout << std::endl;

    for (auto& l : locations) {
        std::cout << l.first.print() << std::endl;
    }

    // create slice xml namespace
    xmlNodePtr nsRoot = xmlDocGetRootElement(doc);
    if (xmlSearchNs(doc, nsRoot, (const xmlChar*)"slice") == NULL) {
        xmlNewNs(nsRoot, (const xmlChar*)"http://www.srcML.org/srcML/slice", (const xmlChar*)"slice");
    }

    // begin inserting attributes
    insertAttributes(slices, locations);

    // close files, save resources
    xmlFreeDoc(doc);
    file.close();
}