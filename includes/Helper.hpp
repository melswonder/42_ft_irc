#pragma once

#include "IRC.hpp"

std::vector<std::string> split(const std::string& str, char delimiter);

std::ostream &operator<<(std::ostream &out, const std::vector<std::string> &data);
