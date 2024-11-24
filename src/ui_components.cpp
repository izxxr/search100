/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#ifndef _SEARCH100_UI_COMPONENTS
#define _SEARCH100_UI_COMPONENTS

#include <fstream>
#include <SFML/Graphics.hpp>
#include "stemming.cpp"
#include "engine.cpp"
#include "ui_states.cpp"
#include "ui_utils.cpp"

/**
 * @brief Base class for all UI components.
 */
class Component {
    public:

    /**
     * @brief Draws this component.
     * 
     * This method is called by the main program and can be used
     * to update the component state.
     * 
     * @param window: The SFML window instance.
     * @param data: The application data.
     */
    virtual void draw(sf::RenderWindow &window, State* &state, AppData &data) = 0;
};


/**
 * @brief The status bar component.
 * 
 * Status bar is a horizontal bar at the bottom of the window that
 * shows status of searching and indexing.
 */
class StatusBar: public Component
{
    public:

    /**
     * @brief The text in the bar.
     */
    sf::Text text;

    void draw(sf::RenderWindow &window, State* &state, AppData &data)
    {
        auto win_size = window.getSize();

        sf::RectangleShape rect(sf::Vector2f(win_size.x, 30));

        rect.setFillColor(sf::Color(237, 237, 237));
        rect.setOutlineColor(sf::Color(190, 190, 190));
        rect.setOutlineThickness(2);
        rect.setPosition(0, win_size.y - 30);

        text.setFont(data.fonts["Roboto"]);
        text.setCharacterSize(16);
        text.setPosition(15, win_size.y - 24);
        text.setStyle(sf::Text::Italic);
        text.setFillColor(sf::Color::Black);

        window.draw(rect);
        window.draw(text);
    }
};


/**
 * @brief The search bar component that accepts input for search query.
 */
class SearchBar: public Component
{
    public:

    bool search_button_hovered = false;

    sf::RectangleShape search_button;

    /**
     * @brief The current value of search bar.
     */
    std::string value = "";

    /**
     * @brief Describes the position of the cursor.
     */
    int cursor_pos = 0;

    /**
     * @brief Moves the cursor one character to the left of text.
     */
    void moveCursorLeft()
    {
        if (cursor_pos == 0)
            return;

        cursor_pos--;
    }

    /**
     * @brief Moves the cursor one character to the left of text.
     */
    void moveCursorRight()
    {
        if (cursor_pos == value.length())
            return;

        cursor_pos++;
    }

    /**
     * @brief Inserts a character to searchbar text.
     * 
     * The value is inserted at current cursor position. If
     * unicode value 8 is entered, it is equivalent to backspace
     * hence a character is removed instead.
     * 
     * If unicode value is 22 (CTRL + V), the content of clipboard
     * is added to value.
     * 
     * @param unicode: The unicode value for character to append.
     */
    void addChar(int unicode)
    {
        if (unicode == 8)
        {
            if (!value.empty() && cursor_pos != 0)
                value.erase(--cursor_pos, 1);
        }
        else if (unicode == 22)
        {
            std::string cb_val = sf::Clipboard::getString();
            value = value.substr(0, cursor_pos) + cb_val + value.substr(cursor_pos);
            cursor_pos += cb_val.length();
        }
        else if (unicode < 128)
            value.insert(cursor_pos++, 1, static_cast<char>(unicode));
    }

    void draw(sf::RenderWindow &window, State* &state, AppData &data)
    {
        auto win_size = window.getSize();
        sf::RectangleShape rect(sf::Vector2f(600, 50));

        rect.setFillColor(sf::Color(237, 237, 237, 0.5));
        rect.setOutlineColor(sf::Color(190, 190, 190));
        rect.setOutlineThickness(2);

        if (state->getName() == "home")
            centerShape(win_size, rect, true, false, 0u, 350u);
        else if (state->getName() == "search")
            rect.setPosition(40u, 40u);

        auto rect_pos = rect.getPosition();

        sf::Text text;
        text.setFont(data.fonts["Roboto"]);
        text.setCharacterSize(20);
        text.setStyle(sf::Text::Italic);
        text.setFillColor(sf::Color::Black);
        text.setString(value.substr(0, cursor_pos) + "|" + value.substr(cursor_pos));
        text.setStyle(sf::Text::Regular);

        if (state->getName() == "home")
            text.setPosition(rect_pos.x - (rect.getSize().x / 2) + 20, 360u);
        else if (state->getName() == "search")
            text.setPosition(50u, 50u);

        search_button = sf::RectangleShape(sf::Vector2f(130, 50));
        if (search_button_hovered)
            search_button.setFillColor(sf::Color(255, 255, 255));
        else
            search_button.setFillColor(sf::Color(0, 0, 0));

        search_button.setOutlineColor(sf::Color(190, 190, 190));
        search_button.setOutlineThickness(2);

        if (state->getName() == "home")
            centerShape(win_size, search_button, true, false, 0u, 450u);
        else if (state->getName() == "search")
            search_button.setPosition(rect.getPosition() + rect.getSize() + sf::Vector2f(40, -50));

        sf::Text search_text("Search", data.fonts["Poppins"], 19);
        if (search_button_hovered)
            search_text.setFillColor(sf::Color::Black);
        else
            search_text.setFillColor(sf::Color::White);

        if (state->getName() == "home")
            centerText(win_size, search_text, true, false, 0u, 465u);
        else if (state->getName() == "search")
            search_text.setPosition(rect.getPosition() + rect.getSize() + sf::Vector2f(70, -35));

        window.draw(rect);
        window.draw(text);
        window.draw(search_button);
        window.draw(search_text);
    }
};

#endif
