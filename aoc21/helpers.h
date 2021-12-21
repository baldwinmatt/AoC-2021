#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <chrono>
#include <cassert>
#include <functional>
#include <iomanip>

#ifndef NDEBUG
#define DEBUG(x) do { \
    x; \
} while (0)
#else
#define DEBUG(x)
#endif

#define DEBUG_PRINT(x) do { DEBUG(std::cout << x << std::endl); } while (0)

#define STRING_CONSTANT(symbol, value) constexpr std::string_view symbol(value)

#ifndef INT_MIN
#define INT_MIN std::numeric_limits<int>::min()
#endif

#ifndef INT_MAX
#define INT_MAX std::numeric_limits<int>::max()
#endif

namespace aoc {

    using Point = std::pair<int, int>;
    // Needed if we want to store a point in a hash
    struct PointHash {
        std::size_t operator() (const Point& pair) const {
            size_t v = pair.first;
            v <<= 32;
            v |= pair.second;
            return v;
        }
    };

    const auto print_result = [](int part, auto result) {
        std::cout << "Part " << part << ": " << result << std::endl;
    };

    const auto print_results = [](const auto& part1, const auto& part2) {
        print_result(1, part1);
        print_result(2, part2);
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

    std::ostream& cls(std::ostream& os) {
        return os << "\033[2J\033[1;1H";
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

    class AutoTimer {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_;
        std::string name_;

    public:
        AutoTimer()
            : start_(std::chrono::high_resolution_clock::now())
        { }

        AutoTimer(const char *name)
            : start_(std::chrono::high_resolution_clock::now())
            , name_(name)
        { }

        ~AutoTimer() {
            calculate_time();
        }

        void elapsed() const {
            calculate_time();
        }

        void reset() {
            start_ = std::chrono::high_resolution_clock::now();
        }

    private:
        // Needs to be a lambda due to use of auto
        void calculate_time () const {
            const auto end = std::chrono::high_resolution_clock::now();

            // Calculating total time taken by the program.
            double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
            time_taken *= 1e-9;

            std::cout << "Elapsed" << (name_.empty() ? "" : " " + name_) << ": " << std::fixed << time_taken << std::setprecision(9) << " sec" << std::endl;
        }

    };

};
