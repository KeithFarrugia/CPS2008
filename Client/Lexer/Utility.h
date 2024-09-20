#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map> 
#include <iostream>
#include <iomanip>

#include "Types/Include_Types.h"


inline std::string toUpperCase(std::string& input) {    
    // Convert each character in the string to uppercase
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    return input;
}

inline char nextChar(const std::string& src_program_str, int& src_program_idx) {
    if (src_program_idx < src_program_str.length()) {
        return src_program_str[src_program_idx++];
    } else {
        return EOF; // Return EOF
    }
}

inline char peekChar(const std::string& src_program_str, int src_program_idx) {
    if (src_program_idx < src_program_str.length()) {
        return src_program_str[src_program_idx];
    } else {
        return EOF; // Return EOF
    }
}

inline std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

inline void throwErr(const std::string errorMessage){
    throw std::runtime_error(errorMessage);
}
inline void printTokenisedList(std::vector<tokenised_t> toks){
    std::cout << "\n\n============================ TOKEN LIST ============================\n\n";
    int i = 0;
    for (const auto& t : toks) {
        std::cout  << std::setw(6) << std::left << i++;
        std::cout << std::setw(18) << std::left << tk_string(t.token_type);
        std::cout << " [";
        for (char c : t.lexeme_string) {
            if (c == '\n') {
                std::cout << "\\n"; // print as '\n'
            } else {
                std::cout << c;
            }
        }
        std::cout << "]" << std::endl;
    }
}

inline std::string removeFirstAndLast(const std::string& str) {
    // Check if the string has at least two characters
    if (str.length() >= 2) {
        // Return the substring excluding the first and last characters
        return str.substr(1, str.length() - 2);
    } else {
        // Return the original string if it has less than two characters
        return str;
    }
}

#endif //UTILITY_H
