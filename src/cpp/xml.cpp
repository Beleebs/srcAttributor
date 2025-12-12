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

xmlNodePtr SliceProfileHandler::findDecl(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int& hintColumn) {

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
    // std::cout << current->name << std::endl;

    while (current) {
        // if node is decl, get ns and look for position attributes
        if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
            // create ns ptr to the position namespace
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }
            // std::cout << "Got the namespace: " << posNs->href << std::endl;

            // std::cout << "In Loop..." << std::endl;
            // decl hit, check to see if it matches slice information
            // std::cout << "In Loop, found node..." << std::endl;
            if (xmlStrcmp(current->name, (const xmlChar *)"decl") == 0 && xmlGetLineNo(current) == hintLine + 1) {
                // get the current node's pos:begin and end values
                // std::cout << "Found " << current->name << " at line: " << xmlGetLineNo(current) << "... Before getting NsProps" << std::endl;

                auto beginValue = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);

                if (!beginValue || !endValue) {
                    std::cout << "start/end value null" << std::endl;
                    return nullptr;
                }

                std::cout << "Begin Value: " << beginValue << std::endl;
                std::cout << "End Value: " << endValue << std::endl;
                std::cout << "After getting NsProps, getting json data" << std::endl;
                json b = {(const char *)beginValue}, e = {(const char *)endValue};

                std::cout << "json made, splicing line data" << std::endl;
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                std::cout << "beginPair/endPair good!" << std::endl;
                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

                if (hintColumn >= beginPair.second && hintColumn <= endPair.second) {
                    std::cout << "hintColumn meets the requirements. mods, place it on line " << hintLine + 1 << std::endl;
                    addAttribute(slice, current, "decl");
                    return current;
                }
            }
        }
        // if node is not decl, search children, then search next.
        else {
            if (current->children) {
                xmlNodePtr result = this->findDecl(slice, current->children, hintLine, hintColumn);
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

xmlNodePtr SliceProfileHandler::findUse(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn) {
    if (!start) {
        return nullptr;
    }
    xmlNodePtr current = start;

    while (current) {
        if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }

            if (xmlGetLineNo(current) == hintLine + 1) {
                // get the current node's pos:begin and end values
                auto beginValue = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);
                json b = {(const char*)beginValue}, e = {(const char*)endValue};
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

                if (hintColumn >= beginPair.second && hintColumn <= endPair.second) {
                    // std::cout << "It works for findUse (expr). INSERT!!!!" << std::endl;
                    addAttribute(slice, current, "use");
                    return current;
                }
            }
        }
        else if (xmlStrcmp(current->name, (const xmlChar*)"decl") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }

            if (xmlGetLineNo(current) == hintLine + 1) {
                // get the current node's pos:begin and end values
                auto beginValue = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);
                json b = {(const char*)beginValue}, e = {(const char*)endValue};
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

                if (hintColumn >= beginPair.second && hintColumn <= endPair.second) {
                    // std::cout << "It works for findUse (decl). INSERT!!!!" << std::endl;
                    addAttribute(slice, current, "use");
                    return current;
                }
            }
        }
        else {
            if (current->children) {
                xmlNodePtr result = findUse(slice, current->children, hintLine, hintColumn);
                if (result != nullptr) {
                    return result;
                }
            }
        }
        current = current->next;
    }
    return nullptr;
}

xmlNodePtr SliceProfileHandler::findDef(const SliceProfile &slice, xmlNodePtr start, const int &hintLine, const int &hintColumn) {
    if (!start) {
        return nullptr;
    }
    xmlNodePtr current = start;

    while (current) {
        if (xmlStrcmp(current->name, (const xmlChar*)"expr") == 0) {
            xmlNsPtr posNs = xmlSearchNs(current->doc, current, (const xmlChar*)"pos");
            if (!posNs) { 
                // std::cout << "Position namespace not found." << std::endl; 
                return nullptr;
            }

            if (xmlGetLineNo(current) == hintLine + 1) {
                // get the current node's pos:begin and end values
                auto beginValue = xmlGetNsProp(current, (const xmlChar*)"start", posNs->href);
                auto endValue = xmlGetNsProp(current, (const xmlChar*)"end", posNs->href);
                json b = {(const char*)beginValue}, e = {(const char*)endValue};
                std::pair<int, int> beginPair = spliceLineData(b);
                std::pair<int, int> endPair = spliceLineData(e);

                // std::cout << "beginPair: " << beginPair.first << " " << beginPair.second << std::endl;
                // std::cout << "endPair: " << endPair.first << " " << endPair.second << std::endl;

                if (hintColumn >= beginPair.second && hintColumn <= endPair.second) {
                    // std::cout << "IT WORKS FOR DEF!!" << std::endl;
                    addAttribute(slice, current, "def");
                    return current;
                }
            }
        }
        else {
            if (current->children) {
                xmlNodePtr result = findDef(slice, current->children, hintLine, hintColumn);
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
    std::cout << "Made Ns namespace" << std::endl;

    for (const auto& sp : slices.profiles_) {
        // step 1: decl elements
        std::cout << "Slice Profile: " << sp.getName() << std::endl;
        
        // gets root element
        xmlNodePtr root = xmlDocGetRootElement(doc);
        // finds decl line
        std::cout << "trying findDecl" << std::endl;
        if (slices.findDecl(sp, root, sp.getDecl().first, sp.getDecl().second)) {
            std::cout << "findDecl for " << sp.getName() << " success." << std::endl;
        }
        
        // step 2: defs
        for (const auto& def : sp.getDefs()) {
            root = xmlDocGetRootElement(doc);
            std::cout << "trying findDef for " << sp.getName() << std::endl;
            if (!slices.findDef(sp, root, def.first, def.second)) {
                std::cout << "findDef did not work for: " << def.first << ", " << def.second << std::endl;
                if (def.first == sp.getDecl().first && def.second == sp.getDecl().second) 
                    std::cout << "Reason: Same as decl line" << std::endl;
            }
        }

        // step 3: uses
        for (const auto& use : sp.getUses()) {
            root = xmlDocGetRootElement(doc);
            std::cout << "trying findUse for " << sp.getName() << std::endl;
            if (!slices.findUse(sp, root, use.first, use.second)) {
                std::cout << "findUse did not work for: " << use.first << ", " << use.second << std::endl;
            }
        }
    }

    // save n close
    xmlSaveFile(file, doc);
    xmlFreeDoc(doc);
}