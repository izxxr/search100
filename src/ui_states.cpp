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

#include "ui_components.cpp"

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
    virtual void draw(sf::RenderWindow &window, State*  &state, AppData &data) = 0;


    /**
     * @brief Handle an event.
     * 
     * This method is called by the main program when an event
     * occurs other than close or resize.
     * 
     * @param event: The event that occured.
     * @param window: The SFML window instance.
     * @param state: Reference variable for state.
     * @param data: The application data.
     */
    virtual void processEvent(sf::Event event, sf::RenderWindow &window, State*  &state, AppData &data) = 0;
};


/**
 * @brief The search result state.
 */
class StateSearch: public State
{
    public:

    /**
     * @brief The search query whose results will be displayed.
     */
    std::string query;

    StateSearch (std::string search_value)
    {
        query = search_value;
    }

    void processEvent(sf::Event event, sf::RenderWindow &window, State*  &state, AppData &data) { }

    void draw(sf::RenderWindow &window, State*  &state, AppData &data)
    {
        window.clear(sf::Color::White);
    }
};


/**
 * @brief State describing the title page of Search100.
 */
class StateHome: public State
{
    public:

    SearchBar searchbar;

    void processEvent(sf::Event event, sf::RenderWindow &window, State*  &state, AppData &data)
    {
        if (event.type == sf::Event::TextEntered)
            searchbar.addChar(event.text.unicode);
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Left)
                searchbar.moveCursorLeft();
            else if (event.key.code == sf::Keyboard::Right)
                searchbar.moveCursorRight();     
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            auto search_button = searchbar.search_button.getGlobalBounds();
            auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));

            if (search_button.contains(mouse))
                searchbar.search_button_hovered = true;
            else
                searchbar.search_button_hovered = false;
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            auto search_button = searchbar.search_button.getGlobalBounds();
            auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));

            if (search_button.contains(mouse))
            {
                delete state;
                state = new StateSearch(searchbar.value);
            }
        }
    }

    void draw(sf::RenderWindow &window, State*  &state, AppData &data)
    {
        auto title = sf::Text("Search100", data.fonts["Poppins"]);
        auto subtitle = sf::Text("Simple yet fast search engine", data.fonts["Poppins"]);
        auto win_size = window.getSize();

        title.setCharacterSize(100);
        subtitle.setCharacterSize(25);

        centerText(win_size, title, true, false, 0u, 100u);
        centerText(win_size, subtitle, true, false, 0u, 250u);

        title.setFillColor(sf::Color::Black);
        title.setStyle(sf::Text::Bold);

        subtitle.setFillColor(sf::Color::Black);
        subtitle.setStyle(sf::Text::Italic);

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(subtitle);
        searchbar.draw(window, data);
    }
};

#endif
