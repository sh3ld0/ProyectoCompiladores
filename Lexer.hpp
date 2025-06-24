// lexer.hpp
#pragma once
#include "tokens.hpp"
#include <string>
#include <vector>

std::vector<Token> tokenize(const std::string& filename);