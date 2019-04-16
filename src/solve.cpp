#include "solve.h"

namespace yunosolver {

namespace detail {
    bool check(Matrix<Cell>& data, const MatrixIndex& idx, const ProblemInput& input) {
        // check if current row or column matches input
        // exact = true : whole row/column match
        // exact = false: partial match up to current position
        const auto doCheck = [](auto&& get, const auto& ref, std::ptrdiff_t bound, bool exact) {
            auto curr = std::cbegin(ref);
            const auto last = std::cend(ref);
            int num = 0;
            bool prev = false;
            for (std::ptrdiff_t i = 0; i < bound; i++) {
                switch (get(i)) {
                case Cell::Empty: {
                    if (prev) {
                        // end of contiguous filled cells
                        if (num != *curr) {
                            // too few filled cells
                            return false;
                        }
                        ++curr;
                        num = 0;
                        prev = false;
                    }
                    break;
                }
                case Cell::Filled: {
                    if (!prev) {
                        // start of contiguous filled cells
                        if (curr == last) {
                            // too many cell groups
                            return false;
                        }
                        prev = true;
                    }
                    ++num;
                    if (num > *curr) {
                        // too many filled cells
                        return false;
                    }
                    break;
                }
                default:
                    throw std::logic_error("logic error in yunosolver::solve");
                }
            }
            if (exact) {
                if (prev) {
                    // last contiguous filled slots
                    if (num != *curr) {
                        // too few filled cells
                        return false;
                    }
                    ++curr;
                }
                if (curr != last) {
                    // too few cell groups
                    return false;
                }
            }
            return true;
        };

        return doCheck([&](std::ptrdiff_t column) { return data(idx.row(), column); }, input.rowdef[idx.row()],
                       idx.column() + 1, idx.column() + 1 == data.column_size())
                && doCheck([&](std::ptrdiff_t row) { return data(row, idx.column()); }, input.coldef[idx.column()],
                           idx.row() + 1, idx.row() + 1 == data.row_size());
    }

    bool solve(Matrix<Cell>& data, const MatrixIndex& idx, const ProblemInput& input) {
        if (idx.is_end()) {
            return true;
        }
        // brute force DFS
        for (Cell val : {Cell::Empty, Cell::Filled}) {
            data[idx] = val;
            if (!check(data, idx, input)) {
                continue;
            }
            MatrixIndex nextIdx = idx;
            ++nextIdx;
            if (solve(data, nextIdx, input)) {
                return true;
            }
        }
        return false;
    }
}

std::optional<Matrix<Cell>> solve(const ProblemInput& input) {
    Matrix<Cell> data(input.rowdef.size(), input.coldef.size());
    MatrixIndex idx(data);
    if (detail::solve(data, idx, input)) {
        return data;
    } else {
        return std::nullopt;
    }
}

}
