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
    xmlNodePtr findDecl(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn);
    xmlNodePtr findUse(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn);
    xmlNodePtr findDef(const SliceProfile& slice, xmlNodePtr start, const int& hintLine, const int& hintColumn);

    // returns true if the slice's information is in the expr statement
    bool parseExpr (const SliceProfile& slice, xmlNodePtr exprStart, const char* type);

    // returns true if the slice's information matches with the content found in the decl element
    bool parseDecl (const SliceProfile& slice, xmlNodePtr declStart, const char* type);

    // inserts attributes found from findDecl/Use/Def
    friend void insertAttributes(SliceProfileHandler& ptrLocations, std::string xmlFileName);

private:
    std::vector<SliceProfile> profiles_;
};

// adds different kinds of attributes for the insertAttributes function
xmlNodePtr addAttribute(const SliceProfile& slice, xmlNodePtr location, const char* attType);

// checks for hash appearance in a const xmlChar*
bool containsHash(const xmlChar* attributeValue, std::string hash);

#endif // SLICE_XML_HPP