#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace {
    class line {
        std::string data;
    public:
        friend std::istream &operator>>(std::istream &is, line &l) {
            std::getline(is, l.data);
            return is;
        }
        operator std::string() const { return data; }    
    };

    auto open_argv_1(int argc, char **argv) {
        if (argc != 2) {
            throw std::runtime_error("Insufficient arguments");
        }

        std::ifstream f;
        f.open(argv[1]);
        return f;
    };

    std::ostream& bold_on(std::ostream& os) {
        return os << "\e[1m";
    }

    std::ostream& bold_off(std::ostream& os) {
        return os << "\e[0m";
    }

#ifndef NDEBUG
#define DEBUG(x) do { \
    x; \
} while (0)
#else
#define DEBUG(x)
#endif

};
