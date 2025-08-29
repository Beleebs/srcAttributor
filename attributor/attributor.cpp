#include <fstream>
#include <nlohmann/json.hpp>
#include "openxml.hpp"

using json = nlohmann::json;

std::string getSliceName(std::string);
int getSliceDeclLine(std::string);

// static variables
const bool conciseMode = true;

int main(int argc, char* argv[]) {
    // follows proper syntax
    if (argc != 2) {
        std::cout << "Syntax: ./jsonReader [json]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    // create ifstream with the filename, along with the json object
    std::ifstream file(filename);
    json j;

    // load file into json j
    file >> j;

    // ensure there is something inside of the json
    if (j == NULL) {
        std::cerr << "Failed to open json file." << std::endl;
    }

    // So it goes { sliceProfile1{elements}, sliceProfile2{elements}, ... , sliceProfilen{elements} }
    for (auto& sliceProfile : j.items()) {
        // sets up slice name and decl line
        std::string sliceName = getSliceName(sliceProfile.key());
        int sliceLine = getSliceDeclLine(sliceProfile.key());
        std::string fileName;
        std::string hash;

        // gets elements in child json
        json childJSON = sliceProfile.value();

        // for loop for slice:decl
        for (auto& child : childJSON.items()) {
            if (child.key() == "file") {
                fileName = child.value();
            }
        }

        AddXMLNamespace(fileName);
        AddDeclAttribute(fileName, sliceName, sliceLine);
        hash = GenerateHash(sliceName, sliceLine, fileName);
        std::cout << "Placed slice:decl in file " << fileName << ".xml at " << sliceLine << " for the slice " << sliceName << std::endl; 

        // std::cout << "Slice: " << sliceName << "\nDeclaration: " << sliceLine << std::endl;

        // for loop for slice:def and slice:use
        for (auto& child : childJSON.items()) {
            // get attribute + value
            std::string sliceAttribute = child.key();
            auto sliceValue = child.value();

            // Concise mode keeps the most important attribute information (defs and uses)
            if (conciseMode) {
                if (sliceAttribute != "definition" && sliceAttribute != "use") {
                    continue;
                }
            }

            for (int i = 0; i < sliceValue.size(); i++) {
                // decl is sliceLine, and superscedes all other attributes. 
                // This prevents the usage/def being used in the same lines as def.
                if (sliceValue[i] != sliceLine) {
                    AddExprAttribute(fileName, sliceName, sliceAttribute, sliceValue[i], hash);
                    std::cout << "Placed " << sliceAttribute << " in file " << fileName << ".xml at " << sliceValue[i] << " for the slice " << sliceName << std::endl;
                }
            }


            // output slice info
            // std::cout << "\t" << sliceAttribute << ": " << sliceValue << std::endl;
        }
    }
}

// returns slice name
// since the slice name always follows the format of name, line number declared, sha1 filename hash, we can find the name of the slice by getting a substring until a '-'.
// since variable names cannot include a '-'.
std::string getSliceName(std::string key) {
    std::string sliceName;
    size_t hyphen = key.find("-");
    
    // finds hyphen
    if (hyphen != std::string::npos) 
        sliceName = key.substr(0, hyphen);
    else 
        sliceName = key;

    // returns slice name
    return sliceName;
}

// returns decl line number
// gets the slice name, sets as a start for substr, goes until next '-'
int getSliceDeclLine(std::string key) {
    // Gets the index of the slice name's final character
    std::string sliceName = getSliceName(key);
    int strStart = sliceName.length() + 1;

    // creates substring starting at the index after the first '-' (the decl line number) 
    std::string sliceLineStr = getSliceName(key.substr(strStart, key.length()));
    int sliceLine = std::stoi(sliceLineStr);
    return sliceLine;
}