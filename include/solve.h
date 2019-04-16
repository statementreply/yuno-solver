#pragma once
#include "matrix.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace yunosolver {

enum class Cell {
    None,
    Empty,
    Filled,
};

struct ProblemInput {
    std::vector<std::vector<int>> rowdef;
    std::vector<std::vector<int>> coldef;
};

// try to solve YU-NO puzzle
std::optional<Matrix<Cell>> solve(const ProblemInput& input);

}
