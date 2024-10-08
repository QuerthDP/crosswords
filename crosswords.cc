/*
 * =====================================================
 * Trzeci projekt zaliczeniowy z
 * przedmiotu Kurs Programowania w C++ (1000-213bCPP).
 *
 * Plik zawiera definicje metod klas zadeklarowanych
 * w pliku nagłówkowym 'crosswords.h'.
 *
 * Autorzy:
 * Karol Baciński <kb448449@students.mimuw.edu.pl>
 * Dawid Pawlik   <dp450244@students.mimuw.edu.pl>
 * =====================================================
 */


/* Plik nagłówkowy implementowanego modułu */

#include "crosswords.h"


/* Pliki nagłówkowe wykorzystane podczas implementacji */

#include <ostream>
#include <unordered_map>
#include <cassert>


/* Deklaracje użycia wybranych elementów z przestrzeni std */

using std::max;
using std::min;
using std::unordered_map;


/* Aliasy używanych typów */

using placement_t = unordered_map<size_t, unordered_map<size_t, char>>;
using list_t = vector<Word>;


/* Definicja wyrażeń domyślnych używanych w implementacji modułu */

char CROSSWORD_BACKGROUND = '.';


/* Funkcje pomocnicze implementacji modułu */

namespace {

    /*
     * Funkcja przekierowuje wiersz obramowania krzyżówki na strumień 'os'.
     */
    void print_empty_row(ostream& os, size_t columns) {
        for (size_t j = 0; j < columns + 1; j++) {
            os << CROSSWORD_BACKGROUND << ' ';
        }
        os << CROSSWORD_BACKGROUND << '\n';
    }

    /*
     * Funkcja przekazuje rozłożenie liter na przestrzeni RectArea
     * zawartych w danej liście słów.
     */
    const placement_t get_layout(const list_t& word_vector) {
        placement_t letter_layout;
        for (Word word : word_vector) {
            pos_t start = word.get_start_position();
            size_t column = start.first;
            size_t row = start.second;
            orientation_t orientation = word.get_orientation();

            for (size_t i = 0; i < word.length(); i++) {
                if (orientation == H) {
                    letter_layout[row][column + i] = word.at(i);
                } else {
                    letter_layout[row + i][column] = word.at(i);
                }
            }
        }
        return letter_layout;
    }

    /*
     * Funkcja sprawdza czy występuje kolizja słów badając
     * przecięcia ich obszarów RectArea.
     */
    bool check_intersection(const Word& word, const Word& that_word) {
        orientation_t that_word_orientation = that_word.get_orientation();
        size_t that_word_start_column = that_word.get_start_position().first;
        size_t that_word_start_row = that_word.get_start_position().second;
        size_t that_word_end_column = that_word.get_end_position().first;
        size_t that_word_end_row = that_word.get_end_position().second;
        RectArea that_area = that_word.rect_area();
        RectArea that_area_surround = that_area;

       that_area_surround
            .embrace({that_word_start_column - (that_word_start_column > 0),
                     that_word_start_row - (that_word_start_row > 0)});
        that_area_surround
            .embrace({that_word_end_column + (that_word_end_column < SIZE_MAX),
                     that_word_end_row + (that_word_end_row < SIZE_MAX)});

        orientation_t word_orientation = word.get_orientation();
        size_t word_start_column = word.get_start_position().first;
        size_t word_start_row = word.get_start_position().second;
        RectArea word_area = word.rect_area();

        // Sprawdzamy czy słowo 'word' znajduje się w otoczeniu 'that_word'.
        if (!(word_area * that_area_surround).empty()) {
            if (that_word_orientation == word_orientation) {
                return true;
            }

            // Sprawdzamy czy słowo 'word' przecina się ze słowem 'that_word'.
            if (!(word_area * that_area).empty()) {
                size_t word_index, that_index;
                if (that_word_orientation == H) {
                    word_index = that_word_start_row - word_start_row;
                    that_index = word_start_column - that_word_start_column;
                } else {
                    word_index = that_word_start_column - word_start_column;
                    that_index = word_start_row - that_word_start_row;
                }

                // Sprawdzamy czy w miejscu przecięcia występuje ta sama litera.
                if (word.at(word_index) != that_word.at(that_index)) {
                    return true;
                }
            } else {
                return true;
            }
        }
        return false;
    }

} // Koniec anonimowej przestrzeni nazw.


/* Implementacja klas z modułu 'crosswords.h' */

/* Klasa RectArea */

