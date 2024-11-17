/**
 *  Search100
 * 
 * A simple yet fast text files based search engine written in C++
 * 
 * This application was developed as the project for CS 100 course in
 * Fall 2024 semester.
 * 
 * Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025
 * 
 * */

#include <iostream>
#include <utils.cpp>
#include <engine.cpp>

using namespace std;

int main()
{    
    log("Welcome to Search100 - a simple yet fast search engine", "");

    SearchEngine engine("corpus/");
    engine.indexCorpusDirectory();

    return 0;
}
