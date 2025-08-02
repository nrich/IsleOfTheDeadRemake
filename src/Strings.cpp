#include <fstream>
#include <exception>
#include <iostream>

#include "Strings.h"

static std::vector<std::string> strings_table;

void Strings::Extract(const std::string &filename) {
    std::ifstream fh(filename, std::ios::binary|std::ios::in);

    fh.seekg(0x33FD1, std::ios_base::beg);

    std::vector<char> current;

    while (!fh.eof()) {
        int position = fh.tellg();

        if (position > 0x36050)
            break;

        char c;

        fh.read(&c, 1);

        current.push_back(c);
        if (!c) {
            auto str = std::string(current.data());
            std::cout << strings_table.size() << " " << str << "\n";
            strings_table.push_back(str);
            current.clear();
            continue;
        }
    } 
}

std::string Strings::Lookup(const size_t index) {
    if (index > strings_table.size())
        throw std::out_of_range("Unknown strings lookup " + std::to_string(index)); 

    return strings_table[index];
}
