#include "app.hpp"

/// App

#include "imgui.h"
#include "imgui-SFML.h"
#include "alloc.hpp"
#include "particlefx.hpp"
#include "particle_editor.hpp"
#include "vec2.hpp"

#define NUM_PARTICLES 100

ParticleEditor editor;
ParticleEmitter particles(NUM_PARTICLES);

bool dragging = false;

bool App::init(const std::string& respath)
{
    editor.setup(respath, "/textures/particle.png");
    particles.emitter = views[0].getCenter();

    reset();
    return true;
}

void App::reset()
{
    particles.resetAll();
}

void App::input(const sf::Event& event)
{
    if(event.type == sf::Event::Resized) {
        views[0].setSize(event.size.width, event.size.height);
    }
    else
    if(event.type == sf::Event::KeyPressed) {
        if(event.key.code == sf::Keyboard::R)
            reset();
    }
    else
    if(event.type == sf::Event::MouseButtonPressed)
        dragging = true;
    else
    if(event.type == sf::Event::MouseButtonReleased)
        dragging = false;
}

void App::fixed(float t, float dt)
{}

void App::update(const sf::Time& elapsed)
{
    bool input = editor.update(particles, elapsed);

    if(!input && dragging) {
        const sf::Vector2f mpos = getWindow().mapPixelToCoords(sf::Mouse::getPosition(getWindow()));
        vec2::lerp(particles.emitter, particles.emitter, mpos, 0.2f);
    }
    particles.update(elapsed);
}

void App::pre_draw()
{
}

void App::draw(const sf::View& view)
{
    getWindow().setView(view);
    getWindow().draw(particles);
}

void App::post_draw()
{}

void App::clean()
{
}

/// Main
#include "core/engine.hpp"

int main(int argc, char ** args)
{
    if (argc < 2) {
        std::cout << "Please specify a res path!\n";
        return EXIT_FAILURE;
    }

    return Engine::start<App>(args[1]);
}
