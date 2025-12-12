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

SliceProfile::SliceProfile(std::string sliceName, std::pair<int, int> decl, std::string hash, std::vector<std::pair<int, int>> defs, std::vector<std::pair<int, int>> uses) {
    sliceName_ = sliceName;
    declLine_ = decl;
    hash_ = hash;
    defLines_ = defs;
    useLines_ = uses;
}

void SliceProfile::addDef(const int& line, const int& column) {
    defLines_.push_back(std::make_pair(line, column));
}

void SliceProfile::addUse(const int& line, const int& column) {
    useLines_.push_back(std::make_pair(line, column));
}

std::string SliceProfile::getName() const {
    return sliceName_;
}

std::pair<int, int> SliceProfile::getDecl() const {
    return declLine_;
}

std::string SliceProfile::getHash() const {
    return hash_;
}

std::vector<std::pair<int, int>> SliceProfile::getDefs() const {
    return defLines_;
}

std::vector<std::pair<int, int>> SliceProfile::getUses() const {
    return useLines_;
}

// prints out slice profile data (neatly)
void SliceProfile::print() const {
    std::cout << "Slice: " << sliceName_ << ", Declared on Line: " << declLine_.first << " (column: " << declLine_.second << ")" << std::endl;
    std::cout << "Hash: " << hash_ << std::endl;
    std::cout << "Definition Lines: " << std::endl;
    for (auto& line : defLines_) {
        std::cout << line.first << " (column: " << line.second << ")" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Use Lines: " << std::endl;
    for (auto& line : useLines_) {
        std::cout << line.first << " (column: " << line.second << ")" << std::endl;
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
std::pair<int, int> getSliceDeclLine(std::string key) {
    // Gets the index of the slice name's final character
    std::string sliceName = getSliceName(key);
    int lineStart = sliceName.length() + 1;

    // creates substring starting at the index after the first '-' (the decl line number) 
    std::string sliceLineStr = getSliceName(key.substr(lineStart, key.length()));

    // look for the column using a column start value (+2 is for the 2 hyphens)
    int columnStart = sliceName.length() + sliceLineStr.length() + 2;
    std::string sliceColumnStr = getSliceName(key.substr(columnStart, key.length()));

    // return the created pair of line and column
    return std::make_pair(std::stoi(sliceLineStr), std::stoi(sliceColumnStr));
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
    } catch (const std::invalid_argument&) {
        // Handle error: return a default value or throw with a clear message
        std::cout << "invalid argument: " << s << std::endl;
        return std::make_pair(-1, -1);
    }
}

// adds all the completed slice profiles to the vector<SliceProfile>
void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices) {
    // takes every object in json, adds them to vector of slices
    for (auto& profile : j.items()) {
        std::string name = getSliceName(profile.key());
        std::pair<int, int> declLine = getSliceDeclLine(profile.key());
        std::string file, hashStr;
        std::vector<std::pair<int, int>> defs, uses;

        // look for the def and uses
        json sliceJSON = profile.value();
        for (auto& property : sliceJSON.items()) {
            std::string attribute = property.key();
            auto value = property.value();

            // used for creating the hash
            if (attribute == "file") {
                // grabs the file name
                file = value;

                // converts into single hash-able string
                std::string declLineString = std::to_string(declLine.first);
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

            // adds the def lines
            if (attribute == "definition") {
                for (auto& line : value) {
                    // std::cout << line << std::endl;
                    defs.push_back(spliceLineData(line));
                }
            }

            // adds the use lines
            if (attribute == "use") {
                for (auto& line : value) {
                    // std::cout << line << std::endl;
                    uses.push_back(spliceLineData(line));
                }
            }
        }
        SliceProfile s(name, declLine, hashStr, defs, uses);
        slices.push_back(s);
    }
}