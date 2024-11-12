/**
 * Implementation of the Porter Stemmer algorithm. 
 * 
 * Porter Stemmer is a stemming algorithm that removes suffixes from words
 * to extract the stem of the given word. In a more simpler terms, this algorithm
 * extracts the base word from different forms of word.
 * 
 * For example, the algorithm can extract the base word (CONNECT) from the
 * following words: 
 * 
 * CONNECT
 * CONNECTS
 * CONNECTION
 * CONNECTIONS
 * CONNECTING
 * CONNECTED
 * 
 * This implementation is based on the details of this algorithm documented here:
 * https://people.scs.carleton.ca/~armyunis/projects/KAPI/porter.pdf
 * 
 * Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025
 */

#include <array>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utils.cpp>

const std::string STEP_2_SUFFIXES[][2] = {
    {"ational", "ate"},
    {"tional", "tion"},
    {"enci", "ence"},
    {"anci", "ance"},
    {"izer", "ize"},
    {"abli", "able"},
    {"alli", "al"},
    {"entli", "ent"},
    {"eli", "e"},
    {"ousli", "ous"},
    {"ization", "ize"},
    {"ation", "ate"},
    {"ator", "ate"},
    {"alism", "al"},
    {"iveness", "ive"},
    {"fulness", "ful"},
    {"ousness", "ous"},
    {"aliti", "al"},
    {"iviti", "ive"},
    {"biliti", "ble"},
};

const std::string STEP_3_SUFFIXES[][2] = {
    {"icate", "ic"},
    {"ative", ""},
    {"alize", "al"},
    {"iciti", "ic"},
    {"ical", "ic"},
    {"ful", ""},
    {"ness", ""},
};

const std::string STEP_4_SUFFIXES[][2] = {
    {"al", ""},
    {"ance", ""},
    {"ence", ""},
    {"er", ""},
    {"ic", ""},
    {"able", ""},
    {"ible", ""},
    {"ant", ""},
    {"ement", ""},
    {"ment", ""},
    {"ent", ""},
    {"ou", ""},
    {"ism", ""},
    {"ate", ""},
    {"iti", ""},
    {"ous", ""},
    {"ive", ""},
    {"ize", ""},
};


// These hash maps are used as a small performance booster when looking up S1
// suffix in the 2D arrays above. These arrays are sorted in order of
// penultimate (second to last) characters so these hash maps maps a character
// to index range (2-element array) in which suffixes for that character are stored.
// For step 2 and 4, map is for penultimate character i.e. second last characters
// For step 3, map is for ultimate character i.e. last characters
// This is due to how suffixes are sorted in the arrays above for each step.
const std::unordered_map<char, std::array<int, 2>> STEP_2_PENULT_MAP = {
    {'a', {0, 2}},
    {'c', {2, 4}},
    {'e', {4, 5}},
    {'l', {5, 10}},
    {'o', {10, 13}},
    {'s', {13, 17}},
    {'t', {17, 20}},
    {'u', {20, 21}},
};

const std::unordered_map<char, std::array<int, 2>> STEP_3_ULT_MAP = {
    {'e', {0, 3}},
    {'i', {3, 4}},
    {'l', {4, 6}},
    {'s', {6, 7}},
};

const std::unordered_map<char, std::array<int, 2>> STEP_4_PENULT_MAP = {
    {'a', {0, 1}},
    {'c', {1, 3}},
    {'e', {3, 4}},
    {'i', {4, 5}},
    {'l', {5, 7}},
    {'n', {7, 11}},
    {'o', {11, 12}},
    {'s', {12, 13}},
    {'t', {13, 15}},
    {'u', {15, 16}},
    {'v', {16, 17}},
    {'z', {17, 18}},
};

class PorterStemmer
{
    public:

    std::string stemSentence(std::string text)
    {
        std::istringstream iss(text);
        std::string word;
        std::string result = "";

        while (std::getline(iss, word, ' '))
        {
            result += stem(word) + " ";
        }

        // L - 1 to strip the leading space
        return result.substr(0, result.length() - 1);
    }

    std::string stem(std::string text)
    {
        data = stringToLower(text);

        step1a();
        step1b();
        step1c();
        step2();
        step3();
        step4();
        step5a();
        step5b();

        return data;
    }

    protected:

    std::string data;

