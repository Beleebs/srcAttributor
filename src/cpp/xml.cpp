#include "xml.hpp"

void SliceProfileHandler::addProfiles(SliceProfile item) {
    profiles_.push_back(item);
}

void SliceProfileHandler::addProfiles(std::vector<SliceProfile> items) {
    profiles_ = items;
}

void SliceProfileHandler::print() const {
    for (const auto& e : profiles_) 
        e.print();
}

bool SliceProfileHandler::parseExpr(const SliceProfile &slice, xmlNodePtr exprStart, const char* type) {
    // starts on an expr statement, outputs the contents of expr
    const char* sliceName = slice.getName().c_str();
    for (xmlNodePtr current = exprStart; current; current = current->next) {
        const xmlChar* content = xmlNodeGetContent(current);
        if (xmlStrcmp((const xmlChar*)sliceName, content) == 0) {
            std::cout << "true: parseExpr(" << type << ") = " << content << std::endl;
            addAttribute(slice, exprStart, type);
            return true;
        }
        this->parseExpr(slice, current->children, type);
    }
    return false;
}

bool SliceProfileHandler::parseDecl(const SliceProfile &slice, xmlNodePtr declStart) {
    // starts on a decl statement, outputs the contents of decl
    const char* sliceName = slice.getName().c_str();
    for (xmlNodePtr current = declStart; current; current = current->next) {
        const xmlChar* content = xmlNodeGetContent(current);
        if (xmlStrcmp((const xmlChar*)sliceName, content) == 0) {
            // std::cout << "true: parseDecl() = " << content << std::endl;
            addAttribute(slice, declStart->parent, "decl");
            return true;
        }
        this->parseDecl(slice, current->children);
    }
    return false;
}

xmlNodePtr SliceProfileHandler::findDecl(const SliceProfile &slice, xmlNodePtr start, const int &hintLine)
{
    // starts at the beginning of the file
    for (xmlNodePtr current = start; current; current = current->next) {
        if (current->type == XML_ELEMENT_NODE) {
            // decl hit
            if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0 ) {
                // parse the decl
                // std::cout << "decl found on line: " << current->line << std::endl;
                // if the decl is part of the slice/where the slice is declared, return current
                if (this->parseDecl(slice, current) == true) {
                    return current;
                }
            }
        }
        // Recurse into children
        xmlNodePtr result = this->findDecl(slice, current->children, hintLine);
        if (result != nullptr) {
            return result;
        }
    }
    // if decl not found, return nullptr
    return nullptr;
}

xmlNodePtr SliceProfileHandler::findUse(const SliceProfile &slice, xmlNodePtr start, const int &hintLine) {
    // starts at the beginning of the file
    for (xmlNodePtr current = start; current; current = current->next) {
        if (current->type == XML_ELEMENT_NODE) {
            // expr hit
            if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0 && xmlGetLineNo(current) == hintLine + 1) {
                if (this->parseExpr(slice, current, "use") == true) {
                    std::cout << "expr found on line: " << current->line << std::endl;
                    return current;
                }
            }
        }
        this->findUse(slice, current->children, hintLine);
    }
    return start;
}

xmlNodePtr SliceProfileHandler::findDef(const SliceProfile &slice, xmlNodePtr start, const int &hintLine) {
    // starts at the beginning of the file
    for (xmlNodePtr current = start; current; current = current->next) {
        if (current->type == XML_ELEMENT_NODE) {
            // expr hit
            if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0 && xmlGetLineNo(current) == hintLine + 1) {
                if (this->parseExpr(slice, current, "def") == true) {
                    std::cout << "expr found on line: " << current->line << std::endl;
                    return current;
                }
            }
        }
        this->findDef(slice, current->children, hintLine);
    }
    return start;
}

xmlNodePtr addAttribute(const SliceProfile &slice, xmlNodePtr location, const char* attType) {
    xmlNsPtr xmlNamespace = xmlSearchNs(location->doc, location, (const xmlChar*)"slice");
    std::string hash = slice.getHash();

    if (attType == "decl") {
        // checks to see if the location is a decl or not
        // prevents multiple decl's from appearing where they are not supposed to
        if (xmlStrcmp(location->name, (const xmlChar*)"decl") == 0) {
            xmlSetNsProp(location, xmlNamespace, (const xmlChar*)"decl", (const xmlChar*)hash.c_str());
        }
    }

    if (attType == "def") {
        std::cout << location->name << std::endl;
        if (xmlStrcmp(location->name, (const xmlChar*)"expr") == 0) {
            // if the node already has a def, concatenate
            if (xmlHasNsProp(location, (const xmlChar*)"def", xmlNamespace->href) != NULL) {
                const xmlChar* oldValue = xmlGetNsProp(location, (const xmlChar*)"def", xmlNamespace->href);
                hash = hash + " " + std::string((const char*)oldValue);
            }
            xmlSetNsProp(location, xmlNamespace, (const xmlChar*)"def", (const xmlChar*)hash.c_str());
        }
    }

    if (attType == "use") {
        std::cout << location->name << std::endl;
        if (xmlStrcmp(location->name, (const xmlChar*)"expr") == 0) {
            // if the node already has a use, concatenate
            if (xmlHasNsProp(location, (const xmlChar*)"use", xmlNamespace->href) != NULL) {
                const xmlChar* oldValue = xmlGetNsProp(location, (const xmlChar*)"use", xmlNamespace->href);
                if (xmlStrcmp(oldValue, (const xmlChar*)hash.c_str()) != 0) {
                    hash = hash + " " + std::string((const char*)oldValue);
                }
            }
            xmlSetNsProp(location, xmlNamespace, (const xmlChar*)"use", (const xmlChar*)hash.c_str());
        }
    }
    return location;
}

void insertAttributes(SliceProfileHandler& slices, std::string xmlFileName) {
    // xml file open
    const char* file = xmlFileName.c_str();
    xmlDocPtr doc = xmlReadFile(file, NULL, XML_PARSE_BIG_LINES);

    // creates slice namespace if not already in there
    xmlNodePtr nsRoot = xmlDocGetRootElement(doc);
    if (xmlSearchNs(doc, nsRoot, (const xmlChar*)"slice") == NULL) {
        xmlNewNs(nsRoot, (const xmlChar*)"http://www.srcML.org/srcML/slice", (const xmlChar*)"slice");
    }

    for (const auto& sp : slices.profiles_) {
        // step 1: decl elements
        std::cout << "Slice Profile: " << sp.getName() << std::endl;
        
        // gets root element
        xmlNodePtr root = xmlDocGetRootElement(doc);
        // finds decl line
        int decl = sp.getDecl();
        slices.findDecl(sp, root, decl);
        

        // step 2: defs
        for (const auto& def : sp.getDefs()) {
            root = xmlDocGetRootElement(doc);
            slices.findDef(sp, root, def);
        }

        // step 3: uses
        for (const auto& use : sp.getDefs()) {
            root = xmlDocGetRootElement(doc);
            slices.findUse(sp, root, use);
        }
    }

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}