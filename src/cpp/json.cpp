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

SliceProfile::SliceProfile(std::string sliceName, int decl, std::string hash, std::vector<int> defs, std::vector<int> uses) {
    sliceName_ = sliceName;
    declLine_ = decl;
    hash_ = hash;
    defLines_ = defs;
    useLines_ = uses;
}

void SliceProfile::addDef(const int& line) {
    defLines_.push_back(line);
}

void SliceProfile::addUse(const int& line) {
    useLines_.push_back(line);
}

std::string SliceProfile::getName() const {
    return sliceName_;
}

int SliceProfile::getDecl() const {
    return declLine_;
}

std::string SliceProfile::getHash() const {
    return hash_;
}

std::vector<int> SliceProfile::getDefs() const {
    return defLines_;
}

std::vector<int> SliceProfile::getUses() const {
    return useLines_;
}

// prints out slice profile data (neatly)
void SliceProfile::print() const {
    std::cout << "Slice: " << sliceName_ << " | Declared on Line: " << declLine_ << std::endl;
    std::cout << "Hash: " << hash_ << std::endl;
    std::cout << "Definition Lines: ";
    for (auto& line : defLines_) {
        std::cout << line << " ";
    }
    std::cout << std::endl;
    std::cout << "Use Lines: ";
    for (auto& line : useLines_) {
        std::cout << line << " ";
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
int getSliceDeclLine(std::string key) {
    // Gets the index of the slice name's final character
    std::string sliceName = getSliceName(key);
    int strStart = sliceName.length() + 1;

    // creates substring starting at the index after the first '-' (the decl line number) 
    std::string sliceLineStr = getSliceName(key.substr(strStart, key.length()));
    int sliceLine = std::stoi(sliceLineStr);
    return sliceLine;
}

// adds all the completed slice profiles to the vector<SliceProfile>
void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices) {
    // takes every object in json, adds them to vector of slices
    for (auto& profile : j.items()) {
        std::string name = getSliceName(profile.key());
        int declLine = getSliceDeclLine(profile.key());
        std::string file, hashStr;
        std::vector<int> defs, uses;

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
                std::string declLineString = std::to_string(declLine);
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
                    defs.push_back(line);
                }
            }

            // adds the use lines
            if (attribute == "use") {
                for (auto& line : value) {
                    uses.push_back(line);
                }
            }
        }
        SliceProfile s(name, declLine, hashStr, defs, uses);
        slices.push_back(s);
    }
}