#pragma once

#include "game.hpp"

class Engine
{
public:
    template<typename T>
    static int start(const std::string& respath)
    {
        static Engine instance;
        return instance.init(new T, respath);
    }

private:
    Engine() {}
    ~Engine() {}
    Engine(const Engine&);
    Engine& operator=(const Engine&);

    int  init(Game* game, const std::string& respath);
    void loop(Game* game, sf::RenderWindow& window);
};
