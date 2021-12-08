#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <chrono>

#ifndef NDEBUG
#define DEBUG(x) do { \
    x; \
} while (0)
#else
#define DEBUG(x)
#endif

#define STRING_CONSTANT(symbol, value) constexpr std::string_view symbol(value)

namespace aoc {
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

    bool getline(std::istream& s, std::string& out, const std::string_view delims) {
        char c;
        out.resize(0);
        while (s.good() && (c = s.get())) {
            if (delims.find(c) != std::string_view::npos) {
                if (out.empty()) {
                    continue;
                }
                return true;
            }
            if (c > 0) {
                out.append(&c, 1);
            }
        }
        return !out.empty() || s.good();
    }
    bool getline(std::istream& s, std::string& out, const char delim) {
        return getline(s, out, std::string_view(&delim, 1));
    }
    bool getline(std::istream& s, std::string& out) {
        char c;
        out.resize(0);
        while (s.good() && (c = s.get())) {
            switch (c) {
                case '\r':
                case '\n':
                    if (out.empty()) { continue; }
                    return true;
                default:
                    if (c > 0) {
                        out.append(&c, 1);
                    }
                    break;
            }
        }
        return !out.empty() || s.good();
    }

    using UnaryIntFunction = std::function<void(int)>;
    void parse_as_integers(std::istream& s, const char delim, UnaryIntFunction op) {
        std::string l;
        while (getline(s, l, delim)) {
            try {
                int n = std::stoi(l);
                op(n);
            } catch (...) { }
        }
    }
    void parse_as_integers(std::istream& s, const std::string_view delims, UnaryIntFunction op) {
        std::string l;
        while (getline(s, l, delims)) {
            try {
                int n = std::stoi(l);
                op(n);
            } catch (...) { }
        }
    }
    void parse_as_integers(std::istream& s, UnaryIntFunction op) {
        std::string l;
        while (getline(s, l)) {
            try {
                int n = std::stoi(l);
                op(n);
            } catch (...) { }
        }
    }
    void parse_as_integers(const std::string& s, const char delim, UnaryIntFunction op) {
        std::stringstream ss(s);
        std::string l;
        while (getline(ss, l, delim)) {
            try {
                int n = std::stoi(l);
                op(n);
            } catch (...) { }
        }
    }
    void parse_as_integers(const std::string& s, const std::string_view delims, UnaryIntFunction op) {
        std::stringstream ss(s);
        std::string l;
        while (getline(ss, l, delims)) {
            try {
                int n = std::stoi(l);
                op(n);
            } catch (...) { }
        }
    }

    // Needs to be a lambda due to use of auto
    auto calculate_time = [](const auto start) {
        const auto end = std::chrono::high_resolution_clock::now();

        // Calculating total time taken by the program.
        double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        time_taken *= 1e-9;

        std::cout << "Elapsed : " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
    };

    class AutoTimer {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_;

    public:
        AutoTimer()
            : start_(std::chrono::high_resolution_clock::now())
        { }

        ~AutoTimer() {
            aoc::calculate_time(start_);
        }

        void elapsed() const {
            aoc::calculate_time(start_);
        }

        void reset() {
            start_ = std::chrono::high_resolution_clock::now();
        }
    };

};