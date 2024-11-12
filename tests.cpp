/** 
 * Unit tests for Search100
 * 
 * This file contains tests for various components of the application. For simplicity
 * sake, the tests are written without any framework so a lot of work is done manually.
 * The IS_EQ directive used for testing is based on this SO answer:
 * 
 * https://stackoverflow.com/a/52314467
 * 
 * Copyright (C) Izhar Ahmad & Mustafa Hussain, 2024-2025
 *
 * */ 

#include <iostream>
#include <string>
#include <utils.cpp>
#include <stemming.cpp>

#define IS_EQ(x, y) { if (x != y) { std::cout << __FUNCTION__ << " failed on line " << __LINE__ << " (" << x << " != " << y << ")" << std::endl; }}


/* -- src/utils.cpp -- */

void testStringEndsWith()
{
    IS_EQ(stringEndsWith("Hello World", "rld"), true);
    IS_EQ(stringEndsWith("Hello World", ""), true);
    IS_EQ(stringEndsWith("Hello World", "Hello World"), true);
    IS_EQ(stringEndsWith("Hello World", "test"), false);
    IS_EQ(stringEndsWith("Hello", "Hello World"), false);
}

void testStringToLower()
{
    IS_EQ(stringToLower("HELLO"), "hello");
    IS_EQ(stringToLower("HeLlO"), "hello");
    IS_EQ(stringToLower("HellO123"), "hello123");
    IS_EQ(stringToLower("HeLlo:WoRld"), "hello:world");
    IS_EQ(stringToLower("hello"), "hello");
    IS_EQ(stringToLower("1234"), "1234");
    IS_EQ(stringToLower(""), "");
}

/* -- src/stemming.cpp */

class TestablePorterStemmer: public PorterStemmer {
    public:

    void setData(std::string input)
    {
        data = stringToLower(input);
    }

    void testIsConstant()
    {
        setData("syiygaeiou");
        IS_EQ(isConsonant(0), true);
        IS_EQ(isConsonant(1), false);
        IS_EQ(isConsonant(2), false);
        IS_EQ(isConsonant(3), true);
        IS_EQ(isConsonant(4), true);
        IS_EQ(isConsonant(5), false);
        IS_EQ(isConsonant(6), false);
        IS_EQ(isConsonant(7), false);
        IS_EQ(isConsonant(8), false);
        IS_EQ(isConsonant(9), false);
    }

    int getMFromData(std::string input)
    {
        setData(input);
        return getm(0);
    }

    void testGetM()
    {
        IS_EQ(getMFromData("tr"), 0);
        IS_EQ(getMFromData("ee"), 0);
        IS_EQ(getMFromData("tree"), 0);
        IS_EQ(getMFromData("y"), 0);
        IS_EQ(getMFromData("by"), 0);


        IS_EQ(getMFromData("trouble"), 1);
        IS_EQ(getMFromData("oats"), 1);
        IS_EQ(getMFromData("tress"), 1);
        IS_EQ(getMFromData("ivy"), 1);

        IS_EQ(getMFromData("troubles"), 2);
        IS_EQ(getMFromData("private"), 2);
        IS_EQ(getMFromData("oaten"), 2);
        IS_EQ(getMFromData("orrery"), 2);
    }

    bool containsVowelWithData(std::string input)
    {
        setData(input);
        return containsVowel(0);
    }

    void testContainsVowel()
    {
        IS_EQ(containsVowelWithData("hell"), true);
        IS_EQ(containsVowelWithData("hello"), true);
        IS_EQ(containsVowelWithData("sky"), true);
        IS_EQ(containsVowelWithData("skey"), true);
        IS_EQ(containsVowelWithData("szwg"), false);
        IS_EQ(containsVowelWithData(""), false);
    }

    bool doubleConsonantSuffixWithData(std::string input)
    {
        setData(input);
        return doubleConsonantSuffix(0);
    }

    void testDoubleConsonantSuffix()
    {
        IS_EQ(doubleConsonantSuffixWithData("hope"), false);
        IS_EQ(doubleConsonantSuffixWithData("fuzz"), true);
        IS_EQ(doubleConsonantSuffixWithData(""), false);
    }

    bool endsCVCWithData(std::string input)
    {
        setData(input);
        return endsCVC(0);
    }

    void testEndsCVC()
    {
        IS_EQ(endsCVCWithData(""), false);
        IS_EQ(endsCVCWithData("hop"), true);
        IS_EQ(endsCVCWithData("wil"), true);
        IS_EQ(endsCVCWithData("fail"), false);
        IS_EQ(endsCVCWithData("fil"), true);
        IS_EQ(endsCVCWithData("fiw"), false);
        IS_EQ(endsCVCWithData("fix"), false);
        IS_EQ(endsCVCWithData("fiy"), false);
    }

    std::string step1aWithData(std::string input)
    {
        setData(input);
        step1a();
        return data;
    }

    std::string step1bWithData(std::string input)
    {
        setData(input);
        step1b();
        return data;
    }

    std::string step1cWithData(std::string input)
    {
        setData(input);
        step1c();
        return data;
    }

    void testStep1()
    {
        IS_EQ(step1aWithData("caresses"), "caress");
        IS_EQ(step1aWithData("ponies"), "poni");
        IS_EQ(step1aWithData("ties"), "ti");
        IS_EQ(step1aWithData("caress"), "caress");
        IS_EQ(step1aWithData("cat"), "cat");

        IS_EQ(step1bWithData("feed"), "feed");
        IS_EQ(step1bWithData("agreed"), "agree");
        IS_EQ(step1bWithData("plastered"), "plaster");
        IS_EQ(step1bWithData("bled"), "bled");
        IS_EQ(step1bWithData("motoring"), "motor");
        IS_EQ(step1bWithData("sing"), "sing");
        IS_EQ(step1bWithData("conflated"), "conflate");
        IS_EQ(step1bWithData("troubled"), "trouble");
        IS_EQ(step1bWithData("sized"), "size");
        IS_EQ(step1bWithData("hopping"), "hop");
        IS_EQ(step1bWithData("tanned"), "tan");
        IS_EQ(step1bWithData("falling"), "fall");
        IS_EQ(step1bWithData("hissing"), "hiss");
        IS_EQ(step1bWithData("fizzing"), "fizz");
        IS_EQ(step1bWithData("failing"), "fail");
        IS_EQ(step1bWithData("filing"), "file");

        IS_EQ(step1cWithData("happy"), "happi");
        IS_EQ(step1cWithData("sky"), "sky");
    }
};

void testPorterStemmer()
{
    TestablePorterStemmer stemmer;
    
    stemmer.testIsConstant();
    stemmer.testGetM();
    stemmer.testContainsVowel();
    stemmer.testDoubleConsonantSuffix();
    stemmer.testEndsCVC();
    stemmer.testStep1();
}

// Runner
int main()
{
    testStringEndsWith();
    testStringToLower();
    testPorterStemmer();
}
