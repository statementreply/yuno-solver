#pragma once
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace yunosolver {

class MatrixIndex;

template <class T>
class Matrix {
public:
    using container_type = std::vector<T>;
    using value_type = typename container_type::value_type;
    using size_type = typename container_type::size_type;
    using difference_type = std::common_type_t<std::ptrdiff_t, typename container_type::difference_type>;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

    Matrix() = default;
    ~Matrix() = default;
    Matrix(const Matrix&) = default;
    Matrix& operator=(const Matrix&) = default;

    Matrix(Matrix&& other) noexcept
            : c{std::move(other).c}, rownum_{std::move(other).rownum_}, colnum_{std::move(other).colnum_} {
        other.clear();
    }

    Matrix& operator=(Matrix&& other) noexcept {
        c = std::move(other).c;
        rownum_ = std::move(other).rownum_;
        colnum_ = std::move(other).colnum_;
        other.clear();
    }

    // Construct with matrix size
    explicit Matrix(difference_type row_size, difference_type column_size)
            : c(row_size * column_size), rownum_{row_size}, colnum_{column_size} {}

    // List initialize
    Matrix(std::initializer_list<std::initializer_list<T>> data)
            : rownum_{static_cast<difference_type>(data.size())}
            , colnum_{static_cast<difference_type>(data.begin()->size())} {
        c.reserve(rownum_ * colnum_);
        for (const auto& row : data) {
            for (const T& x : row) {
                c.push_back(x);
            }
        }
    }

    void swap(Matrix& other) noexcept {
        using std::swap;
        swap(c, other.c);
        swap(rownum_, other.rownum_);
        swap(colnum_, other.colnum_);
    }
    friend void swap(Matrix& lhs, Matrix& rhs) { lhs.swap(rhs); }

    iterator begin() noexcept { return c.begin(); }
    const_iterator begin() const noexcept { return c.begin(); }
    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() noexcept { return c.end(); }
    const_iterator end() const noexcept { return c.end(); }
    const_iterator cend() const noexcept { return end(); }

    reverse_iterator rbegin() noexcept { return c.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return c.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    reverse_iterator rend() noexcept { return c.rend(); }
    const_reverse_iterator rend() const noexcept { return c.rend(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    pointer data() noexcept { return c.data(); }
    const_pointer data() const noexcept { return c.data(); }

    reference operator()(difference_type row, difference_type column) { return c[row * colnum_ + column]; }
    const_reference operator()(difference_type row, difference_type column) const { return c[row * colnum_ + column]; }
    reference operator[](const MatrixIndex& idx);
    const_reference operator[](const MatrixIndex& idx) const;

    reference at(difference_type row, difference_type column) {
        bound_check(row, column);
        return (*this)(row, column);
    }
    const_reference at(difference_type row, difference_type column) const {
        bound_check(row, column);
        return (*this)(row, column);
    }
    reference at(const MatrixIndex& idx) {
        index_check(idx);
        return (*this)[idx];
    }
    const_reference at(const MatrixIndex& idx) const {
        index_check(idx);
        return (*this)[idx];
    }

    [[nodiscard]] bool empty() const noexcept { return c.empty(); }

    difference_type row_size() const noexcept { return rownum_; }
    difference_type column_size() const noexcept { return colnum_; }
    size_type size() const noexcept { return c.size(); }

    void clear() noexcept {
        c.clear();
        rownum_ = 0;
        colnum_ = 0;
    }

    void resize(difference_type row_size, difference_type column_size) {
        Matrix ret(row_size, column_size);
        const difference_type rows = std::min(rownum_, row_size);
        const difference_type cols = std::min(colnum_, column_size);
        for (std::ptrdiff_t row = 0; row != rows; row++) {
            for (std::ptrdiff_t col = 0; col != cols; col++) {
                ret(row, col) = std::move_if_noexcept((*this)(row, col));
            }
        }
        *this = std::move(ret);
    }

private:
    void bound_check(difference_type row, difference_type column) const {
        if (row < 0 || row >= rownum_) {
            throw std::range_error{"matrix row out of range"};
        }
        if (column < 0 || column >= colnum_) {
            throw std::range_error{"matrix column out of range"};
        }
    }
    void index_check(const MatrixIndex& idx) const;

    container_type c;
    difference_type rownum_ = 0;
    difference_type colnum_ = 0;
};


class MatrixIndex {
public:
    MatrixIndex() = default;

    constexpr MatrixIndex(std::ptrdiff_t row_size, std::ptrdiff_t column_size)
            : rownum_{row_size}, colnum_{column_size} {}

    template <class T>
    constexpr explicit MatrixIndex(const Matrix<T>& m) : MatrixIndex(m.row_size(), m.column_size()) {}

    constexpr std::ptrdiff_t row_size() const noexcept { return rownum_; }
    constexpr std::ptrdiff_t column_size() const noexcept { return colnum_; }
    constexpr std::ptrdiff_t row() const noexcept { return row_; }
    constexpr std::ptrdiff_t column() const noexcept { return col_; }

    constexpr void row(std::ptrdiff_t value) { row_ = value; }
    constexpr void column(std::ptrdiff_t value) { col_ = value; }

    constexpr bool is_begin() const noexcept { return row() == 0 && column() == 0; }
    constexpr bool is_end() const noexcept { return row() == row_size(); }

    constexpr MatrixIndex& operator++() {
        ++col_;
        if (col_ == colnum_) {
            col_ = 0;
            ++row_;
        }
        return *this;
    }

    constexpr MatrixIndex& operator--() {
        if (col_ == 0) {
            col_ = colnum_;
            --row_;
        }
        --col_;
        return *this;
    }

    constexpr MatrixIndex operator++(int) {
        auto ret = *this;
        ++*this;
        return ret;
    }

    constexpr MatrixIndex operator--(int) {
        auto ret = *this;
        --*this;
        return ret;
    }

private:
    std::ptrdiff_t rownum_ = 0;
    std::ptrdiff_t colnum_ = 0;
    std::ptrdiff_t row_ = 0;
    std::ptrdiff_t col_ = 0;
};

template <class T>
inline typename Matrix<T>::reference Matrix<T>::operator[](const MatrixIndex& idx) {
    return (*this)(idx.row(), idx.column());
}

template <class T>
inline typename Matrix<T>::const_reference Matrix<T>::operator[](const MatrixIndex& idx) const {
    return (*this)(idx.row(), idx.column());
}

template <class T>
inline void Matrix<T>::index_check(const MatrixIndex& idx) const {
    if (idx.row_size() != row_size()) {
        throw std::range_error{"invalid matrix index"};
    }
    if (idx.column_size() != column_size()) {
        throw std::range_error{"invalid matrix index"};
    }
    bound_check(idx.row(), idx.column());
}

}