    /**
     * @brief Determines whether character at given index is consonant.
     * 
     * In Porter Stemmer's specification, consonant is any alphabetical letter
     * other than A, E, I, O, U, and Y after a consonant. Anything that's not a
     * consonant is a vowel.
     * 
     * TOY -> T and Y are consonant.
     * SYZYGY -> S, Z, and G are consonant (Y is a vowel as it has consonant before it)
     * 
     * @param index: the index of character to test from string.
     * @return bool
     * */
    bool isConsonant(int index)
    {
        char c = data.at(index);
        if ((c == 'a') || (c == 'e') || (c == 'i') || (c == 'o') || (c == 'u'))
            return false;

        if (c == 'y')
        {
            if (index == 0)
                return true;

            if (isConsonant(index - 1))
                return false;
        }

        return true;
    }

    /**
     * @brief Determines the value of m.
     * 
     * As described in algorithm's specification, m is the measure of a word or word part.
     * 
     * If we take C and V as as sequence of consonants and vowels respectively then
     * in word of form:
     * 
     * `[C](VC){m}[V]`
     * 
     * [C] and [V] indicate arbitrary presence of C and V and (VC){m} indicates presence of
     * V and C, in order, "m" number of times.
     * 
     * For more information, see algorithm's specification.
     * 
     * @param suffix_length: The length of suffix that will be removed to obtain stem.
     * 
     * @return the value of m
     * */
    int getm(int suffix_length)
    {
        int i;
        int start = -1;
        int m = 0;
        int len = data.length() - suffix_length;
        std::string old_data = data;
        data.replace(len, suffix_length, "");

        for (i = 0; i < len; i++)
        {
            if (!isConsonant(i))
            {
                start = i;
                break;
            }
        }

        if (start == -1)
        {
            data = old_data;
            return 0;
        }

        int end = -1;

        for (i = len - 1; i > start; i--)
        {
            if (isConsonant(i))
            {
                end = i;
                break;
            }
        }

        if ((end < start) || (end == -1))
        {
            data = old_data;
            return 0;
        }

        bool v = true;
        for (i = start; i <= end; i++)
        {
            if (isConsonant(i) && v)
            {
                m += 1;
                v = false;
            }
            else if (!isConsonant(i) && !v)
                v = true;
        }

        data = old_data;
        return m;
    }

    /**
     * @brief Checks whether data contains a vowel.
     * 
     * @param suffix_length: The length of suffix that will be removed to obtain stem.
     * 
     * @returns true if data contains vowel and vice versa.
     */
    bool containsVowel(int suffix_length)
    {
        std::string old_data = data;
        data.replace(data.length() - suffix_length, suffix_length, "");

        bool res;
        if (data.find('a') != std::string::npos)
            res = true;
        else if (data.find('e') != std::string::npos)
            res = true;
        else if (data.find('i') != std::string::npos)
            res = true;
        else if (data.find('o') != std::string::npos)
            res = true;
        else if (data.find('u') != std::string::npos)
            res = true;

        if (res)
        {
            data = old_data;
            return res;
        }

        int y_index = data.find('y');
        if ((y_index == std::string::npos) || (y_index == 0))
            res = false;
        else
            res = isConsonant(y_index - 1);

        data = old_data;
        return res;
    }

    /**
     * @brief Checks if the string ends with a double (same) consonant.
     * 
     * fuzz -> true
     * 
     * buzz -> true
     * 
     * boys -> false
     * 
     * @param suffix_length: The length of suffix that will be removed to obtain stem.
     *
     * @returns boolean
     */
    bool doubleConsonantSuffix(int suffix_length)
    {
        int len = data.length() - suffix_length;

        if (len < 2)
            return false;

        std::string old_data = data;
        data.replace(len, suffix_length, "");

        int last = len - 1;
        int second_last = len - 2;

        bool res;
        if (isConsonant(last))
            res = data.at(second_last) == data.at(last);
        else
            res = false;

        data = old_data;
        return res;
    }

    /**
     * @brief Checks whether string ends with cvc sequence.
     * 
     * c and v are consonant and vowel respectively and second c in cvc
     * is not W, X, or Y.
     * 
     * @param suffix_length: The length of suffix that will be removed to obtain stem.
     * 
     * @returns boolean
     */
    bool endsCVC(int suffix_length)
    {
        int len = data.length() - suffix_length;
        std::string old_data = data;
        data.replace(len, suffix_length, "");

        if (len < 3)
            return false;

        int c1_index = len - 3;
        int v_index = len - 2;
        int c2_index = len - 1;
        char c2 = data.at(c2_index);

        bool res;
        if (isConsonant(c1_index) && !isConsonant(v_index) && isConsonant(c2_index))
            res = ((c2 != 'w') && (c2 != 'x') && (c2 != 'y'));
        else
            res = false;

        data = old_data;
        return res;
    }

