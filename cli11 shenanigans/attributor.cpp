// Ben Leber
// 8/18/2025
// CLI11 Learning Process for srcAttributor

#include <iostream>
#include "CLI11.hpp"

int main(int argc, char **argv) {
    CLI::App app{"srcAttributor is a property generator based on srcSlice outputs.", "srcAttributor"};

    int p = 0;
    std::string file;
    app.add_option("-p", p, "Parameter");
    app.add_option("-i,--input", file, "File Input (Requires existing file)")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    std::cout << "Parameter value: " << p << std::endl;
    std::cout << "Filepath provided: " << file << std::endl;
    return 0;
}