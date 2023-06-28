#include <cstdint>
#include <iterator>
#include <iostream>

template<typename T>
class XrangeIterator {
public:
    using value_type = T;
    using iterator_category = std::input_iterator_tag;
    using difference_type = int64_t;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    XrangeIterator() = default;

    template<typename X = T>
    requires(std::is_arithmetic_v<T>)
    explicit XrangeIterator(const int64_t step) : cnt_(0), step_(step) {};

    template<typename X = T>
    requires(!std::is_arithmetic_v<T>)
    explicit XrangeIterator(const int64_t step) : cnt_(value_type()), step_(step) {};

    XrangeIterator(const_reference cnt, const int64_t step) : cnt_(cnt), step_(step) {};

    template<typename X = T>
    requires requires(T x) { x + 1; }
    XrangeIterator& operator++() {
        cnt_ = cnt_ + step_;
        return *this;
    }

    template<typename X = T> requires (requires(T x) { x++; }, !requires (
    T x
    ) { x + 1; })

    XrangeIterator& operator++() {
        for (int64_t i = 0; i < step_; ++i) {
            cnt_++;
        }
        return *this;
    }

    size_t operator-(XrangeIterator other) {
        size_t a = abs(other.cnt_ - cnt_);
        return a;
    }

    value_type operator*() const {
        return cnt_;
    }

    template<typename X = T>
    requires requires(T x, T y) { x < y; }
    bool operator!=(XrangeIterator& other) const {
        if (step_ > 0) {
            return (cnt_ < other.cnt_);
        }
        return cnt_ > other.cnt_;
    }

private:
    value_type cnt_;
    difference_type step_;
};

template<typename T>
class Xrange {
public:
    using value_type = T;
    using iterator_category = std::input_iterator_tag;
    using pointer = value_type*;
    using difference_type = int64_t;
    using reference = value_type&;
    using iterator = XrangeIterator<value_type>;
    using const_reference = const value_type&;

    Xrange(const_reference end) : begin_(1), end_(end, 1) {};

    Xrange(const_reference begin, const_reference end) : begin_(begin, 1), end_(end, 1) {};

    Xrange(const_reference begin, const_reference end, difference_type step) : begin_(begin, step), end_(end, step) {};

    iterator begin() const {
        return begin_;
    }

    iterator end() const {
        return end_;
    }

private:
    iterator begin_;
    iterator end_;
};
