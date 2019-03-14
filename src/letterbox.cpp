/*
 * File: letterbox.cpp
 * Author: Jeremy Ephron
 * ---------------------
 * The implementation of the LetterBox class.
 */

#include "letterbox.h"

const size_t LetterBox::kNumWalls = 4;
const size_t LetterBox::kMinWordLength = 3;

LetterBox::LetterBox(const std::string &letters) {
    unsigned int lettersPerWord = letters.size() / kNumWalls;
    for (unsigned int wallNum = 0; wallNum < kNumWalls; wallNum++) {
        std::vector<char> *p_wall = new std::vector<char>;

        for (size_t i = 0; i < lettersPerWord; i++) {
            size_t idx = wallNum * lettersPerWord + i;
            p_wall->push_back(letters[idx]);
            this->letters.insert(letters[idx]);
            if (letterToWall.count(letters[idx]) == 0) {
                letterToWall[letters[idx]] = p_wall;
            }
        }

        this->letterWalls.push_back(p_wall);
    }
}

LetterBox::~LetterBox() {
    for (auto ptr : this->letterWalls) delete ptr;
}

std::unordered_set<char> LetterBox::getLetters() const {
    return this->letters;
}

std::vector<char> *LetterBox::getWall(char letter) const {
    return this->letterToWall.find(letter)->second;
}

bool LetterBox::onSameWall(char letter1, char letter2) const {
    auto iter1 = this->letterToWall.find(letter1);
    if (iter1 == this->letterToWall.end()) return false;

    auto iter2 = this->letterToWall.find(letter2);
    if (iter2 == this->letterToWall.end()) return false;

    return iter1->second == iter2->second;
}

bool LetterBox::contains(char letter) const {
    return this->letters.count(letter) > 0;
}

bool LetterBox::canMakeWord(const std::string &word) const {
    if (word.length() < kMinWordLength) return false;

    for (size_t i = 0; i < word.length(); i++) {
        if (!contains(word[i]) || (i > 0 && onSameWall(word[i - 1], word[i])))
            return false;
    }

    return true;
}

size_t LetterBox::numLetters() const {
    return this->letters.size();
}

