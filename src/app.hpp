#pragma once

#include "core/game.hpp"

struct App : public Game
{
protected:
    // Once at start of program
    virtual bool init(const std::string& respath);

	// Called when game should reset
    virtual void reset();

    // Every poll of input
    virtual void input(const sf::Event& event);

    // At fixed steps
    void fixed(float t, float dt);

    // Every update tick
    virtual void update(const sf::Time& elapsed);

    // Once before scene is rendered
    void pre_draw();

    // Every draw call for all views
    virtual void draw(const sf::View& view);

    // Once after scene is rendered
    void post_draw();

    // Once at end of program
    virtual void clean();
};
