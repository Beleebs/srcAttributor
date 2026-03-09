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
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "cli11.hpp"

using json = nlohmann::json;

// Slice appearance 
// can be used for decl/def/use

class SliceLine {
public:
    SliceLine() : row_(), column_(), fileName_("") {}
    SliceLine(int r, int c, std::string f="");

    std::string getFile() const {return fileName_;}
    std::pair<int, int> getLine() const {return std::make_pair(row_, column_);}
    void setFile(std::string s) {fileName_ = s;}
    bool operator==(const SliceLine&) const noexcept;

    std::string print() const;

private:
    int row_;
    int column_;
    std::string fileName_;
};

// hash function for SliceLine
// boost style hash function 
namespace std {
    template <> struct hash<SliceLine> { 
        size_t operator() (const SliceLine& obj) const noexcept { 
            size_t seed = 0;

            size_t h1 = hash<int>()(obj.getLine().first);
            size_t h2 = hash<int>()(obj.getLine().second);
            size_t h3 = hash<std::string>()(obj.getFile());

            seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}

// Holds slice profile information.
// usually will be gathered by json parsing.
class SliceProfile {
public:
    SliceProfile() : sliceName_(""), fileName_(""), declLine_(), hash_(""), defLines_(), useLines_(){}
    SliceProfile(std::string& sliceName, std::string& fileName, SliceLine decl, std::string& hash, std::vector<SliceLine> defs, std::vector<SliceLine> uses);

    void setName(const std::string& name)          {sliceName_ = name;}
    void setDecl(SliceLine line)                   {declLine_ = line;}
    void setDefs(std::vector<SliceLine> lines)     {defLines_ = lines;} 
    void setUses(std::vector<SliceLine> lines)     {useLines_ = lines;}

    void addDef(SliceLine s);
    void addUse(SliceLine s);

    std::string getName() const;
    std::string getFile() const;
    SliceLine getDecl() const;
    std::string getHash() const;
    std::vector<SliceLine> getDefs() const;
    std::vector<SliceLine> getUses() const;

    void print() const;

private:
    // slice name (string)
    std::string sliceName_;
    // originating file (string) 
    std::string fileName_;
    // declaration line
    SliceLine declLine_;
    // attribute hash
    std::string hash_;
    // lines where slice is (re)defined
    std::vector<SliceLine> defLines_;
    // lines where slice is used
    std::vector<SliceLine> useLines_;

};

std::string getSliceName(std::string);
SliceLine getSliceDeclLine(std::string);
void getSliceProfiles(const json& j, std::vector<SliceProfile>& slices);
std::pair<int, int> spliceLineData(json& j);
SliceLine returnLineData(json& j);

#endif // SLICE_JSON_HPP
