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
    bool indexes_loaded = false;
    bool indexes_use_data = true;
    bool state_reset = false;

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
     * @brief Whether to search using the AND search strategy.
     */
    bool search_strategy_and;

    /**
     * @brief The search bar associated with the state.
     */
    SearchBar searchbar;

    /**
     * @brief The results produced by SearchEngine::search()
     */
    std::vector<SearchResult> results;

    /**
     * @brief Back to home button.
     */
    sf::RectangleShape sf_back_home_button;

    /**
     * @brief Result heading entries outlining document ID.
     */
    std::vector<std::tuple<bool, std::string, sf::Text>> sf_result_headings;

    sf::Text sf_result_text;

    bool search_results_fetched = false;
    bool back_home_button_hovered = false;

    StateSearch (std::string search_value, bool search_strategy_and_value)
    {
        query = search_value;
        search_strategy_and = search_strategy_and_value;
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
            state = new StateSearch(searchbar.value, search_strategy_and);
            return;
        }

        if (!searchbar.search_button_hovered && (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased))
        {
            auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
            back_home_button_hovered = sf_back_home_button.getGlobalBounds().contains(mouse);

            if (back_home_button_hovered)
            {
                if ((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Left))
                    data.state_reset = true;

                return;
            }

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

        sf_back_home_button = sf::RectangleShape(sf::Vector2f(120, 50));
        if (back_home_button_hovered)
            sf_back_home_button.setFillColor(sf::Color(220, 220, 220));
        else
            sf_back_home_button.setFillColor(sf::Color(237, 237, 237));

        sf_back_home_button.setOutlineColor(sf::Color(190, 190, 190));
        sf_back_home_button.setOutlineThickness(2);
        sf_back_home_button.setPosition(
            searchbar.search_button.getPosition() + sf::Vector2f(160, 0)
        );

        sf::Text sf_back_home_text("Home", data.fonts["Poppins"], 19);
        sf_back_home_text.setFillColor(sf::Color::Black);
        sf_back_home_text.setPosition(searchbar.search_button.getPosition() + sf::Vector2f(190, 13));

        window.clear(sf::Color::White);
        window.draw(sf_result_text);
        window.draw(sf_back_home_button);
        window.draw(sf_back_home_text);
        searchbar.draw(window, state, data);

        if (!search_results_fetched)
        {
            results = data.engine.search(query, search_strategy_and);
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

    bool search_strategy_and = true;
    bool search_strategy_toggle_hover = false;
    bool reindex_button_hover = false;
    bool open_corpus_dir_hover = false;

    sf::RectangleShape sf_search_strategy_toggle;
    sf::RectangleShape sf_reindex_button;
    sf::RectangleShape sf_open_corpus_dir;

    std::string getName()
    {
        return "home";
    }

    void processEvent(sf::Event event, sf::RenderWindow &window, State* &state, AppData &data)
    {
        if (processEventSearchbar(event, window, state, data, searchbar))
        {
            delete state;
            state = new StateSearch(searchbar.value, search_strategy_and);
        }
        else if (!searchbar.search_button_hovered && (event.type == sf::Event::MouseMoved ||
                (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)))
        {
            auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
            auto toggle_bounds = sf_search_strategy_toggle.getGlobalBounds();
            auto reindex_bounds = sf_reindex_button.getGlobalBounds();
            auto open_corpus_bounds = sf_open_corpus_dir.getGlobalBounds();

            search_strategy_toggle_hover = toggle_bounds.contains(mouse);
            reindex_button_hover = reindex_bounds.contains(mouse);
            open_corpus_dir_hover = open_corpus_bounds.contains(mouse);

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (search_strategy_toggle_hover)
                    search_strategy_and = !search_strategy_and;
                else if (reindex_button_hover)
                {
                    data.indexes_loaded = false;
                    data.indexes_use_data = false;
                }
                else if (open_corpus_dir_hover)
                {
                    std::string path = data.engine.corpus_directory_path.string();
                    normalizePath(path);

                    // This would not work on Linux and unfortunately there are no
                    // solutions either as each linux distro has its own file explorer
                    // having different command e.g. nautilus, nemo, etc.
                    path = "explorer " + path;
                    system(path.c_str());
                }
            }
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

        sf_search_strategy_toggle = sf::RectangleShape(sf::Vector2f(200, 50));
        if (search_strategy_toggle_hover)
            sf_search_strategy_toggle.setFillColor(sf::Color(220, 220, 220));
        else
            sf_search_strategy_toggle.setFillColor(sf::Color(237, 237, 237));

        sf_search_strategy_toggle.setOutlineColor(sf::Color(190, 190, 190));
        sf_search_strategy_toggle.setOutlineThickness(2);

        std::string text = "Search Strategy: ";
        if (search_strategy_and)
            text += "AND";
        else
            text += "OR";

        sf::Text sf_search_strategy_text(text, data.fonts["Roboto"], 19);
        sf_search_strategy_text.setFillColor(sf::Color::Black);

        centerShape(win_size, sf_search_strategy_toggle, true, false, 0u, 550u);
        centerText(win_size, sf_search_strategy_text, true, false, 0u, 565u);

        sf_search_strategy_text.setPosition(sf_search_strategy_text.getPosition() - sf::Vector2f(220, 0));
        sf_search_strategy_toggle.setPosition(sf_search_strategy_toggle.getPosition() - sf::Vector2f(220, 0));

        sf_reindex_button = sf::RectangleShape(sf::Vector2f(200, 50));
        if (reindex_button_hover)
            sf_reindex_button.setFillColor(sf::Color(220, 220, 220));
        else
            sf_reindex_button.setFillColor(sf::Color(237, 237, 237));

        sf_reindex_button.setOutlineColor(sf::Color(190, 190, 190));
        sf_reindex_button.setOutlineThickness(2);

        sf::Text sf_reindex_text("Reindex Documents", data.fonts["Roboto"], 19);
        sf_reindex_text.setFillColor(sf::Color::Black);

        centerShape(win_size, sf_reindex_button, true, false, 0u, 550u);
        centerText(win_size, sf_reindex_text, true, false, 0u, 565u);

        sf_open_corpus_dir = sf::RectangleShape(sf::Vector2f(200, 50));
        if (open_corpus_dir_hover)
            sf_open_corpus_dir.setFillColor(sf::Color(220, 220, 220));
        else
            sf_open_corpus_dir.setFillColor(sf::Color(237, 237, 237));

        sf_open_corpus_dir.setOutlineColor(sf::Color(190, 190, 190));
        sf_open_corpus_dir.setOutlineThickness(2);

        sf::Text sf_open_corpus_text("Corpus Directory", data.fonts["Roboto"], 19);
        sf_open_corpus_text.setFillColor(sf::Color::Black);

        centerShape(win_size, sf_open_corpus_dir, true, false, 0u, 550u);
        centerText(win_size, sf_open_corpus_text, true, false, 0u, 565u);

        sf_open_corpus_text.setPosition(sf_open_corpus_text.getPosition() + sf::Vector2f(220, 0));
        sf_open_corpus_dir.setPosition(sf_open_corpus_dir.getPosition() + sf::Vector2f(220, 0));

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(subtitle);
        searchbar.draw(window, state, data);
        window.draw(sf_search_strategy_toggle);
        window.draw(sf_search_strategy_text);
        window.draw(sf_reindex_button);
        window.draw(sf_reindex_text);
        window.draw(sf_open_corpus_dir);
        window.draw(sf_open_corpus_text);
    }
};


#endif
