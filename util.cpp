//
// Created by cleuton on 6/6/24.
//
#include "util.h"

bool isNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}