#include <attributor.hpp>

int main(int argc, char** argv) {
    // cli11 targets being used for operations
    std::string fileIn = "";
    std::string fileOut = "";
    
    CLI::App app{"srcAttributor is a property generator based on srcSlice outputs.", "srcAttributor"};
    app.add_option("-i,--input", fileIn, "JSON File Input")
        ->required()
        ->check(CLI::ExistingFile);
    /*
    app.add_option("-o,--output", fileOut, "XML File Output")
    ->required()
    ->check(CLI::ExistingFile);
    */
    CLI11_PARSE(app, argc, argv);

    
    // create ifstream with the filename, along with the json object
    std::ifstream file(fileIn);
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