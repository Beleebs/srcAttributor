#ifndef OPENXML_HPP
#define OPENXML_HPP

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <iostream>
#include <string>

// Function declarations

// parses for decl elements (used in AddDeclAttribute)
void FindDeclElement(xmlNodePtr parentNode, int declLine, std::string hash);
// adds the slice:decl attribute to each slice's <decl> element
void AddDeclAttribute(std::string filePath, std::string sliceName, int declLine);
// parses for expr elements (used in AddExprAttribute)
void FindExprElement(xmlNodePtr parentNode, std::string attributeType, int lineNumber, std::string hash);
// adds either slice:def or slice:use attributes to each slice's <expr> elements
void AddExprAttribute(std::string filePath, std::string sliceName, std::string attributeType, int lineNumber, std::string hash);
// generates a sha1 hash for slice attributes in the xml elements
std::string GenerateHash(std::string sliceName, int declLine, std::string filePath);
// adds the slice:* library to the xml
void AddXMLNamespace(std::string filename);

#endif