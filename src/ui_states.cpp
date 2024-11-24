/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#ifndef _SEARCH100_UI_STATES
#define _SEARCH100_UI_STATES

#include <string>
#include <map>
#include <tuple>
#include <SFML/Graphics.hpp>
#include "utils.cpp"
#include "engine.cpp"
#include "ui_utils.cpp"

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
     * @brief Get unique identifier of the state.
     */
    virtual std::string getName() = 0;

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
    virtual void draw(sf::RenderWindow &window, State* &state, AppData &data) = 0;


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
    virtual void processEvent(sf::Event event, sf::RenderWindow &window, State* &state, AppData &data) = 0;
};

#include "ui_components.cpp"  // ui_components require State definition


bool processEventSearchbar(
    sf::Event event,
    sf::RenderWindow &window,
    State* &state,
    AppData &data,
    SearchBar &searchbar
)
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
            return true;
    }

    return false;
}


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

    /**
     * @brief The search bar associated with the state.
     */
    SearchBar searchbar;

    /**
     * @brief The results produced by SearchEngine::search()
     */
    std::vector<SearchResult> results;

    /**
     * @brief Result heading entries outlining document ID.
     */
    std::vector<std::tuple<bool, std::string, sf::Text>> sf_result_headings;

    sf::Text sf_result_text;

    bool search_results_fetched = false;

    StateSearch (std::string search_value)
    {
        query = search_value;
        searchbar.value = query;
        searchbar.cursor_pos = query.length();
    }

    std::string getName()
    {
        return "search";
    }

    void processEvent(sf::Event event, sf::RenderWindow &window, State* &state, AppData &data)
    {
        if (processEventSearchbar(event, window, state, data, searchbar))
        {
            delete state;
            state = new StateSearch(searchbar.value);
            return;
        }

        auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
        if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased)
        {
            for (int i = 0; i < sf_result_headings.size(); ++i)
            {
                auto &tup = sf_result_headings[i];
                auto path = std::get<1>(tup);
                auto &text = std::get<2>(tup);
                auto bounds = text.getGlobalBounds();
                bool hovered = bounds.contains(mouse);

                sf_result_headings[i] = std::make_tuple(hovered, path, text);

                if (hovered && event.type == sf::Event::MouseMoved)
                    break;
                else if (hovered && (event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Left))
                    system(path.c_str());
            }
        }
        if (!searchbar.search_button_hovered && event.type == sf::Event::MouseMoved)
        {
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {

        }
    }

    void drawResults(sf::RenderWindow &window, State* &state, AppData &data)
    {
        int y_entry = 240;
        int dy_entry = 69;
        int index = 0;
        bool empty = sf_result_headings.empty();

        for (auto &entry : results)
        {
            int y_occurrence = 15;
            int dy_occurrence = 40;

            std::filesystem::path path = data.engine.getDocumentPath(entry.document_id);
            std::string document = path.filename().string();
            sf::RectangleShape sf_result_entry(sf::Vector2f(800, entry.occurrences.size() * dy_occurrence + 20));

            sf_result_entry.setFillColor(sf::Color(180, 180, 180, 0.3));
            sf_result_entry.setOutlineColor(sf::Color(190, 190, 190));
            sf_result_entry.setOutlineThickness(2);
            sf_result_entry.setPosition(sf_result_text.getPosition() + sf::Vector2f(40, y_entry));

            sf::Text sf_result_heading;

            if (empty)
            {
                sf_result_heading = sf::Text(document + " (" + std::to_string(entry.occurrences.size()) + ")",
                                             data.fonts["Roboto"], 22);
                sf_result_heading.setFillColor(sf::Color::Blue);
                sf_result_heading.setStyle(sf::Text::Bold);
                sf_result_heading.setPosition(sf_result_entry.getPosition() + sf::Vector2f(0, -40));

                auto path_str = path.string();
                normalizePath(path_str);

                auto tup = std::make_tuple(false, path_str, sf_result_heading);
                sf_result_headings.push_back(tup);
            }
            else
            {
                sf_result_heading = std::get<2>(sf_result_headings[index]);
                bool hovered = std::get<0>(sf_result_headings[index]);
                if (hovered)
                    sf_result_heading.setStyle(sf::Text::Underlined | sf::Text::Bold);
                else
                    sf_result_heading.setStyle(sf::Text::Bold);

                index++;
            }

            y_entry += sf_result_entry.getSize().y + dy_entry;

            for (auto &occurrence : entry.occurrences)
            {
                sf::Text text("Line " + std::to_string(occurrence.line + 1) + ", Column " +
                              std::to_string(occurrence.index + 1) + ": \"" + occurrence.original + "\"",
                              data.fonts["Roboto"], 22);

                text.setPosition(sf_result_entry.getPosition() + sf::Vector2f(20, y_occurrence));
                text.setFillColor(sf::Color::Black);
                window.draw(text);

                y_occurrence += dy_occurrence;
            }

            window.draw(sf_result_entry);
            window.draw(sf_result_heading);
        }
    }

    void draw(sf::RenderWindow &window, State* &state, AppData &data)
    {
        sf::Text sf_result_text("", data.fonts["Roboto"], 24);
        sf_result_text.setFillColor(sf::Color::Black);
        sf_result_text.setPosition(40u, 140u);
        sf_result_text.setStyle(sf::Text::Italic);

        if (!search_results_fetched)
            sf_result_text.setString("Searching...");
        else if (search_results_fetched && !results.size())
            sf_result_text.setString("No results found for \"" + query + "\"");
        else
            sf_result_text.setString("Showing results for \"" + query + "\"");


        window.clear(sf::Color::White);
        window.draw(sf_result_text);
        searchbar.draw(window, state, data);

        if (!search_results_fetched)
        {
            results = data.engine.search(query);
            search_results_fetched = true;
        }

        drawResults(window, state, data);
    }
};


/**
 * @brief State describing the title page of Search100.
 */
class StateHome: public State
{
    public:

    std::string name = "home";

    SearchBar searchbar;

    std::string getName()
    {
        return "home";
    }

    void processEvent(sf::Event event, sf::RenderWindow &window, State* &state, AppData &data)
    {
        if (processEventSearchbar(event, window, state, data, searchbar))
        {
            delete state;
            state = new StateSearch(searchbar.value);
        }
    }

    void draw(sf::RenderWindow &window, State* &state, AppData &data)
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
        searchbar.draw(window, state, data);
    }
};


#endif
