#ifndef SLICE_XML_HPP
#define SLICE_XML_HPP

#include <iostream>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>
#include <map>
#include "json.hpp"

// Handles each slice profile
class SliceProfileHandler {
public:
    SliceProfileHandler() : profiles_() {}
    SliceProfileHandler(std::vector<SliceProfile> items) {profiles_ = items;}

    void addProfiles(SliceProfile item);
    void addProfiles(std::vector<SliceProfile> items);
    void print() const;

    // finds the xmlNodePtrs to different decl/expr statements
    xmlNodePtr findDecl(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn, const std::string& hintFile);
    xmlNodePtr findUse(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn, const std::string& hintFile);
    xmlNodePtr findDef(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn, const std::string& hintFile);

    // inserts attributes found from findDecl/Use/Def

private:
    std::vector<SliceProfile> profiles_;
};

// inserts the slice attributes at specified locations
void insertAttributes(std::vector<SliceProfile>& slices, std::unordered_map<SliceLine, xmlNodePtr>& positions);

// creates xmlNodePtr dictionary for current slices from XML File
void createNodeDictonary(std::vector<SliceProfile>& slices, xmlNodePtr current, std::unordered_map<SliceLine, xmlNodePtr>& dictionary, std::string& currentFile);

// adds different kinds of attributes for the insertAttributes function
xmlNodePtr addAttribute(const SliceProfile& slice, xmlNodePtr location, const char* attType);

// checks for hash appearance in a const xmlChar*
bool containsHash(const xmlChar* attributeValue, std::string hash);

// finds the filename the parser is currently in
std::string findFileName(xmlNodePtr current);

#endif // SLICE_XML_HPP