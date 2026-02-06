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

// finds current node's filename for multifile
std::string findFileName(xmlNodePtr current) {
    // checks to see if the current element is a unit file
    if (xmlStrcmp(current->name, (const xmlChar*)"unit") == 0) {
        // checks for filename
        if (xmlHasProp(current, (const xmlChar*)"filename") != NULL) {
            // if filename exists, grab it!!
            return std::string((const char*)xmlGetProp(current, (const xmlChar*)"filename"));
        }
    }
    // checks for parent so it can recurse
    if (current->parent != nullptr) {
        std::string result = findFileName(current->parent);
        if (result != "") {
            return result;
        }   
    }
    // if the following don't apply, return empty string
    return "";
}

xmlNodePtr SliceProfileHandler::findDecl(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int& hintColumn, const std::string& hintFile) {

    // BFS search
    // check start node
    // if decl, run operation
    // if not,
    //      check children, if not found, move next/

    // starts at the beginning of the file
    if (!start) {
        return nullptr;
    }

    xmlNodePtr current = start;
    // // std::cout << current->name << std::endl;

    while (current) {
        // if node is decl, get ns and look for position attributes
        if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
            // create ns ptr to the position namespace
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                // std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }
            // // std::cout << "Got the namespace: " << posNs->href << std::endl;

            // // std::cout << "In Loop..." << std::endl;
            // decl hit, check to see if it matches slice information
            // // std::cout << "In Loop, found node..." << std::endl;
            if (xmlStrcmp(current->name, (const xmlChar *)"decl") == 0) {
                // get the current node's pos:begin and end values
                // // std::cout << "Found " << current->name << " at line: " << xmlGetLineNo(current) << "... Before getting NsProps" << std::endl;

                // checks to find if the current file name is the same as the slice's file
                // std::cout << "checking for same filename: current: " << findFileName(current) << " hintFile: " << hintFile << std::endl;
                if (findFileName(current) == hintFile) {
                    // std::cout << "yep, worked for: " << findFileName(current) << " " << hintFile << std::endl;
                    auto beginValue = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                    auto endValue = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);

                    if (!beginValue || !endValue) {
                        // std::cout << "start/end value null" << std::endl;
                        return nullptr;
                    }

                    // std::cout << "pos:start Value: " << beginValue << std::endl;
                    // std::cout << "pos:end Value: " << endValue << std::endl;
                    // std::cout << "After getting NsProps, getting json data" << std::endl;
                    json b = {(const char *)beginValue}, e = {(const char *)endValue};

                    // std::cout << "json made, splicing line data" << std::endl;
                    std::pair<int, int> beginPair = spliceLineData(b);
                    std::pair<int, int> endPair = spliceLineData(e);

                    // std::cout << "beginPair/endPair good!" << std::endl;
                    // // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                    // // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;
                    // std::cout << "line: " << beginPair.first << " <= " << hintLine << " <= " << endPair.first << std::endl;
                    // std::cout << "colm: " << beginPair.second << " <= " << hintColumn << " <= " << endPair.second << std::endl;
                    if ((hintLine >= beginPair.first && hintLine <= endPair.first) && (hintColumn >= beginPair.second && hintColumn <= endPair.second)) {
                        // std::cout << "findDecl meets the requirements. Slice " << slice.getName() << " placed on line " << current->line << std::endl;
                        addAttribute(slice, current, "decl");
                        return current;
                    }
                }
            }
        }
        // if node is not decl, search children, then search next.
        else {
            if (current->children) {
                xmlNodePtr result = this->findDecl(slice, current->children, hintLine, hintColumn, hintFile);
                if (result != nullptr) {
                    return result;
                }
            }
        }
        current = current->next;
    }

    // if decl not found, return nullptr
    return nullptr;
}

