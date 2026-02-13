#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>
#include "json.hpp"

//
// SliceProfile Class Definitions 
//

SliceProfile::SliceProfile(std::string& sliceName, std::string& fileName, SliceLine* decl, std::string& hash, std::vector<SliceLine*> defs, std::vector<SliceLine*> uses) {
    sliceName_ = sliceName;
    fileName_ = fileName;
    declLine_ = decl;
    hash_ = hash;
    defLines_ = defs;
    useLines_ = uses;
}

void SliceProfile::addDef(SliceLine* s) {
    defLines_.push_back(s);
}

void SliceProfile::addUse(SliceLine* s) {
    useLines_.push_back(s);
}

std::string SliceProfile::getName() const {
    return sliceName_;
}

std::string SliceProfile::getFile() const {
    return fileName_;
}

SliceLine* SliceProfile::getDecl() const {
    return declLine_;
}

std::string SliceProfile::getHash() const {
    return hash_;
}

std::vector<SliceLine*> SliceProfile::getDefs() const {
    return defLines_;
}

std::vector<SliceLine*> SliceProfile::getUses() const {
    return useLines_;
}

SliceLine::SliceLine(int r, int c, std::string f) {
    row_ = r;
    column_ = c;
    fileName_ = f;
}

// prints out slice profile data (neatly)
void SliceProfile::print() const {
    std::cout << "Slice: " << sliceName_ << " From File: " << fileName_ << ", Declared on Line: " << declLine_->getLine().first << " (column: " << declLine_->getLine().second << ")" << std::endl;
    std::cout << "Hash: " << hash_ << std::endl;
    std::cout << "Definition Lines: " << std::endl;
    for (auto& line : defLines_) {
        std::cout << "\t" << "row: " << line->getLine().first << " (column: " << line->getLine().second << ")" << "\t" << line->getFile() << std::endl;
    }
    std::cout << "Use Lines: " << std::endl;
    for (auto& line : useLines_) {
        std::cout << "\t" << "row: " << line->getLine().first << " (column: " << line->getLine().second << ")" << "\t" << line->getFile() << std::endl;
    }
    std::cout << std::endl;
}

//
// Other Function Definitions
//

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
SliceLine* getSliceDeclLine(std::string key) {
    // Gets the index of the slice name's final character
    std::string sliceName = getSliceName(key);
    int lineStart = sliceName.length() + 1;

    // creates substring starting at the index after the first '-' (the decl line number) 
    std::string sliceLineStr = getSliceName(key.substr(lineStart, key.length()));
    // std::cout << sliceLineStr << std::endl;

    // look for the column using a column start value (+2 is for the 2 hyphens)
    int columnStart = sliceName.length() + sliceLineStr.length() + 2;
    std::string sliceColumnStr = getSliceName(key.substr(columnStart, key.length()));
    // std::cout << sliceColumnStr << std::endl;

    // return the created pair of line and column
    try {
        int row = std::stoi(sliceLineStr);
        int col = std::stoi(sliceColumnStr);
        return new SliceLine(row, col);
    }
    catch (const std::invalid_argument&) {
        std::cout << "invalid argument in getSliceDeclLine." << std::endl;
        return nullptr;
    }
}

// splices the line data into different pieces (line, column)
std::pair<int, int> spliceLineData(json& j) {
    // std::cout << j << std::endl;
    std::string s = to_string(j);
    // std::cout << s << std::endl;
    size_t quotes = s.find('"');
    size_t colon = s.find(':');

    if (colon == std::string::npos) {
        // Handle error: return a default value or throw with a clear message
        std::cout << "colon = npos" << std::endl;
        return std::make_pair(-1, -1);
    }
    try {
        int first = std::stoi(s.substr(quotes + 1, colon));
        int second = std::stoi(s.substr(colon + 1, s.back() - 1));
        return std::make_pair(first, second);
    } 
    catch (const std::invalid_argument&) {
        // Handle error: return a default value or throw with a clear message
        std::cout << "invalid argument: " << s << std::endl;
        return std::make_pair(-1, -1);
    }
}

SliceLine* returnLineData(json& j) {
    // std::cout << j << std::endl;
    std::string s = to_string(j);
    // std::cout << s << std::endl;
    size_t quotes = s.find('"');
    size_t colon1 = s.find(':');

    if (colon1 == std::string::npos) {
        // Handle error: return a default value or throw with a clear message
        std::cout << "colon = npos" << std::endl;
        return new SliceLine(-1, -1, "");
    }
    try {
        size_t colon2 = s.find(':', colon1 + 1);
        // std::cout << colon2 << std::endl;
        std::string name = s.substr(quotes + 1, colon1 - 1);
        int row = std::stoi(s.substr(colon1 + 1, colon2));
        int col = std::stoi(s.substr(colon2 + 1, s.back() - 1));
        // std::cout << "worked. " << row << ", " << col << ", " << name << std::endl;
        return new SliceLine(row, col, name);
    } catch (const std::invalid_argument&) {
        // Handle error: return a default value or throw with a clear message
        std::cout << "invalid argument: " << s << std::endl;
        return new SliceLine(-1, -1, "");
    }
}

// adds all the completed slice profiles to the vector<SliceProfile>
void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices) {
    // takes every object in json, adds them to vector of slices
    for (auto& profile : j.items()) {
        std::string name = getSliceName(profile.key());
        SliceLine* declLine = getSliceDeclLine(profile.key());
        SliceLine* decl;
        std::string file, hashStr;
        std::vector<SliceLine*> defs, uses;

        // look for the defs, uses, filename
        json sliceJSON = profile.value();
        for (auto& property : sliceJSON.items()) {
            std::string attribute = property.key();
            auto value = property.value();

            // used for creating the hash
            if (attribute == "file") {
                // grabs the file name
                file = value;

                // converts into single hash-able string
                std::string declLineString = std::to_string(declLine->getLine().first) + "-" + std::to_string(declLine->getLine().second);
                std::string input = name + declLineString + file;

                // hash it
                unsigned char hash[SHA_DIGEST_LENGTH];
                SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

                // Convert hash to hex string
                char buf[3];
                for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
                    snprintf(buf, sizeof(buf), "%02x", hash[i]);
                    hashStr += buf;
                }
            }

            if (attribute == "initial") {
                decl = returnLineData(value);
            }

            // adds the def lines
            if (attribute == "definition") {
                for (auto& line : value) {
                    // std::cout << line << std::endl;
                    defs.push_back(returnLineData(line));
                }
            }

            // adds the use lines
            if (attribute == "use") {
                for (auto& line : value) {
                    // std::cout << line << std::endl;
                    uses.push_back(returnLineData(line));
                }
            }

            // // references
            // if (attribute == "dependence") {
            //     for (auto& ref : value) {
            //         for (auto& item : ref.items()) {
            //             //std::cout << item.key() << std::endl;
            //         }
            //     }
            // }
        }
        SliceProfile s(name, file, decl, hashStr, defs, uses);
        slices.push_back(s);
    }
}