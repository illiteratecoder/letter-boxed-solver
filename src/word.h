/*
 * File: word.h
 * Author: Jeremy Ephron
 * ---------------------
 * Definition of the Word object, which simply wraps a string together with the
 * number of unique letters the string contains.
 */

#ifndef Word_
#define Word_

#include <iostream>
#include <string>
#include <unordered_set>

struct Word {
    std::string content;
    size_t nUniqueLetters;

    Word(const std::string &word) : content(word) {
        std::unordered_set<char> letters;
        for (char ch : word) letters.insert(ch);
        nUniqueLetters = letters.size();
    }

    size_t size() const { return content.size(); }

    friend bool operator==(const Word &lhs, const Word &rhs) {
        return lhs.content == rhs.content;
    }

    friend bool operator<(const Word &lhs, const Word &rhs) {
        return lhs.content < rhs.content;
    }

    friend bool operator<(const Word &lhs, const std::string &rhs) {
        return lhs.content < rhs;
    }

    friend bool operator<(const std::string &lhs, const Word &rhs) {
        return lhs < rhs.content;
    }

    friend std::ostream& operator<<(std::ostream &out, const Word &word) {
        return out << word.content;
    }

    char operator[](int index) const {
        if (index < 0 || index >= this->content.length()) {
            std::cout << "Word index out of bounds, exiting." << std::endl;
            exit(0);
        }

        return this->content[index];
    }
};

namespace std {
    template <>
    struct std::hash<Word> {
        typedef Word argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type &word) const {
            return std::hash<std::string>()(word.content);
        }
    };
}

#endif
