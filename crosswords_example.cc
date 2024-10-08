/*
 * File:        crosswords_example.cc
 * Created on:  31/10/2023
 * Author:      Przemysław Rutka
 */

#include <cassert>
#include <iostream>
#include <utility>
#include "crosswords.h"

#define WORD_BASIC_ASSERTS(w, sp, ep, o, ci, c, l) \
do { \
    assert(w.get_start_position() == sp); \
    assert(w.get_end_position() == ep); \
    assert(w.get_orientation() == o); \
    assert(w.at(ci) == c); \
    assert(w.length() == l); \
} while (0)

#define RECT_AREA_BASE_ASSERTS(ra, lt, rb, s, e) \
do { \
    assert(ra.get_left_top() == lt); \
    assert(ra.get_right_bottom() == rb); \
    assert(ra.size() == s); \
    assert(ra.empty() == e); \
} while(0)

#define CROSSWORD_DIM_ASSERTS(cr, sz, cnt) \
do { \
    assert(cr.size() == sz); \
    assert(cr.word_count() == cnt); \
} while(0)

namespace {
    using orientation_t::H;
    using orientation_t::V;
    using std::cout;

    void word_tests() {
        Word w1(1, 1, H, "COMPUTER");
        Word w2(3, 1, V, "MEMORY");
        Word w3(1, 5, V, "");

        WORD_BASIC_ASSERTS(w1, pos_t(1, 1), pos_t(8, 1), H, 0, 'C', 8);
        WORD_BASIC_ASSERTS(w2, pos_t(3, 1), pos_t(3, 6), V, 6, DEFAULT_CHAR, 6);
        WORD_BASIC_ASSERTS(w3, pos_t(1, 5), pos_t(1, 5), V, 0,
                           DEFAULT_CHAR, DEFAULT_WORD.length());

        assert(w1 < w3);
        assert(w1 <= w3);
        assert(w2 > w1);
        assert(w2 >= w1);
        assert(w1 != w2);
        assert(w2 > Word(3, 1, H, "MEMORY"));
        assert(w2 == Word(3, 1, V, "MNEMONIC"));
    }

    void rect_area_tests() {
        RectArea ra1(pos_t(1, 2), pos_t(10, 7));
        RECT_AREA_BASE_ASSERTS(ra1, pos_t(1, 2), pos_t(10, 7), dim_t(10, 6), false);
        ra1.set_left_top(pos_t(2, 3));
        RECT_AREA_BASE_ASSERTS(ra1, pos_t(2, 3), pos_t(10, 7), dim_t(9, 5), false);
        ra1.set_right_bottom(pos_t(9, 7));
        RECT_AREA_BASE_ASSERTS(ra1, pos_t(2, 3), pos_t(9, 7), dim_t(8, 5), false);

        RectArea ra2(pos_t(10, 7), pos_t(1, 2));
        RECT_AREA_BASE_ASSERTS(ra2, pos_t(10, 7), pos_t(1, 2), dim_t(0, 0), true);

        RectArea ra3(pos_t(1, 2), pos_t(10, 7));
        ra3 *= RectArea(pos_t(4, 5), pos_t(15, 10));
        RECT_AREA_BASE_ASSERTS(ra3, pos_t(4, 5), pos_t(10, 7), dim_t(7, 3), false);

        RectArea ra4 = ra3 * RectArea(pos_t(15, 10), pos_t(15, 11));
        assert(ra4.size() == dim_t(0, 0));
        assert(ra4.empty());

        RectArea ra5(pos_t(2, 3), pos_t(10, 7));
        ra5.embrace(pos_t(4, 5));
        RECT_AREA_BASE_ASSERTS(ra5, pos_t(2, 3), pos_t(10, 7), dim_t(9, 5), false);
        ra5.embrace(pos_t(1, 2));
        RECT_AREA_BASE_ASSERTS(ra5, pos_t(1, 2), pos_t(10, 7), dim_t(10, 6), false);

        assert(DEFAULT_EMPTY_RECT_AREA.size() == dim_t(0, 0));
        assert(DEFAULT_EMPTY_RECT_AREA.empty());

        Word w1(1, 1, V, "COMPUTER");
        RECT_AREA_BASE_ASSERTS(w1.rect_area(), pos_t(1, 1), pos_t(1, 8),
                               dim_t(1, 8), false);
    }

    void crossword_tests() {
        Word w1(1, 1, H, "computer");
        Word w2(3, 1, V, "memory");
        Crossword cr1(w1, {w2});
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(8, 6), dim_t(1, 1));
        assert(cr1.insert_word(Word(11, 3, V, "LINUX")));
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(11, 7), dim_t(1, 2));
        assert(!cr1.insert_word(Word(2, 4, H, "programme")));
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(11, 7), dim_t(1, 2));
        assert(!cr1.insert_word(Word(2, 5, H, "programme")));
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(11, 7), dim_t(1, 2));
        assert(cr1.insert_word(Word(2, 5, H, "programming")));
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(12, 7), dim_t(2, 2));
        cout << cr1 << '\n';

        Crossword cr2(Word(10, 7, H, "EXECUTION"), {});
        CROSSWORD_DIM_ASSERTS(cr2, dim_t(9, 1), dim_t(1, 0));
        assert(cr2.insert_word(Word(10, 3, V, "PARSER")));
        CROSSWORD_DIM_ASSERTS(cr2, dim_t(9, 6), dim_t(1, 1));
        assert(cr2.insert_word(Word(13, 7, V, "Code")));
        CROSSWORD_DIM_ASSERTS(cr2, dim_t(9, 8), dim_t(1, 2));
        CROSSWORD_BACKGROUND = '~';
        cout << cr2 << '\n';

        Crossword cr3 = cr1 + cr2;
        CROSSWORD_DIM_ASSERTS(cr3, dim_t(18, 10), dim_t(3, 3));
        CROSSWORD_BACKGROUND = '=';
        cout << cr3 << '\n';
        cr1 += cr2;
        CROSSWORD_DIM_ASSERTS(cr1, dim_t(18, 10), dim_t(3, 3));
        CROSSWORD_BACKGROUND = '.';
        cout << cr1 << '\n';

        Crossword cr4(Word(2, 5, V, "C++20"), {Word(0, 8, H, "C17")});
        cout << cr4;
    }
}   /* anonymous namespace */

int main() {
    word_tests();
    rect_area_tests();
    crossword_tests();
}
