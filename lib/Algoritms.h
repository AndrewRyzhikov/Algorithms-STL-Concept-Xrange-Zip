#include <concepts>
#include <iterator>
namespace algorithm {
    template<typename iterator>
    concept BidirectionalIterator = requires(iterator x) { std::bidirectional_iterator<iterator>; };

    template<typename iterator, typename Predicate>
    concept UnaryPredicate = requires(iterator x, Predicate comp) {{ comp(*x) } -> std::same_as<bool>; };

    template<typename iterator, typename Compare>
    requires requires(iterator x, iterator y, Compare comp) {{ comp(*x, *y) } -> std::same_as<bool>; }
    bool is_sorted(iterator first, iterator last, Compare comp) {
        while (first + 1 != last) {
            if (!comp(*first, *(first + 1))) {
                return false;
            }
            ++first;
        }
        return true;
    }

    template<BidirectionalIterator iterator>
    bool is_palindrome(iterator first, iterator last) {
        if ((last - first) % 2 == 1) {
            --last;
            while (first != last) {
                if (*first != *last) {
                    return false;
                }
                ++first, --last;
            }
        } else {
            --last;
            while (first < last) {
                if (*first != *last) {
                    return false;
                }
                ++first, --last;
            }
        }
        return true;
    }

    template<typename iterator, typename Predicate>
    requires UnaryPredicate<iterator, Predicate>
    bool all_of(iterator first, iterator last, Predicate comp) {
        while (first != last) {
            if (!comp(*first)) {
                return false;
            }
            ++first;
        }
        return true;
    }

    template<typename iterator, typename Predicate>
    requires UnaryPredicate<iterator, Predicate>
    bool any_of(iterator first, iterator last, Predicate comp) {
        while (first != last) {
            if (comp(*first)) {
                return true;
            }
            ++first;
        }
        return false;
    }

    template<typename iterator, typename Predicate>
    requires UnaryPredicate<iterator, Predicate>
    bool none_of(iterator first, iterator last, Predicate comp) {
        while (first != last) {
            if (comp(*first)) {
                return false;
            }
            ++first;
        }
        return true;
    }

    template<typename iterator, typename Predicate>
    requires UnaryPredicate<iterator, Predicate>
    bool one_of(iterator first, iterator last, Predicate comp) {
        bool flag_exist = false;
        while (first != last) {
            if (comp(*first)) {
                if (flag_exist) {
                    return false;
                }
                flag_exist = true;
            }
            ++first;
        }
        return flag_exist;
    }

    template<typename iterator, typename Predicate>
    requires UnaryPredicate<iterator, Predicate>
    bool is_partitioned(iterator first, iterator last, Predicate comp) {
        while (first != last and comp(*first)) {
            ++first;
        }
        while (first != last) {
            if (comp(*first)) {
                return false;
            }
            ++first;
        }
        return true;
    }

    template<typename iterator, typename T = iterator::value_type>
    iterator find_not(iterator first, iterator last, T target) {
        while (first != last) {
            if (*first != target) {
                return first;
            }
            ++first;
        }
        return first;
    }

    template<BidirectionalIterator iterator, typename T = iterator::value_type>
    iterator find_backward(iterator first, iterator last, T target) {
        iterator tmp = last;
        --last;
        while (first != last) {
            if (*last == target) {
                return last;
            }
            --last;
        }
        return tmp;
    }
}