/* Definicja publicznych metod klasy RectArea */

/*
 * Funkcja przekazuje pozycję lewego górnego rogu prostokąta.
 */
pos_t RectArea::get_left_top() const {
    return left_top;
}

/*
 * Funkcja ustawia pozycję lewego górnego rogu prostokąta
 * na pozycję przekazaną jako argument funkcji.
 */
void RectArea::set_left_top(const pos_t& lt) {
    left_top = lt;
}

/*
 * Funkcja przekazuje pozycję prawego dolnego rogu prostokąta.
 */
pos_t RectArea::get_right_bottom() const {
    return right_bottom;
}

/*
 * Funkcja ustawia pozycję prawego dolnego rogu prostokąta
 * na pozycję przekazaną jako argument funkcji.
 */
void RectArea::set_right_bottom(const pos_t& rb) {
    right_bottom = rb;
}

/*
 * Funkcja przekazuje rozmiar prostokąta.
 */
dim_t RectArea::size() const {
    if (empty())
        return dim_t(0, 0);
    assert(left_top.first > 0 || right_bottom.first < SIZE_MAX);
    assert(left_top.second > 0 || right_bottom.second < SIZE_MAX);
    return dim_t(right_bottom.first - left_top.first + 1,
                 right_bottom.second - left_top.second + 1);
}

/*
 * Funkcja przekazuje czy prostokąt jest pusty.
 */
bool RectArea::empty() const {
    return left_top.first > right_bottom.first ||
           left_top.second > right_bottom.second;
}

/*
 * Funkcja rozszerza obszar prostokąta, aby obejmował punkt 'new_box'.
 */
void RectArea::embrace(const pos_t& new_box) {
    if (empty()) {
        left_top = new_box;
        right_bottom = new_box;
    } else {
        left_top.first = min(left_top.first, new_box.first);
        left_top.second = min(left_top.second, new_box.second);
        right_bottom.first = max(right_bottom.first, new_box.first);
        right_bottom.second = max(right_bottom.second, new_box.second);
    }
}


/* Definicja operatorów klasy RectArea */

/*
 * Domyślny operator kopiujący przypisania.
 */
RectArea& RectArea::operator=(const RectArea& that) = default;

/*
 * Domyślny operator przenoszący przypisania.
 */
RectArea& RectArea::operator=(RectArea&& that) = default;

/*
 * Operator intersekcji obszarów RectArea.
 */
RectArea& RectArea::operator*=(const RectArea& that) {
    left_top.first = max(left_top.first, that.left_top.first);
    left_top.second = max(left_top.second, that.left_top.second);
    right_bottom.first = min(right_bottom.first, that.right_bottom.first);
    right_bottom.second = min(right_bottom.second, that.right_bottom.second);
    return *this;
}

/*
 * Operator dwuargumentowy intersekcji obszarów RectArea.
 */
const RectArea operator*(const RectArea& a, const RectArea& b) {
    return RectArea(a) *= b;
}


/* Klasa Word */

/* Definicja publicznych metod klasy Word */

/*
 * Funkcja przekazuje pozycję pierwszej litery słowa.
 */
pos_t Word::get_start_position() const {
    return pos_t(column, row);
}

/*
 * Funkcja przekazuje pozycję ostatniej litery słowa.
 */
pos_t Word::get_end_position() const {
    if (orientation == H) {
        return pos_t(column + w_length - 1, row);
    } else {
        return pos_t(column, row + w_length - 1);
    }
}

/*
 * Funkcja przekazuje kierunek słowa.
 */
orientation_t Word::get_orientation() const {
    return orientation;
}

/*
 * Funkcja przekazuje znak występujący w słowie na pozycji 'index'.
 */
char Word::at(size_t index) const {
    if (index >= w_length)
        return DEFAULT_CHAR;
    return word[index];
}

/*
 * Funkcja przekazuje długość słowa.
 */
size_t Word::length() const {
    return w_length;
}

/*
 * Funkcja przekazuje obszar zajmowany przez słowo.
 */
RectArea Word::rect_area() const {
    return RectArea(get_start_position(), get_end_position());
}


/* Definicja operatorów klasy Word */

/*
 * Domyślny operator kopiujący przypisania.
 */
Word& Word::operator=(const Word& that) = default;

/*
 * Domyślny operator przenoszący przypisania.
 */
Word& Word::operator=(Word&& that) = default;

/*
 * Operator porównania trójkierunkowego.
 */
