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

    std::string query;

    while (true)
    {
        log("", "QUERY", true, 0, false);
        getline(cin, query);

        auto results = engine.search(query);
        
        for (auto &result : results)
        {
            auto path = engine.getDocumentPath(result.document_id);
            
            log("In document " + path.string(), "", false);
            
            for (auto &occurence : result.occurrences)
            {
                std::string line = to_string(occurence.line + 1);
                std::string index = to_string(occurence.index + 1);
                std::string msg = "At line " + line + ", column " + index + ", found '" + occurence.original + "'";
                log(msg, "", false, 1);
            }
        }
    }

    return 0;
}
