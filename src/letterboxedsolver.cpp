/*
 * File: letterboxedsolver.cpp
 * Author: Jeremy Ephron
 * ---------------------------
 * This is a program to solve the NY Times online puzzle Letter Boxed
 * (https://www.nytimes.com/puzzles/letter-boxed).
 *
 * The NY Times says they use the Oxford English Dictionary, for which I could
 * not find a text file with all recognized words. As such I am using a
 * Scrabble dictionary text file from
 * https://raw.githubusercontent.com/jonbcard/scrabble-bot/master/src/dictionary.txt
 *
 * This means that not all solutions generated will necessarily be recognized
 * as valid, but there certainly will be some valid solutions.
 *
 * You can modify the dictionary used by editing "dictionary.txt" in the res
 * folder, or adding a dictionary of your own and specifying the dictionary
 * you would like to use.
 *
 * This is a multithreaded implementation, where starting from each letter is
 * handled by it's own thread.
 *
 * !!! Important: each letter can only appear *once* in a letter box puzzle
 *                (this is an assumption this program relies on).
 */

#include <fstream>
#include <thread>
#include "letterbox.h"
#include "word.h"

using WordTable = std::unordered_map<char, std::unordered_set<Word> >;
using Solution = std::vector<std::string>;

static const std::string DEFAULT_DICT = "dictionary.txt";

