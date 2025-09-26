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
    SliceProfile(std::string sliceName, int decl, std::string hash, std::vector<int> defs, std::vector<int> uses);

    void setName(const std::string& name)   {sliceName_ = name;}
    void setDecl(const int& line)           {declLine_ = line;}
    void setDefs(std::vector<int> lines)    {defLines_ = lines;} 
    void setUses(std::vector<int> lines)    {useLines_ = lines;}

    void addDef(const int& line);
    void addUse(const int& line);

    std::string getName() const;
    int getDecl() const;
    std::string getHash() const;
    std::vector<int> getDefs() const;
    std::vector<int> getUses() const;

    void print() const;

private:
    // Name (string)
    std::string sliceName_;
    // Decl (line number)
    int declLine_;
    // attribute hash
    std::string hash_;
    // Defs (line numbers)
    std::vector<int> defLines_;
    // Uses (line numbers)
    std::vector<int> useLines_;
};

std::string getSliceName(std::string);
int getSliceDeclLine(std::string);

void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices);

#endif // SLICE_JSON_HPP
