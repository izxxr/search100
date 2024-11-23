/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#ifndef _SEARCH100_UI_UTILS
#define _SEARCH100_UI_UTILS

#include <cmath>
#include <SFML/Graphics.hpp>

/**
 * @brief Centers a text.
 * 
 * @param window: The vector for SFML window or parent.
 * @param text: The text object to center.
 * @param x: Whether to center horizontally.
 * @param y: Whether to center vertically.
 * @param inc_x: Increment in x-coordinate after centering.
 * @param inc_y: Increment in y-coordinate after centering.
 * 
 */
template<typename _T>
void centerText(
    const sf::Vector2<_T> &vec,
    sf::Text &text,
    bool x = true,
    bool y = true,
    _T inc_x = 0,
    _T inc_y = 0
)
{
    auto center = text.getGlobalBounds().getSize() / 2.f;
    auto local_bounds = center + text.getLocalBounds().getPosition();
    auto rounded = sf::Vector2f(std::round(local_bounds.x), std::round(local_bounds.y));

    float x_pos = local_bounds.x;
    float y_pos = local_bounds.y;

    if (x)
        x_pos = vec.x / 2;
    if (y)
        y_pos = vec.y / 2;

    text.setOrigin(rounded);
    text.setPosition(sf::Vector2f{x_pos + inc_x, y_pos + inc_y});
}


/**
 * @brief Centers a shape.
 * 
 * @param window: The SFML window.
 * @param shape: The shape object to center.
 * @param x: Whether to center horizontally.
 * @param y: Whether to center vertically.
 * @param inc_x: Increment in x-coordinate after centering.
 * @param inc_y: Increment in y-coordinate after centering.
 * 
 */
template<typename _T>
void centerShape(
    const sf::Vector2<_T> &vec,
    sf::Shape &shape,
    bool x = true,
    bool y = true,
    _T inc_x = 0,
    _T inc_y = 0
)
{
    auto center = shape.getGlobalBounds().getSize() / 2.f;
    auto local_bounds = center + shape.getLocalBounds().getPosition();
    auto rounded = sf::Vector2f(std::round(local_bounds.x), std::round(local_bounds.y));

    float x_pos = local_bounds.x;
    float y_pos = local_bounds.y;

    if (x)
        x_pos = vec.x / 2;
    if (y)
        y_pos = vec.y / 2;

    shape.setOrigin(rounded);
    shape.setPosition(sf::Vector2f{x_pos + inc_x, y_pos + inc_y});
}


#endif