/* For resetting the input stream */
static inline void reset(std::istream& in) {
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * Function: getDictionaryNameFromUser
 * -----------------------------------
 * Gets a valid dictionary filename from the user.
 *
 * Uses the default dictionary filename if user input is empty, and reprompts
 * if file does not exist.
 *
 * @returns the filename of the chosen dictionary.
 */
std::string getDictionaryNameFromUser();

/**
 * Function: getLettersFromUser
 * ----------------------------
 * Gets the letters constituting the letter box from the user.
 *
 * @returns the string of letters representing the letter box.
 */
std::string getLettersFromUser();

/**
 * Function: getNumWordsFromUser
 * -----------------------------
 * Gets the number of words the user wants in a solution.
 *
 * @param letterBox the LetterBox puzzle object.
 * @returns the number of words in a solution.
 */
unsigned int getNumWordsFromUser(const LetterBox &letterBox);

/**
 * Function: buildFilteredWordList
 * -------------------------------
 * Filters words from a dictionary and populates a table with words grouped by
 * starting character.
 *
 * @param dictFilename the filename of the dictionary to use.
 * @param letterBox the LetterBox puzzle object.
 * @param wordsStartingWith the map from starting character to set of words.
 */
void buildFilteredWordList(const std::string &dictFilename,
                           const LetterBox &letterBox,
                           WordTable &wordsStartingWith);

/**
 * Function: generateSolutions
 * ---------------------------
 * Wrapper function to generate all solutions of a fixed number of words.
 *
 * @param letterBox the LetterBox puzzle object.
 * @param nWords the number of words per solution.
 * @param wordsStartingWith the map from starting characters to sets of words.
 * @param solutions a vector of solutions to be populated.
 */
void generateSolutions(const LetterBox &letterBox, unsigned int nWords,
                       const WordTable &wordsStartingWith,
                       std::vector<Solution>& solutions);

/**
 * Function: generateSolutionsRec
 * ------------------------------
 * Generates solutions to a letterBox puzzle with a given number of words
 * remaining, a given last character typed, a given set of characters
 * remaining, and a given list of already chosen words.
 *
 * @param letterBox the LetterBox puzzle object.
 * @param nWords the number of words left in a possible solution.
 * @param last the last character typed that our next word must start with.
 * @param remaining the set of characters we haven't used yet.
 * @param result the solution being built up.
 * @param solutions the vector of all solutions found so far.
 * @param solutionsLock a lock acquired before adding to solutions.
 * @param wordsStartingWith the map from starting characters to sets of words.
 */
void generateSolutionsRec(const LetterBox &letterBox,
                          unsigned int nWords,
                          char last,
                          std::unordered_set<char> &remaining,
                          Solution &result,
                          std::vector<Solution> &solutions,
                          std::mutex &solutionsLock,
                          const WordTable &wordsStartingWith);

/**
 * Function: writeSolutionsToFile
 * ------------------------------
 * Writes solutions to a file, specified by the user.
 *
 * @param filename the name of the output file.
 * @param solutions the solutions to the LetterBox puzzle.
 */
void writeSolutionsToFile(const std::string &filename,
                          const std::vector<Solution> &solutions);

/**
 * Function: fileExists
 * --------------------
 * Checks whether a given file exists.
 *
 * @param filename the name of the file to check.
 * @returns true if the file exists, false otherwise.
 */
bool fileExists(const std::string &filename);

int main() {
    std::string dictionaryFilename = getDictionaryNameFromUser();

    std::string letters = getLettersFromUser();
    LetterBox letterBox(letters);

    WordTable wordsStartingWith;
    buildFilteredWordList(dictionaryFilename, letterBox, wordsStartingWith);

    unsigned int nWords = getNumWordsFromUser(letterBox);

    std::cout << "Please be patient, finding all solutions can take a few "
                 "minutes for n > 2."
              << std::endl;

    std::vector<Solution> solutions;
    generateSolutions(letterBox, nWords, wordsStartingWith, solutions);

    std::cout << solutions.size() << " solution(s) found! Would you like to "
                                     "save them to a file? (y/n): ";

    std::string line;
    getline(std::cin, line);
    if (!line.empty() && tolower(line[0]) == 'y') {
        std::cout << "Enter the output filename: ";
        getline(std::cin, line);
        writeSolutionsToFile(line, solutions);
    } else {
        std::cout << "Alright, goodbye!" << std::endl;
    }

    std::cout << "Have a nice day!" << std::endl;
    return 0;
}

std::string getDictionaryNameFromUser() {
    std::cout << "Enter the filename of the dictionary you want to use "
                 "(hit enter for \"dictionary.txt\"): ";

    std::string dictionaryFilename;
    std::getline(std::cin, dictionaryFilename);

    while (dictionaryFilename != "" && !fileExists(dictionaryFilename)) {
        std::cout << "File \"" << dictionaryFilename << "\" does not exist. "
                     "Please try again: ";

        std::getline(std::cin, dictionaryFilename);
    }

    return dictionaryFilename != "" ? dictionaryFilename : DEFAULT_DICT;
}

std::string getLettersFromUser() {
    std::cout << "Enter each letter such that entire walls "
                 "are typed in consecutively: ";

    std::string input;
    std::getline(std::cin, input);

    while (input.length() % LetterBox::kNumWalls != 0) {
        std::cout << "Please enter a multiple of " << LetterBox::kNumWalls
                  << " letters:";

        std::getline(std::cin, input);
    }

    for (size_t i = 0; i < input.length(); i++) {
        input[i] = toupper(input[i]);
    }

    return input;
}

unsigned int getNumWordsFromUser(const LetterBox &letterBox) {
    unsigned int minWords = 1;
    unsigned int maxWords = letterBox.numLetters() / LetterBox::kMinWordLength;

    std::cout << "Please enter the number of words you want in your solution: ";

    int n;
    std::cin >> n;
    while (n < minWords || n > maxWords) {
        reset(std::cin);
        std::cout << "Please enter a number between " << minWords << " and "
                  << maxWords << ": ";
        std::cin >> n;
    }
    reset(std::cin);

    return n;
}

void generateSolutions(const LetterBox &letterBox, unsigned int nWords,
                       const WordTable &wordsStartingWith,
                       std::vector<Solution>& solutions) {
    std::mutex solutionsLock;
    std::vector<std::thread> threads;
    for (char ch : letterBox.getLetters()) {
        threads.push_back(std::thread([&](char ch) {
            std::unordered_set<char> remaining = letterBox.getLetters();
            Solution result(nWords);

            generateSolutionsRec(letterBox, nWords, ch, remaining, result,
                                 solutions, solutionsLock, wordsStartingWith);
        }, ch));
    }

    for (std::thread& t : threads) t.join();
}

void generateSolutionsRec(const LetterBox &letterBox,
                          unsigned int nWords,
                          char last,
                          std::unordered_set<char> &remaining,
                          Solution &result,
                          std::vector<Solution> &solutions,
                          std::mutex &solutionsLock,
                          const WordTable &wordsStartingWith) {
    if (nWords == 0) {
        if (remaining.empty()) {
            std::lock_guard<std::mutex> lg(solutionsLock);
            solutions.push_back(result);
        }
        return;
    }

    if (wordsStartingWith.count(last) == 0) return;

    for (const auto& word : wordsStartingWith.at(last)) {
        if (nWords == 1 && remaining.size() > word.nUniqueLetters) continue;

        auto remainingCopy = remaining;
        for (char ch : word.content) remainingCopy.erase(ch);
        result[result.size() - nWords] = word.content;
        generateSolutionsRec(letterBox, nWords - 1, word[word.size() - 1],
                             remainingCopy, result, solutions,
                             solutionsLock, wordsStartingWith);
    }
}

void buildFilteredWordList(const std::string &dictFilename,
    const LetterBox &letterBox,
    std::unordered_map<char, std::unordered_set<Word> > &wordsStartingWith) {
    std::ifstream file(dictFilename);
    std::string word;
    while (getline(file, word)) {
        if (!letterBox.canMakeWord(word)) continue;
        wordsStartingWith[word[0]].insert(Word(word));
    }
}

void writeSolutionsToFile(const std::string &filename,
                          const std::vector<Solution> &solutions) {
    std::ofstream out(filename);

    for (const auto &solution : solutions) {
        for (const auto &word : solution) {
            out << word << " ";
        }
        out << std::endl;
    }
}

bool fileExists(const std::string &filename) {
    return bool(std::ifstream(filename));
}
