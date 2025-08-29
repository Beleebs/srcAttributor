#include <attributor.hpp>

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