    void step1a()
    {
        int len = data.length();
        if (stringEndsWith(data, "sses"))
            data.replace(data.length() - 4, 4, "ss");
        else if (stringEndsWith(data, "ies"))
            data.replace(data.length() - 3, 3, "i");
        else if (stringEndsWith(data, "s") && !stringEndsWith(data, "ss"))
            data.pop_back();
    }

    void step1b()
    {
        bool followup = false;
        if (stringEndsWith(data, "eed"))
        {
            if ((getm(3) > 0))
                data.replace(data.length() - 3, 3, "ee");
        }
        else if (stringEndsWith(data, "ing"))
        {
            if (containsVowel(3))
            {
                data.replace(data.length() - 3, 3, "");
                followup = true;
            }
        }
        else if (stringEndsWith(data, "ed"))
        {
            if (containsVowel(2))
            {
                followup = true;
                data.replace(data.length() - 2, 2, "");;
            }
        }

        if (followup)
        {
            if (stringEndsWith(data, "at") || stringEndsWith(data, "bl") || stringEndsWith(data, "iz"))
                data.push_back('e');
            else if (doubleConsonantSuffix(0))
            {
                if (!(stringEndsWith(data, "l") || stringEndsWith(data, "s") || stringEndsWith(data, "z")))
                    data.pop_back();
            }
            else if (endsCVC(0))
            {
                if (getm(0) == 1)
                    data.push_back('e');
            }
        }
    }

    void step1c()
    {
        if (stringEndsWith(data, "y"))
        {
            if (containsVowel(1))
                data.replace(data.length() - 1, 1, "i");
        }
    }

    void processSuffixArray(const std::string arr[][2], int start, int end, int m)
    {
        for (int i = start; i < end; i++)
        {
            std::string s1 = arr[i][0];
            std::string s2 = arr[i][1];

            if (stringEndsWith(data, s1))
            {
                int s1_len = s1.length();
                if (getm(s1_len) > m)
                {
                    data.replace(data.length() - s1_len, s1_len, s2);
                    break;
                }
            }
        }
    }

    void getSuffixBounds(const std::unordered_map<char, std::array<int, 2>> hash_map, char c, int &start, int &end)
    {
        start = 0;
        end = 0;

        if (!hash_map.count(c))
            return;

        auto bounds = hash_map.at(c);
        start = bounds[0];
        end = bounds[1];
    }

    void step2()
    {
        int start, end;
        int len = data.length();

        if (len < 2)
            return;

        getSuffixBounds(STEP_2_PENULT_MAP, data.at(len - 2), start, end);
        processSuffixArray(STEP_2_SUFFIXES, start, end, 0);
    }

    void step3()
    {
        int start, end;
        int len = data.length();

        if (len < 1)
            return;

        getSuffixBounds(STEP_3_ULT_MAP, data.at(len - 1), start, end);
        processSuffixArray(STEP_3_SUFFIXES, start, end, 0);
    }

    void step4()
    {
        int start, end;
        int len = data.length();

        if (len < 2)
            return;
        
        // The -ION suffix requires special treatment because
        // it has *S and *T forms in condition as well that
        // processPrefixArray does not handle.
        if (stringEndsWith(data, "ion"))
        {
            std::string old_data = data;
            data = data.replace(len - 3,  3, "");
            if (stringEndsWith(data, "s") || stringEndsWith(data, "t"))
            {
                if (!(getm(0) > 1))
                    data = old_data;
            }
            else
                data = old_data;

            return;
        }

        getSuffixBounds(STEP_4_PENULT_MAP, data.at(len - 2), start, end);
        processSuffixArray(STEP_4_SUFFIXES, start, end, 1);
    }

    void step5a()
    {
        if (stringEndsWith(data, "e"))
        {
            int m = getm(1);
            if ((m > 1) || ((m == 1) && !endsCVC(1)))
                data.replace(data.length() - 1, 1, "");
        }
    }

    void step5b()
    {
        int m = getm(0);
        if ((m > 1) && doubleConsonantSuffix(0) && stringEndsWith(data, "l"))
            data.replace(data.length() - 1, 1, "");
    }
};
