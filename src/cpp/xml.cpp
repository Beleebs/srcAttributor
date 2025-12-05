#include "xml.hpp"
#include "json.hpp"

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

// used to find the location of a parent decl element, if it exists.
xmlNodePtr findDeclParent(xmlNodePtr location) {
    if (!location) {
        return nullptr;
    }
    if (xmlStrcmp(location->name, (const xmlChar*)"decl") == 0) {
        return location;
    }
    if (location->parent) {
        return findDeclParent(location->parent);
    }
    return nullptr;
}

// recursive content finder
std::vector<std::string> getContents(xmlNodePtr parent) {
    std::vector<std::string> result;
    for (xmlNodePtr current = parent; current; current = current->next) {
        // If the node is a text node, add its content
        if (current->type == XML_TEXT_NODE) {
            std::string content = (const char*)xmlNodeGetContent(current);
            if (!content.empty()) {
                result.push_back(content);
            }
        }
        // If the node is an element node, recursively get contents from its children
        if (current->type == XML_ELEMENT_NODE && current->children) {
            std::vector<std::string> childContents = getContents(current->children);
            result.insert(result.end(), childContents.begin(), childContents.end());
        }
    }
    return result;
}

// checks to see if the expr location is a conditional
// has to do with a wacky thing where assignment CAN be used in the conditional *sigh*
bool checkConditional(xmlNodePtr location) {
    std::vector<std::string> contents = getContents(location);
    if (location->parent && xmlStrcmp(location->parent->name, (const xmlChar*)"condition") == 0) {
        for (auto c : contents) {
            if (c == "= " || c == "=") {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SliceProfileHandler::parseExpr(const SliceProfile &slice, xmlNodePtr exprStart, const char* type) {
    // starts on an expr statement, outputs the contents of expr
    const char* sliceName = slice.getName().c_str();
    for (xmlNodePtr current = exprStart; current; current = current->next) {
        // problem found. this stupid line right here
        std::vector<std::string> contents = getContents(exprStart);
        for(auto str : contents) {
            if (xmlStrcmp((const xmlChar*)sliceName, (const xmlChar*)str.c_str()) == 0) {
                // std::cout << "true: parseExpr(" << type << ") = " << str << std::endl;
                // this checks to see if the expr is in a <decl>
                // if it is inside a decl, then place the stuff in decl instead!!!! BOOOOOOOOOOM!!!!!!!
                xmlNodePtr parentDecl = findDeclParent(exprStart);
                if (parentDecl) {
                    addAttribute(slice, parentDecl, type);
                }
                else {
                    addAttribute(slice, exprStart, type);
                }
                return true;
            }
        }
    }
    return false;
}

bool SliceProfileHandler::parseDecl(const SliceProfile &slice, xmlNodePtr declStart, const char* type) {
    // starts on a decl statement, outputs the contents of decl
    const char* sliceName = slice.getName().c_str();
    for (xmlNodePtr current = declStart; current; current = current->next) {
        const xmlChar* content = xmlNodeGetContent(current);
        if (xmlStrcmp((const xmlChar*)sliceName, content) == 0) {
            // std::cout << "true: parseDecl() = " << content << std::endl;
            addAttribute(slice, declStart->parent, type);
            return true;
        }
        this->parseDecl(slice, current->children, type);
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
                if (this->parseDecl(slice, current, "decl") == true) {
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

xmlNodePtr SliceProfileHandler::findUse(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn) {
    // starts at the beginning of the file
    for (xmlNodePtr current = start; current; current = current->next) {
        if (current->type == XML_ELEMENT_NODE) {
            // std::cout << "expr " << hintLine << std::endl;

            // get the pos:start value
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");

            // decl hit
            if ((xmlStrcmp(current->name, (const xmlChar*)"decl") == 0 && xmlGetLineNo(current) == hintLine + 1)) {
                auto v = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);
                std::string posValue = "\"" + std::string((const char*)v) + "\"";
                    
                // convert to int pair
                json j = {posValue};
                auto posPair = spliceLineData(j);
                std::cout << posPair.second << std::endl;

                if (this->parseDecl(slice, current, "use") && posPair.second <= hintColumn) {
                    return current;
                }                
            }
            // expr hit
            if ((xmlStrcmp(current->name, (const xmlChar*)"expr") == 0 && xmlGetLineNo(current) == hintLine + 1))   {
                std::cout << "attempting parseExpr with element " << current->name << ": ";// << std::endl;

                auto v = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);
                std::string posValue = (const char*)v;
                std::cout << posValue << std::endl;
                    
                // convert to int pair
                json j = posValue;
                auto posPair = spliceLineData(j);
                std::cout << posPair.second << std::endl;

                if (this->parseExpr(slice, current, "use") == true && posPair.second <= hintColumn) {
                    std::cout << "success " << current->line << std::endl;
                    return current;
                }
                else {
                    std::cout << "fail" << std::endl;
                }
            }
        }
        this->findUse(slice, current->children, hintLine, hintColumn);
    }
    return nullptr;
}

xmlNodePtr SliceProfileHandler::findDef(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn) {
    // starts at the beginning of the file
    for (xmlNodePtr current = start; current; current = current->next) {
        if (current->type == XML_ELEMENT_NODE) {
            // std::cout << "decl" << hintLine << std::endl;
            // decl hit, skip
            if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
                break;
            }
            // expr hit
            if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0 && xmlGetLineNo(current) == hintLine + 1) {
                std::cout << "attempting parseExpr with element " << current->name << ": ";// << std::endl;
                
                // THIS IS A HORRID SOLUTION.
                // BY NO MEANS IS THIS EFFICIENT,
                // BUT IT WORKS.

                // get the pos:start value
                xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
                auto v = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                std::string posValue = (const char*)v;
                
                // convert to int pair
                json j = {posValue};
                auto posPair = spliceLineData(j);

                // compare columns
                if (posPair.second == hintColumn) {
                    if (this->parseExpr(slice, current, "def")) {
                        std::cout << "success " << current->line << std::endl;
                        return current;
                    }
                    else {
                        std::cout << "fail " << std::endl;
                    }
                }
                else {
                    std::cout << "Failed hint column check" << std::endl;
                }
            }
        }
        this->findDef(slice, current->children, hintLine, hintColumn);
    }
    return nullptr;
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
        // NEEDS to be in an expr element
        if (xmlStrcmp(location->name, (const xmlChar *)"expr") == 0) {
            // if the node already has a def, concatenate
            if (xmlHasNsProp(location, (const xmlChar *)"def", xmlNamespace->href) != NULL) {
                const xmlChar *oldValue = xmlGetNsProp(location, (const xmlChar *)"def", xmlNamespace->href);
                hash = hash + " " + std::string((const char *)oldValue);
                xmlSetNsProp(location, xmlNamespace, (const xmlChar *)"def", (const xmlChar *)hash.c_str());
                return location;
            }
            // no def yet
            else {
                xmlSetNsProp(location, xmlNamespace, (const xmlChar *)"def", (const xmlChar *)hash.c_str());
            }
        }
    }

    if (attType == "use") {
        // checks to see if the current location is in an <expr> or <decl>
        if (xmlStrcmp(location->name, (const xmlChar*)"expr") == 0 || xmlStrcmp(location->name, (const xmlChar*)"decl") == 0) {
            // if the node has a decl
            if (xmlHasNsProp(location, (const xmlChar*)"decl", xmlNamespace->href) != NULL) {
                const xmlChar* declValue = xmlGetNsProp(location, (const xmlChar*)"decl", xmlNamespace->href);
                // checks if there is a hash in def with the same hash
                if (containsHash(declValue, hash)) {
                    // if there is, just return and exit function
                    return location;
                }
                // no decl hash matches? all good!!! move on.
            }
            // if the node has a def
            if (xmlHasNsProp(location, (const xmlChar*)"def", xmlNamespace->href) != NULL) {
                const xmlChar* defValue = xmlGetNsProp(location, (const xmlChar*)"def", xmlNamespace->href);
                // checks if there is a hash in def with the same hash
                if (containsHash(defValue, hash)) {
                    // if there is, just return and exit function
                    return location;
                }
                // no def hash matches? all good!!! move on.
            }
            // if the node already has a use
            if (xmlHasNsProp(location, (const xmlChar*)"use", xmlNamespace->href) != NULL) {
                const xmlChar* oldValue = xmlGetNsProp(location, (const xmlChar*)"use", xmlNamespace->href);
                std::string newValue = (const char*)oldValue;
                // check if the hash is already present
                if (!containsHash(oldValue, hash)) {
                    // if it is, set the new value to the hash + old value
                    newValue = hash + ' ' + (const char*)oldValue;
                }
                // set prop
                xmlSetNsProp(location, xmlNamespace, (const xmlChar*)"use", (const xmlChar*)newValue.c_str());
                return location;
            }
            // if there is no use
            else {
                xmlSetNsProp(location, xmlNamespace, (const xmlChar*)"use", (const xmlChar*)hash.c_str());
            }
        }
    }
    return location;
}

bool containsHash(const xmlChar* attributeValue, std::string hash) {
    std::string valueString = (const char*)attributeValue;
    std::vector<std::string> hashes;
    std::string foundHash = "";
    size_t pos = 0;

    while (pos < valueString.size()) {
        if (valueString[pos] == ' ') {
            hashes.push_back(foundHash);
            foundHash = "";
        } 
        else {
            foundHash += valueString[pos];
        }
        ++pos;
    }
    if (!foundHash.empty()) {
        hashes.push_back(foundHash);
    }

    for (auto h : hashes) {
        if (h == hash)
            return true;
    }
    return false;
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
        // std::cout << "Slice Profile: " << sp.getName() << std::endl;
        
        // gets root element
        xmlNodePtr root = xmlDocGetRootElement(doc);
        // finds decl line
        int decl = sp.getDecl();
        slices.findDecl(sp, root, decl);
        
        // step 2: defs
        for (const auto& def : sp.getDefs()) {
            root = xmlDocGetRootElement(doc);
            slices.findDef(sp, root, def.first, def.second);
        }

        // step 3: uses
        for (const auto& use : sp.getUses()) {
            root = xmlDocGetRootElement(doc);
            slices.findUse(sp, root, use.first, use.second);
        }
    }

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}