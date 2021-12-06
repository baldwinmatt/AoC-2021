#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

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
        if (argc < 2) {
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

namespace aoc {
    bool getline(std::istream& s, std::string& out, const std::string_view delims) {
        char c;
        out.resize(0);
        while (s.good() && (c = s.get())) {
            if (delims.find(c) != std::string_view::npos) {
                if (out.empty()) {
                    continue;
                }
                return s.good();
            }
            out.append(&c, 1);
        }
        return s.good();
    }
    bool getline(std::istream& s, std::string& out, const char delim) { return getline(s, out, std::string_view(&delim, 1)); }
};