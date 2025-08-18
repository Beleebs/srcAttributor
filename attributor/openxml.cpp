#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <iostream>
#include <string>
#include <openssl/sha.h>
#include "openxml.hpp"

// Uses a root as the starting
void FindDeclElement(xmlNodePtr parentNode, int declLine, std::string hash) {
    // parse through each child node from the parent
    for (xmlNodePtr current = parentNode; current; current = current->next) {
        // checks the type of child, looks to see if its an element
        if (current->type == XML_ELEMENT_NODE) {
            // looks for decl element
            // xmlStrcmp compares the current node name with "decl". Returns 0 if they match, 1 otherwise
            if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
                // checks if its the correct decl element
                if (xmlGetLineNo(current) == declLine + 1) {
                    // Adds attribute
                    xmlSetProp(current, (const xmlChar*)"slice:decl", (const xmlChar*)hash.c_str());
                }
            }
        }

        // recursive call with the current node and its children
        if (current->children) {
            FindDeclElement(current->children, declLine, hash);
        }
    }
}

// finds expr elements
// similar to FindDeclElement
void FindExprElement(xmlNodePtr parentNode, std::string attributeType, int lineNumber, std::string hash) {
    bool isDeclStatement = false;
    bool sliceHasDef = false;
    for (xmlNodePtr current = parentNode; current; current = current->next) {
        xmlNsPtr xmlNamespace = xmlSearchNs(current->doc, current, (const xmlChar*)"slice");
        if (current->type == XML_ELEMENT_NODE) {
            // looks for expr element
            // this first if statement only applies if the outermost element is a declaration. we don't want to do anything with the expr then.
            if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
                isDeclStatement = true;
            }
            // isDeclStatement prevents the inner expr statements from being inserted into.
            if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0 && !isDeclStatement) {
                // checks if its the correct expr element
                if (xmlGetLineNo(current) == lineNumber + 1) {
                    // Adds either "slice:use" or "slice:def"
                    if (attributeType == "definition") {
                        // adds in the slice:def for the first time
                        if (xmlHasNsProp(current, (const xmlChar*)"def", (const xmlChar*)"http://www.srcML.org/srcML/slice") == NULL) {
                            // std::cout << "slice def not found." << std::endl;
                            xmlSetNsProp(current, xmlNamespace, (const xmlChar*)"def", (const xmlChar*)hash.c_str());
                        }
                        // if slice:def already exists, then the hash is concatenated.
                        else {
                            // std::cout << "slice def found!" << std::endl;
                            const xmlChar* oldValue = xmlGetNsProp(current, (const xmlChar*)"def", (const xmlChar*)"http://www.srcML.org/srcML/slice");
                            std::string updatedValue = std::string((const char*)oldValue) + " " + hash;
                            xmlSetNsProp(current, xmlNamespace, (const xmlChar*)"def", (const xmlChar*)updatedValue.c_str());
                        }
                    }
                    else if (attributeType == "use") {
                        // this if statement checks to see if the slice is trying to put a use into the same element as a def for the same slice.
                        // if it does, it sets sliceHasDef as true.
                        if (xmlHasNsProp(current, (const xmlChar*)"def", (const xmlChar*)"http://www.srcML.org/srcML/slice") != NULL) {
                            const xmlChar* defValue = xmlGetNsProp(current, (const xmlChar*)"def", (const xmlChar*)"http://www.srcML.org/srcML/slice");
                            std::string compareValue = (const char*)defValue;
                            if (compareValue == hash) {
                                sliceHasDef = true;
                            }
                        }
                        // adds in the slice:use for the first time
                        if (xmlHasNsProp(current, (const xmlChar*)"use", (const xmlChar*)"http://www.srcML.org/srcML/slice") == NULL) {
                            // std::cout << "slice use not found." << std::endl;
                            if (!sliceHasDef) {
                                xmlSetNsProp(current, xmlNamespace, (const xmlChar*)"use", (const xmlChar*)hash.c_str());
                            }
                        }
                        // if slice:use already exists, then the hash is concatenated.
                        else {
                            // std::cout << "slice use found." << std::endl;
                            if (!sliceHasDef) {
                                const xmlChar* oldValue = xmlGetNsProp(current, (const xmlChar*)"use", (const xmlChar*)"http://www.srcML.org/srcML/slice");
                                std::string updatedValue = std::string((const char*)oldValue) + " " + hash;
                                xmlSetNsProp(current, xmlNamespace, (const xmlChar*)"use", (const xmlChar*)updatedValue.c_str());
                            }
                        }
                    }
                    else {
                        std::cout << "Invalid Attribute Injection Type: " << attributeType << std::endl;
                    }
                }
            }
        }

        // recursive call with the current node and its children
        if (current->children) {
            FindExprElement(current->children, attributeType, lineNumber, hash);
        }
    }
}

// Generates sha1 hash from the parameters
std::string GenerateHash(std::string sliceName, int declLine, std::string filePath) {
    std::string declLineString = std::to_string(declLine);
    std::string input = sliceName + declLineString + filePath;

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    // Convert hash to hex string
    std::string hashStr;
    char buf[3];
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        hashStr += buf;
    }
    return hashStr;
}

// Finds the decl attribute and adds it
// First opens the file from the slice information. Next, the hash value is generated.
// Lastly, it parses to the line, finds the outer most decl statement, and adds attribute.
void AddDeclAttribute(std::string filePath, std::string sliceName, int declLine) {
    // format filepath for the xmlFile opening
    std::string xmlFile = filePath + ".xml";
    const char* file    = xmlFile.c_str();
    
    // open file
    xmlDocPtr   doc     = xmlReadFile(file, NULL, XML_PARSE_BIG_LINES);
    xmlNodePtr  root    = xmlDocGetRootElement(doc);

    // generate hash (uses the .cpp, not .cpp.xml, which is stored in filePath, not xmlFile)
    std::string hash    = GenerateHash(sliceName, declLine, filePath);

    // Find and add decl Element
    FindDeclElement(root, declLine, hash);

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}

void AddExprAttribute(std::string filePath, std::string sliceName, std::string attributeType, int lineNumber, std::string hash) {
    // format filepath for the xmlFile opening
    std::string xmlFile = filePath + ".xml";
    const char* file    = xmlFile.c_str();
    
    // open file
    xmlDocPtr   doc     = xmlReadFile(file, NULL, XML_PARSE_BIG_LINES);
    xmlNodePtr  root    = xmlDocGetRootElement(doc);

    // Find and add expr Element
    FindExprElement(root, attributeType, lineNumber, hash);

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}

void AddXMLNamespace(std::string filename) {
    std::string xmlFile = filename + ".xml";
    const char* file    = xmlFile.c_str();

    // open file
    xmlDocPtr   doc     = xmlReadFile(file, NULL, XML_PARSE_BIG_LINES);
    xmlNodePtr  root    = xmlDocGetRootElement(doc);

    // add namespace
    // looks for the slice namespace, if NULL then it is not there.
    if (xmlSearchNs(doc, root, (const xmlChar*)"slice") == NULL) {
        xmlNewNs(root, (const xmlChar*)"http://www.srcML.org/srcML/slice", (const xmlChar*)"slice");
    }

    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}