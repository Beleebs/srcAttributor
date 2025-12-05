#ifndef SLICE_JSON_HPP
#define SLICE_JSON_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>
#include "cli11.hpp"

using json = nlohmann::json;

// Holds slice profile information.
// usually will be gathered by json parsing.
class SliceProfile {
public:
    SliceProfile() : sliceName_(""), declLine_(0), hash_(""), defLines_(), useLines_() {}
    SliceProfile(std::string sliceName, int decl, std::string hash, std::vector<std::pair<int, int>> defs, std::vector<std::pair<int, int>> uses);

    void setName(const std::string& name)   {sliceName_ = name;}
    void setDecl(const int& line)           {declLine_ = line;}
    void setDefs(std::vector<std::pair<int, int>> lines)    {defLines_ = lines;} 
    void setUses(std::vector<std::pair<int, int>> lines)    {useLines_ = lines;}

    void addDef(const int& line, const int& column);
    void addUse(const int& line, const int& column);

    std::string getName() const;
    int getDecl() const;
    std::string getHash() const;
    std::vector<std::pair<int, int>> getDefs() const;
    std::vector<std::pair<int, int>> getUses() const;

    void print() const;

private:
    // needs overhauled, since position:column is now a thing
    // pairs?
    // std::vector<std::pair<int, int>> defLines_

    // Name (string)
    std::string sliceName_;
    // Decl (line number)
    int declLine_;
    // attribute hash
    std::string hash_;
    // Defs (line number, column)
    std::vector<std::pair<int, int>> defLines_;
    // Uses (line number, column)
    std::vector<std::pair<int, int>> useLines_;
};

std::string getSliceName(std::string);
int getSliceDeclLine(std::string);
void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices);
std::pair<int, int> spliceLineData(json& j);

#endif // SLICE_JSON_HPP
