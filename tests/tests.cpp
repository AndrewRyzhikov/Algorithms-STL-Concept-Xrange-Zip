#include <gtest/gtest.h>
#include "lib/Xrange.h"
#include "lib/Zip.h"
#include "lib/Algoritms.h"
#include "lib/buffer.h"
#include <iostream>
#include <vector>
#include <list>

struct Structure {
    int field = 10;

    Structure() {
        field = 0;
    }

    Structure(int field0) {
        field = field0;
    }

    Structure& operator++(int) {
        field += 1;
        return *this;
    }

    bool operator<(const Structure& other) const {
        return field < other.field;
    }

    bool operator>(const Structure& other) const {
        return field > other.field;
    }

    Structure operator+(int64_t other) {
        return Structure(field + other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Structure& data);
};

std::ostream& operator<<(std::ostream& os, const Structure& data) {
    os << data.field;
    return os;
}

TEST(TestXrange, Test2) {

    int i = 1;
    for (auto elem: Xrange(3, 13, 3)) {
        ASSERT_EQ(3 * i, elem);
        ++i;
    }
}

TEST(TestXrange, Test3) {
    Structure a(20);
    Structure b(30);
    auto x = Xrange(a, b, 2);

    int i = 0;
    for (auto elem: x) {
        ASSERT_EQ(20 + 2 * i, elem.field);
        ++i;
    }
}

TEST(TestXrange, Test4) {
    auto x = Xrange(5, 2);

    int i = 0;
    for (auto elem: x) {
        ASSERT_EQ(5 - i, elem);
        ++i;
    }
}

TEST(TestXrange, Test5) {
    Structure a(20);
    Structure b(30);
    auto x = Xrange(b, a, -2);

    int i = 0;
    for (auto elem: x) {
        ASSERT_EQ(30 - 2 * i, elem.field);
        ++i;
    }
}

TEST(TestXrange, Test6) {
    auto x = Xrange(4);
    BufferStatic<int> v{x.begin(), x.end()}; // 0 1 2 3
    int i = 0;
    for (auto elem: v) {
        ASSERT_EQ(i, elem);
        ++i;
    }
}

// -----------------------------------
TEST(TestZip, Test6) {
    std::list l = {1, 2, 3, 4, 5};
    std::vector v = {'a', 'b', 'c', 'd'};

    int i = 0;
    for (auto value: Zip(l, v)) {
        ASSERT_EQ(i + 1, value.first);
        ASSERT_EQ('a' + i, value.second);
        ++i;
    }
}

TEST(TestZip, Test7) {
    std::list l = {1, 2, 3, 4, 5};
    std::vector v = {'a', 'b', 'c', 'd'};

    std::vector <std::pair<int, char>> pairs(Zip(l, v).begin(), Zip(l, v).end());
    int i = 0;
    for (auto value: pairs) {
        ASSERT_EQ(i + 1, value.first);
        ASSERT_EQ('a' + i, value.second);
        ++i;
    }
}


TEST(TestMixed, Test8) {
    Structure a(20);
    Structure b(30);

    auto x1 = Xrange(a, b, 1);
    std::vector v1(x1.begin(), x1.end());

    auto x2 = Xrange(b, a, -2);
    std::vector v2 (x2.begin(), x2.end());

    auto z = Zip(v1, v2);
    std::vector <std::pair<Structure, Structure>> pairs(z.begin(), z.end());
    int i = 0;
    for (auto value: pairs) {
        ASSERT_EQ(value.first.field, i + 20);
        ASSERT_EQ(value.second.field, 30 - 2 * i);
        ++i;
    }
}


// ----------------------------------


TEST(Testalgorithm, TestAllOf) {
    auto is_even = [](int i) { return i % 2 == 0; };

    std::vector<int> v1{2, 4, 6, 8};
    std::vector<int> v2{1, 2, 4, 6, 8};
    ASSERT_TRUE(algorithm::all_of(v1.begin(), v1.end(), is_even));
    ASSERT_FALSE(algorithm::all_of(v2.begin(), v2.end(), is_even));
}

TEST(Testalgorithm, TestAnyOf) {
    auto is_even = [](int i) { return i % 2 == 0; };

    std::vector<int> v1{1, 3, 5, 8};
    std::vector<int> v2{1, 7, 3, 5, 9};

    ASSERT_TRUE(algorithm::any_of(v1.begin(), v1.end(), is_even));
    ASSERT_FALSE(algorithm::any_of(v2.begin(), v2.end(), is_even));
}

TEST(Testalgorithm, TestNoneOf) {
    auto is_even = [](int i) { return i % 2 == 0; };

    std::vector<int> v1{1, 3, 5, 7};
    std::vector<int> v2{2, 4, 6, 8};

    ASSERT_TRUE(algorithm::none_of(v1.begin(), v1.end(), is_even));
    ASSERT_FALSE(algorithm::none_of(v2.begin(), v2.end(), is_even));
}

TEST(Testalgorithm, TestOneOf) {
    auto is_even = [](int i) { return i % 2 == 0; };

    std::vector<int> v1{1, 2, 3, 5, 7};
    std::vector<int> v2{1, 3, 5, 7};

    ASSERT_TRUE(algorithm::one_of(v1.begin(), v1.end(), is_even));
    ASSERT_FALSE(algorithm::one_of(v2.begin(), v2.end(), is_even));
}


TEST(Testalgorithm, TestSorted) {
    auto compare = [](int i, int j) { return i % 2 <= j % 2; };

    std::vector<int> v_unsorted{2, 4, 6, 8, 1, 10, 3, 7, 7};
    std::vector<int> v_sorted{2, 4, 6, 8, 10, 1, 3, 7, 7};

    ASSERT_FALSE(algorithm::is_sorted(v_unsorted.begin(), v_unsorted.end(), compare));
    ASSERT_TRUE(algorithm::is_sorted(v_sorted.begin(), v_sorted.end(), compare));
}

TEST(Testalgorithm, TestPartitioned) {
    auto is_negative = [](int i) { return i < 0; };

    std::vector<int> v1{-1, -2, 3, 5, 7};
    std::vector<int> v2{1, 3, 5, 7};
    std::vector<int> v3{-1, -3, -5, -7};
    std::vector<int> v4{1, -3, -5, -7};

    ASSERT_TRUE(algorithm::is_partitioned(v1.begin(), v1.end(), is_negative));
    ASSERT_TRUE(algorithm::is_partitioned(v2.begin(), v2.end(), is_negative));
    ASSERT_TRUE(algorithm::is_partitioned(v3.begin(), v3.end(), is_negative));

    ASSERT_FALSE(algorithm::is_partitioned(v4.begin(), v4.end(), is_negative));

}


TEST(Testalgorithm, TestFindNot) {
    std::vector <int> v1{3, 3, 3, 5, 7};
    std::vector <int> v2{1, 1, 1, 1, 1};
    ASSERT_EQ(algorithm::find_not(v1.begin(), v1.end(), 3), v1.begin() + 3);
    ASSERT_EQ(algorithm::find_not(v2.begin(), v2.end(), 1), v2.end());
}

TEST(Testalgorithm, TestFindBackward) {
    std::vector<int> v1{3, 3, 5, 3, 7};
    std::vector<int> v2{1, 1, 1, 1, 1};
    ASSERT_EQ(algorithm::find_backward(v1.begin(), v1.end(), 3), v1.begin() + 3);
    ASSERT_EQ(algorithm::find_backward(v2.begin(), v2.end(), 3), v2.end());
}
