/*
 * =====================================================
 * Trzeci projekt zaliczeniowy z
 * przedmiotu Kurs Programowania w C++ (1000-213bCPP).
 *
 * Plik nagłówkowy modułu 'crosswords'.
 *
 * Plik zawiera implementacje klas wykorzystywanych
 * do tworzenia i wyświetlania krzyżówek.
 *
 * Autorzy:
 * Karol Baciński <kb448449@students.mimuw.edu.pl>
 * Dawid Pawlik   <dp450244@students.mimuw.edu.pl>
 * =====================================================
 */


/* Początek deklaracji elementów modułu crosswords.h */
#ifndef CROSSWORDS_H
    #define CROSSWORDS_H


    /* Pliki nagłówkowe wykorzystywane przy implementacji modułu */

    #include <string>
    #include <utility>
    #include <vector>
    #include <cstdint>
    #include <initializer_list>


    /* Deklaracje użycia wybranych elementów z przestrzeni std */

    using std::string;
    using std::pair;
    using std::vector;
    using std::move;
    using std::strong_ordering;
    using std::initializer_list;
    using std::ostream;


    /* Aliasy używanych typów */

    using pos_t = pair<size_t, size_t>;
    using dim_t = pair<size_t, size_t>;


    /* Wyrażenia domyślne używane w implementacji modułu */

    constexpr char DEFAULT_CHAR = '?';
    constexpr string DEFAULT_WORD = "?";
    constinit extern char CROSSWORD_BACKGROUND;


    /* Wyrażenie wyliczeniowe definiujące kierunek słowa wraz z jego aliasami */

    enum class orientation_t : bool {H = 0, V = 1};

    using orientation_t::H;
    using orientation_t::V;


    /* Klasy implementowane w module */

    /* Klasa RectArea */

    /*
     * Klasa reprezentuje prostokąt zadany przeciwległymi wierzchołkami.
     */
    class RectArea {
        private:
            /* Prywatne pola klasy RectArea */

            pos_t left_top;
            pos_t right_bottom;


        public:
            /* Konstruktory klasy RectArea */

            /*
             * Konstruktor obiektu klasy RectArea.
             */
            constexpr RectArea(pos_t left_top, pos_t right_bottom) :
                     left_top(left_top), right_bottom(right_bottom) {
            }

            /*
             * Domyślny konstruktor kopiujący klasy RectArea.
             */
            RectArea(const RectArea& that) = default;

            /*
             * Domyślny konstruktor przenoszący klasy RectArea.
             */
            RectArea(RectArea&& that) = default;


            /* Deklaracja użycia domyślnego destruktora klasy RectArea */

            ~RectArea() = default;


            /* Deklaracja metod klasy RectArea */

            pos_t get_left_top() const;
            pos_t get_right_bottom() const;
            void set_left_top(const pos_t& lt);
            void set_right_bottom(const pos_t& rb);
            dim_t size() const;
            bool empty() const;
            void embrace(const pos_t& new_box);


            /* Deklaracja operatorów klasy RectArea */

            RectArea& operator=(const RectArea& that);
            RectArea& operator=(RectArea&& that);
            RectArea& operator*=(const RectArea& that);
            friend const RectArea operator*(const RectArea& a,
                                            const RectArea& b);


    }; // Koniec deklaracji klasy RectArea.

    /*
     * Domyślny obiekt klasy RectArea.
     */
    constexpr RectArea DEFAULT_EMPTY_RECT_AREA(pos_t(1, 1), pos_t(0, 0));


    /* Klasa Word */

    /*
     * Klasa reprezentująca słowo umieszczone w przestrzeni RectArea.
     */
    class Word {
        private:
            /* Prywatne pola klasy Word */

            size_t column;
            size_t row;
            orientation_t orientation;
            string word;
            size_t w_length;


        public:
            /* Konstruktory klasy Word */

            /*
             * Konstruktor obiektu klasy Word.
             */
            Word(size_t x, size_t y, orientation_t o, const string& letters) :
                                     column(x), row(y), orientation(o),
                                     word(letters), w_length(letters.length()) {
                if (!w_length) {
                    word = DEFAULT_WORD;
                    w_length++;
                } else {
                    // Skracamy słowo jeśli wykracza poza maksymalny rozmiar.
                    pos_t end = get_end_position();
                    if (orientation==H) {
                        if (column > end.first) {
                            w_length -= (end.first + 1);
                        }
                    }
                    else {
                        if (row > end.second) {
                            w_length -= (end.second + 1);
                        }
                    }
                    word.resize(w_length);

                    // Konwertujemy znaki na wielkie litery lub domyślny znak.
                    for (size_t i = 0; i < w_length; i++) {
                        char letter = letters[i];
                        if ((letter < 'A' || letter > 'Z') &&
                            (letter < 'a' || letter > 'z')) {
                            word[i] = DEFAULT_CHAR;
                        } else if (letter >= 'a') {
                            word[i] += 'A' - 'a';
                        }
                    }
                }
            }

            /*
             * Domyślny konstruktor kopiujący klasy Word.
             */
            Word (const Word& that) = default;

            /*
             * Domyślny konstruktor przenoszący klasy Word.
             */
            Word (Word&& that) = default;


            /* Deklaracja użycia domyślnego destruktora klasy Word */

            ~Word() = default;


            /* Deklaracja metod klasy Word */

            pos_t get_start_position() const;
            pos_t get_end_position() const;
            orientation_t get_orientation() const;
            char at(size_t index) const;
            size_t length() const;
            RectArea rect_area() const;


            /* Deklaracja operatorów klasy Word */

            Word& operator=(const Word& that);
            Word& operator=(Word&& that);
            strong_ordering operator<=>(const Word& that) const;
            bool operator==(const Word& that) const;
            bool operator!=(const Word& that) const;


    }; // Koniec deklaracji klasy Word.


    /* Klasa Crossword */

    /*
     * Klasa reprezentująca krzyżówkę przechowującą słowa i ich obszar RectArea.
     */
    class Crossword {
        private:
            /* Prywatne pola klasy Word */

            vector<Word> word_vector;
            RectArea area;


            /* Deklaracja prywatnych metod klasy Crossword */

            bool check_collision(const Word& that_word);


        public:
            /* Konstruktory klasy Crossword */

            /*
             * Konstruktor obiektu klasy Crossword.
             */
            Crossword(const Word& initial_word,
                      const initializer_list<Word>& list_of_words) :
                      area(DEFAULT_EMPTY_RECT_AREA) {
                insert_word(initial_word);
                for (Word word : list_of_words) {
                    insert_word(word);
                }
            }

            /*
             * Konstruktor kopiujący klasy Crossword.
             */
            Crossword(const Crossword& that) :
                word_vector(that.word_vector), area(that.area) {
                if (word_vector.empty()) {
                    area = DEFAULT_EMPTY_RECT_AREA;
                }
            }

            /*
             * Konstruktor przenoszący klasy Crossword.
             */
            Crossword(Crossword&& that) :
                word_vector(move(that.word_vector)), area(move(that.area)) {
                if (word_vector.empty()) {
                    area = DEFAULT_EMPTY_RECT_AREA;
                }
            }


            /* Deklaracja użycia domyślnego destruktora klasy Crossword */

            ~Crossword() = default;


            /* Deklaracja metod klasy Crossword */

            dim_t size() const;
            dim_t word_count() const;
            bool insert_word(const Word& new_word);


            /* Deklaracja operatorów klasy Crossword */

            Crossword& operator=(const Crossword& that);
            Crossword& operator=(Crossword&& that);
            Crossword& operator+=(const Crossword& that);
            friend const Crossword operator+(const Crossword& a,
                                             const Crossword& b);
            friend ostream& operator<<(ostream& os, const Crossword& crossword);


    }; // Koniec deklaracji klasy Crossword.


#endif // Koniec header-guard.