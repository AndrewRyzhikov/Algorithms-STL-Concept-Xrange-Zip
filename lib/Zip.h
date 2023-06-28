#include <iostream>
#include <cstdint>
#include <type_traits>

template<typename iterator_lhs, typename iterator_rhs>
class ZipIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::pair<typename iterator_lhs::value_type, typename iterator_rhs::value_type>;
    using difference_type = int64_t;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    ZipIterator(iterator_lhs it_lhs, iterator_rhs it_rhs) : it_lhs_(it_lhs), it_rhs_(it_rhs) {};

    ZipIterator& operator++() {
        it_lhs_++, it_rhs_++;
        return *this;
    }

    bool operator!=(const ZipIterator& other) {
        return (it_lhs_ != other.it_lhs_ and it_rhs_ != other.it_rhs_);
    }

    value_type operator*() {
        return {*it_lhs_, *it_rhs_};
    }

private:
    iterator_lhs it_lhs_;
    iterator_rhs it_rhs_;
};

template<typename container>
concept Container = requires(container con) {
    std::input_iterator<typename container::iterator>;
};

template<Container lhs_container, Container rhs_container>
class Zip {
public:
    using iterator_lhs = typename lhs_container::iterator;
    using iterator_rhs = typename rhs_container::iterator;
    using iterator = ZipIterator<iterator_lhs, iterator_rhs>;

    Zip(lhs_container& lhs, rhs_container& rhs) : begin_(lhs.begin(), rhs.begin()), end_(lhs.end(), rhs.end()) {}

    iterator begin() {
        return begin_;
    }

    iterator end() {
        return end_;
    }

private:
    iterator begin_;
    iterator end_;
};