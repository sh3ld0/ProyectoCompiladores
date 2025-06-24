// lexer.cpp
#include "tokens.hpp"
#include "lexer.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

std::vector<Token> tokenize(const std::string& filename) {
    std::vector<Token> tokens;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << '\n';
        return tokens;
    }

    std::string line;
    int line_number = 1;

    std::regex note_regex(R"(([A-Ga-g](#)?)(\d+)-(\d+/\d+))"); // Ej: C#4-1/4
    std::regex rest_regex(R"(rest-(\d+/\d+))");                 // Ej: rest-1/4
    std::regex loop_regex(R"(loop)");
    std::regex number_regex(R"(\d+)");
    std::regex lbrace_regex(R"(\{)");
    std::regex rbrace_regex(R"(\})");

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            std::smatch match;

            if (std::regex_match(word, match, note_regex)) {
                tokens.push_back({TokenType::NOTE_NAME, match[1], line_number});
                tokens.push_back({TokenType::OCTAVE, match[3], line_number});
                tokens.push_back({TokenType::DASH, "-", line_number});
                tokens.push_back({TokenType::FRACTION, match[4], line_number});
            }
            else if (std::regex_match(word, match, rest_regex)) {
                tokens.push_back({TokenType::REST, "rest", line_number});
                tokens.push_back({TokenType::DASH, "-", line_number});
                tokens.push_back({TokenType::FRACTION, match[1], line_number});
            }
            else if (std::regex_match(word, match, loop_regex)) {
                tokens.push_back({TokenType::LOOP, word, line_number});
            }
            else if (std::regex_match(word, match, number_regex)) {
                tokens.push_back({TokenType::NUMBER, word, line_number});
            }
            else if (std::regex_match(word, match, lbrace_regex)) {
                tokens.push_back({TokenType::LBRACE, "{", line_number});
            }
            else if (std::regex_match(word, match, rbrace_regex)) {
                tokens.push_back({TokenType::RBRACE, "}", line_number});
            }
            else {
                tokens.push_back({TokenType::INVALID, word, line_number});
            }
        }
        line_number++;
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line_number});
    return tokens;
}
