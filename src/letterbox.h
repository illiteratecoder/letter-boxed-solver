/*
 * File: letterbox.h
 * Author: Jeremy Ephron
 * ---------------------
 * The interface for the LetterBox class. Represents the walls (and rules) of
 * the Letter Boxed puzzle.
 *
 * We assume that each letter can only appear once in a Letter Box.
 */

#ifndef Letter_Box
#define Letter_Box

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class LetterBox {
public:  /* Interface */

    /** Creates a LetterBox puzzle from a string of letters. */
    LetterBox(const std::string &letters);

    /** Returns the set of all letters of the LetterBox puzzle. */
    std::unordered_set<char> getLetters() const;

    /** Returns a pointer to the "wall" that a letter belongs to. */
    std::vector<char> *getWall(char letter) const;

    /** Returns true if two letters are on the same wall, false otherwise. */
    bool onSameWall(char letter1, char letter2) const;

    /** Returns true if the letter in contained in the Letter Box. */
    bool contains(char letter) const;

    /** Returns true if a word can be written within the Letter Box. */
    bool canMakeWord(const std::string &word) const;

    /** Returns the number of letters in the Letter Box. */
    size_t numLetters() const;

private:

    // A list of "walls" of chars.
    std::vector<std::vector<char> *> letterWalls;

    // A map from letter to the wall it belongs to.
    std::unordered_map<char, std::vector<char> *> letterToWall;

    // The set of letters contained in the Letter Box.
    std::unordered_set<char> letters;

public:  /* public, but not necessary for most users */

    static const size_t kNumWalls;
    static const size_t kMinWordLength;

    ~LetterBox();
};

#endif
