/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#ifndef _SEARCH100_UI_STATES
#define _SEARCH100_UI_STATES

#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include "ui_utils.cpp"
#include "engine.cpp"

/**
 * @brief Class to store application data.
 * 
 * Passed to State.draw() to propagate stateful data.
 */
class AppData
{
    public:

    std::map<std::string, sf::Font> fonts;
    SearchEngine &engine;

    AppData(SearchEngine &engine_inst) : engine(engine_inst) {}
};


/**
 * @brief Base class for all states.
 * 
 * A state refers to a specific GUI layout or view. These state classes make
 * the management of different views of program easy to manage.
 */
class State {
    public:

    /**
     * @brief Draws this state.
     * 
     * This method is called by the main program and can be used
     * to update the state view.
     * 
     * @param window: The SFML window instance.
     * @param state: Reference variable for state. This can be used to change
     * the state.
     * @param data: The application data.
     */
    virtual void draw(sf::RenderWindow &window, State &state, AppData &data) = 0;
};


/**
 * @brief State describing the title page of Search100.
 */
class StateHome: public State
{
    public:

    void draw(sf::RenderWindow &window, State &state, AppData &data)
    {
        auto title = sf::Text("Search100", data.fonts["Poppins"]);
        auto subtitle = sf::Text("Simple, yet fast.", data.fonts["Poppins"]);
        auto win_size = window.getSize();

        title.setCharacterSize(100);
        subtitle.setCharacterSize(30);

        centerText(win_size, title, true, false, 0u, 50u);
        centerText(win_size, subtitle, true, false, 0u, 200u);

        title.setFillColor(sf::Color::Black);
        title.setStyle(sf::Text::Bold);

        subtitle.setFillColor(sf::Color::Black);
        subtitle.setStyle(sf::Text::Italic);

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(subtitle);
    }
};


#endif
