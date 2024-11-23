/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#ifndef _SEARCH100_UI_COMPONENTS
#define _SEARCH100_UI_COMPONENTS

#include <SFML/Graphics.hpp>
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
    virtual void draw(sf::RenderWindow &window, AppData &data) = 0;
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

    void draw(sf::RenderWindow &window, AppData &data)
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

#endif
