#ifndef SRCATTRIBUTOR
#define SRCATTRIBUTOR

#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <openxml.hpp>
#include <CLI11.hpp>

using json = nlohmann::json;

std::string getSliceName(std::string);
int getSliceDeclLine(std::string);

// static variables
const bool conciseMode = true;

#endif