xmlNodePtr SliceProfileHandler::findUse(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn, const std::string& hintFile) {
    if (!start) {
        return nullptr;
    }
    xmlNodePtr current = start;

    while (current) {
        // expr
        if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                // std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }

            // std::cout << "expr: " << (findFileName(current) == hintFile) << std::endl;
            // std::cout << (findFileName(current) >= hintFile ? "findFileName greater" : "hintfile greater") << std::endl;
            // std::cout << findFileName(current) << " " << hintFile << std::endl;

            if (findFileName(current) == hintFile) {
                // get the current node's pos:begin and end values
                auto beginValue = xmlGetNsProp(current, (const xmlChar *)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar *)"end", posNs->href);
                json b = {(const char *)beginValue}, e = {(const char *)endValue};
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

                if ((hintLine >= beginPair.first && hintLine <= endPair.first) && (hintColumn >= beginPair.second && hintColumn <= endPair.second)) {
                    // std::cout << "(expr)Adding slice " << slice.getName() << " with hash " << slice.getHash() << std::endl;
                    addAttribute(slice, current, "use");
                    return current;
                }
            }
        }
        // decl
        else if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                // std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }
            
            // checks to see if the filename matches the current slice line
            // std::cout << "decl: " << (findFileName(current) == hintFile) << std::endl;
            // std::cout << (findFileName(current) >= hintFile ? "findFileName greater" : "hintfile greater") << std::endl;
            // std::cout << findFileName(current) << " " << hintFile << std::endl;

            if (findFileName(current) == hintFile) {
                // get the current node's pos:begin and end values
                auto beginValue = xmlGetNsProp(current, (const xmlChar *)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar *)"end", posNs->href);
                json b = {(const char *)beginValue}, e = {(const char *)endValue};
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;
                
                if ((hintLine >= beginPair.first && hintLine <= endPair.first) && (hintColumn >= beginPair.second && hintColumn <= endPair.second)) {
                    // std::cout << "(decl)Adding slice " << slice.getName() << " with hash " << slice.getHash() << std::endl;
                    addAttribute(slice, current, "use");
                    return current;
                }
            }
        }
        else {
            if (current->children) {
                xmlNodePtr result = findUse(slice, current->children, hintLine, hintColumn, hintFile);
                if (result != nullptr) {
                    return result;
                }
            }
        }
        current = current->next;
    }
    return nullptr;
}

xmlNodePtr SliceProfileHandler::findDef(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn, const std::string& hintFile) {
    if (!start) {
        return nullptr;
    }
    xmlNodePtr current = start;

    while (current) {
        if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                // // std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }

            // get the current node's pos:begin and end values
            auto beginValue = xmlGetNsProp(current, (const xmlChar *)"start", posNs->href);
            auto endValue = xmlGetNsProp(current, (const xmlChar *)"end", posNs->href);
            json b = {(const char *)beginValue}, e = {(const char *)endValue};
            std::pair<int, int> beginPair = spliceLineData(b);
            std::pair<int, int> endPair = spliceLineData(e);

            // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
            // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

            if (findFileName(current) == hintFile) {
                if ((hintLine >= beginPair.first && hintLine <= endPair.first) && (hintColumn >= beginPair.second && hintColumn <= endPair.second)) {
                    // // std::cout << "IT WORKS FOR DEF!!" << std::endl;
                    addAttribute(slice, current, "def");
                    return current;
                }
            }
        }
        else {
            if (current->children) {
                xmlNodePtr result = findDef(slice, current->children, hintLine, hintColumn, hintFile);
                if (result != nullptr) {
                    return result;
                }
            }
        }
        current = current->next;
    }
    
    return nullptr;
}

// adds the attribute at a location
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
                // checks to see if the hash is already present
                if (containsHash(oldValue, hash)) {
                    // dont put in the new hash
                    return location;
                }
                hash = std::string((const char *)oldValue) + " " + hash;
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
                    newValue = std::string((const char*)oldValue) + " " + hash;
                    // std::cout << newValue << std::endl;
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

// used in checking if a hash is already present within an attribute's
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
    // std::cout << "Made Ns namespace" << std::endl;

    for (const auto& sp : slices.profiles_) {
        // step 1: decl elements
        // std::cout << "Slice Profile: " << sp.getName() << std::endl;
        
        // gets root element
        xmlNodePtr root = xmlDocGetRootElement(doc);
        // finds decl line
        // std::cout << "trying findDecl for profile " << sp.getName() << std::endl;
        // std::cout << sp.getDecl()->getFile() << std::endl;
        if (slices.findDecl(sp, root, sp.getDecl()->getLine().first, sp.getDecl()->getLine().second, sp.getDecl()->getFile())) {
            // std::cout << "findDecl for " << sp.getName() << " success." << std::endl;
        }
        
        // step 2: defs
        for (const auto& def : sp.getDefs()) {
            root = xmlDocGetRootElement(doc);
            // std::cout << "trying findDef for " << sp.getName() << std::endl;
            if (!slices.findDef(sp, root, def->getLine().first, def->getLine().second, def->getFile())) {
                // std::cout << "findDef did not work for: " << def->getLine().first << ", " << def->getLine().second << std::endl;
                if (def->getLine().first == sp.getDecl()->getLine().first && def->getLine().second == sp.getDecl()->getLine().second) {}
                    // std::cout << "Reason: Same as decl line" << std::endl;
            }
        }

        // step 3: uses
        for (const auto& use : sp.getUses()) {
            root = xmlDocGetRootElement(doc);
            // std::cout << "trying findUse for " << sp.getName() << std::endl;
            if (!slices.findUse(sp, root, use->getLine().first, use->getLine().second, use->getFile())) {
                // std::cout << "findUse did not work for: " << use->getLine().first << ", " << use->getLine().second << std::endl;
            }
        }
    }

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}