strong_ordering Word::operator<=>(const Word& that) const {
    if (column < that.column)
        return strong_ordering::less;
    if (column > that.column)
        return strong_ordering::greater;
    if (row < that.row)
        return strong_ordering::less;
    if (row > that.row)
        return strong_ordering::greater;
    if (orientation < that.orientation)
        return strong_ordering::less;
    if (orientation > that.orientation)
        return strong_ordering::greater;
    return strong_ordering::equal;
}

/*
 * Operator równości.
 */
bool Word::operator==(const Word& that) const {
    if (*this<=>that == strong_ordering::equal) {
        return true;
    }
    return false;
}

/*
 * Operator nierówności.
 */
bool Word::operator!=(const Word& that) const {
    return !(*this == that);
}


/* Klasa Crossword */

/* Definicja publicznych metod klasy Crossword */

/*
 * Funkcja przekazuje rozmiar przestrzeni, na której zawarta jest krzyżówka.
 */
dim_t Crossword::size() const {
    return area.size();
}

/*
 * Funkcja przekazuje liczbę słów w krzyżówce
 * zorientowanych horyzontalnie i wertykalnie.
 */
dim_t Crossword::word_count() const {
    size_t h = 0;
    size_t v = 0;
    for (Word word : word_vector) {
        if (word.get_orientation() == H) {
            h++;
        } else {
            v++;
        }
    }
    return {h, v};
}

/*
 * Funkcja sprawdza czy 'new_word' może zostać dodane do krzyżówki,
 * a następnie jeśli to możliwe dodaje je i rozszerza RectArea.
 */
bool Crossword::insert_word(const Word& new_word) {
    if (check_collision(new_word)) {
        return false;
    }
    word_vector.push_back(new_word);
    area.embrace(new_word.get_start_position());
    area.embrace(new_word.get_end_position());
    return true;
}


/* Definicja prywatnych metod klasy Crossword */

/*
 * Funkcja sprawdzająca czy wystepuje kolizja między słowem 'word',
 * a słowami zawartymi w krzyżówce.
 */
bool Crossword::check_collision(const Word& that_word) {
    for (Word word : word_vector) {
        if (that_word == word) {
            return true;
        }

        if (check_intersection(word, that_word)) {
            return true;
        }
    }
    return false;
}


/* Definicja operatorów klasy Crossword */

/*
 * Operator kopiujący przypisania.
 */
Crossword& Crossword::operator=(const Crossword& that) {
    word_vector = that.word_vector;
    if (word_vector.empty()) {
        area = DEFAULT_EMPTY_RECT_AREA;
        return *this;
    }
    area = that.area;
    return *this;
}

/*
 * Operator przenoszący przypisania.
 */
Crossword& Crossword::operator=(Crossword&& that) {
    word_vector = move(that.word_vector);
    if (word_vector.empty()) {
        area = DEFAULT_EMPTY_RECT_AREA;
        return *this;
    }
    area = move(that.area);
    return *this;
}

/*
 * Operator sumy obszarów i zawartości krzyżówek.
 */
Crossword& Crossword::operator+=(const Crossword& that) {
    for (Word word : that.word_vector) {
        insert_word(word);
    }
    return *this;
}

/*
 * Operator dwuargumentowy sumy obszarów i zawartości krzyżówek.
 */
const Crossword operator+(const Crossword& a, const Crossword& b) {
    return Crossword(a) += b;
}

/*
 * Operator wyświetlania zawartości krzyżówki.
 */
ostream& operator<<(ostream& os, const Crossword& crossword) {
    list_t word_vector = crossword.word_vector;
    RectArea area = crossword.area;
    size_t columns = area.size().first;
    size_t rows = area.size().second;
    pos_t left_top = area.get_left_top();
    size_t left_top_column = left_top.first;
    size_t left_top_row = left_top.second;
    placement_t letter_layout = get_layout(word_vector);

    print_empty_row(os, columns);

    for (size_t i = 0; i < rows; i++) {
        os << CROSSWORD_BACKGROUND << ' ';
        for (size_t j = 0; j < columns; j++) {
            if (letter_layout.contains(left_top_row + i) &&
                letter_layout[left_top_row + i].contains(left_top_column + j)) {
                os << letter_layout[left_top_row + i][left_top_column + j];
                os << ' ';
            } else {
                os << CROSSWORD_BACKGROUND << ' ';
            }
        }
        os << CROSSWORD_BACKGROUND << "\n";
    }

    print_empty_row(os, columns);

    return os;
}