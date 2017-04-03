#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>

#include "config.hpp"

struct Game
{
public:
    Game() {}
    ~Game() {}

private:
    Game(const Game&);
    Game& operator=(const Game&);

    // Game window
    sf::RenderWindow* window;

    friend class Engine;
    inline void config(sf::RenderWindow* window) { this->window = window; }

protected:
    // Once at start of program
    virtual bool init(const std::string& respath) = 0;

	// Called when game should reset
    virtual void reset() = 0;

    // Every poll of input
    virtual void input(const sf::Event& event) = 0;

#if PROC_FIXED
    // At fixed steps
    virtual void fixed(float t, float dt) = 0;
#endif

    // Every update tick
    virtual void update(const sf::Time& elapsed) = 0;

#if PROC_PRE_DRAW
    // Once before scene is rendered
    virtual void pre_draw() = 0;
#endif

    // Every draw call for all views
    virtual void draw(const sf::View& view) = 0;

#if PROC_POST_DRAW
    // Once after scene is rendered
    virtual void post_draw() = 0;
#endif

    // Once at end of program
    virtual void clean() = 0;

    // Returns game window
    inline sf::RenderWindow& getWindow() { return *window; }

    // Scene views/cameras
    std::vector<sf::View> views;
};
