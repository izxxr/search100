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

#ifndef _SEARCH100_MAIN
#define _SEARCH100_MAIN

#include <iostream>
#include <map>
#include <SFML/Graphics.hpp>
#include "engine.cpp"
#include "utils.cpp"
#include "ui_states.cpp"
#include "ui_components.cpp"

const int FRAMES_PER_SECOND = 60;


void loadFont(const std::string filename, const std::string name, AppData &data)
{
    sf::Font font;
    font.loadFromFile(filename);
    data.fonts[name] = font;
}


int main()
{
    // Window initialization
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Search100");
    window.setFramerateLimit(FRAMES_PER_SECOND);

    // States
    State* state = new StateHome();
    SearchEngine engine("corpus/");
    AppData data(engine);

    // Loading Assets
    sf::Image icon;
    loadFont("assets/font_poppins.ttf", "Poppins", data);
    loadFont("assets/font_roboto.ttf", "Roboto", data);
    icon.loadFromFile("assets/img_icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Common components
    StatusBar status_bar;
    status_bar.text.setString("Initializing...");

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized)
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            else
                state->processEvent(event, window, state, data);
        }

        if (!engine.getIndexSize())
            status_bar.text.setString("No documents are available to search. Add text files to corpus directory and reindex documents to start searching.");
        else
            status_bar.text.setString("Ready | " + std::to_string(engine.getIndexSize()) + " documents");

        if (!data.indexes_loaded)
            status_bar.text.setString("Preparing indexes...");

        state->draw(window, state, data);
        status_bar.draw(window, state, data);
        window.display();

        if (!data.indexes_loaded)
        {
            engine.indexCorpusDirectory(data.indexes_use_data);
            data.indexes_loaded = true;
            data.indexes_use_data = false;
        }
        if (data.state_reset)
        {
            delete state;
            state = new StateHome();
            data.state_reset = false;
        }
    }

    delete state;

    return 0;
}

#endif
