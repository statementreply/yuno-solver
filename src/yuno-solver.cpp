#include "matrix.h"
#include "solve.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace yunosolver {

std::ostream& operator<<(std::ostream& os, const Matrix<Cell>& m) {
    const auto rows = m.row_size();
    const auto cols = m.column_size();
    for (std::ptrdiff_t i = 0; i < rows; i++) {
        if (i != 0) {
            os << "\n";
        }
        for (std::ptrdiff_t j = 0; j < cols; j++) {
            if (j != 0) {
                os << " ";
            }
            switch (m(i, j)) {
            case Cell::Empty:
                os << ".";
                break;
            case Cell::Filled:
                os << "#";
                break;
            default:
                os << "?";
                break;
            }
        }
    }
    return os;
}

class InvalidProgramInput : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

ProblemInput readInput(std::istream& is) {
    // format:
    //   row_num col_num
    //   row_1
    //   ...
    //   row_M
    //   col_1
    //   ...
    //   col_N
    // row/column without filled cells: "0" (a single zero)
    // comment starts with "#"

    struct MatrixSize {
        std::ptrdiff_t rows = 0;
        std::ptrdiff_t cols = 0;
    };

    const auto readNonCommentLine = [&](std::string& line) {
        while (std::getline(is, line)) {
            std::istringstream iss{line};
            if (auto c = (iss >> std::ws).peek(); c != EOF && c != '#') {
                return true;
            }
        }
        return false;
    };

    const auto readMatrixSize = [&] {
        if (std::string line; readNonCommentLine(line)) {
            std::istringstream iss{line};
            MatrixSize ret;
            if (!(iss >> ret.rows >> ret.cols)) {
                throw InvalidProgramInput("invalid input");
            }
            if (!(iss.eof() || (iss >> std::ws).eof())) {
                throw InvalidProgramInput("invalid input");
            }
            return ret;
        } else {
            throw InvalidProgramInput("invalid input");
        }
    };

    const auto readOneLine = [&] {
        if (std::string line; readNonCommentLine(line)) {
            std::istringstream iss{line};
            std::vector<int> ret;
            std::copy(std::istream_iterator<int>(iss), std::istream_iterator<int>{}, std::back_inserter(ret));
            if (!iss.eof()) {
                throw InvalidProgramInput("invalid input");
            }
            // "0" also means empty row/column
            if (ret.size() == 1 && ret.front() == 0) {
                return std::vector<int>{};
            }
            return ret;
        } else {
            throw InvalidProgramInput("invalid input");
        }
    };

    const auto readDef = [&](std::ptrdiff_t size) {
        std::vector<std::vector<int>> ret;
        ret.reserve(size);
        std::generate_n(std::back_inserter(ret), size, readOneLine);
        return ret;
    };

    const auto size = readMatrixSize();
    ProblemInput input;
    input.rowdef = readDef(size.rows);
    input.coldef = readDef(size.cols);
    return input;
}

}

int main() {
    using namespace yunosolver;
    try {
        if (const auto ret = solve(readInput(std::cin)); ret) {
            std::cout << *ret << "\n";
        } else {
            std::cout << "No solution\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    return 0;
}
