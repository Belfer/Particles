#include "app.hpp"

/// App

#include "imgui.h"
#include "imgui-SFML.h"
#include "alloc.hpp"
#include "particlefx.hpp"
#include "particle_editor.hpp"
#include "vec2.hpp"

#define MAX_PARTICLES 32

#define P_RADIUS 10
#define P_SQRADIUS P_RADIUS*P_RADIUS

LinearAllocator* allocator;
std::vector<ParticleEmitter*> particles;
size_t active = 0;

ParticleEditor editor;
sf::CircleShape circle;

bool dragging = false;

bool App::init(const std::string& respath)
{
    const size_t allocSize = sizeof(ParticleEmitter) * MAX_PARTICLES;
    allocator = new LinearAllocator(allocSize, malloc(allocSize));

    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));
    particles.emplace_back(mem::New<ParticleEmitter>(*allocator));

    editor.setup(respath, "/textures/particle.png");

    circle.setRadius(P_RADIUS);
    circle.setOutlineThickness(1.f);
    circle.setOutlineColor(sf::Color::Green);
    circle.setFillColor(sf::Color::Transparent);

    reset();
    return true;
}

void App::reset()
{
    for(auto p : particles)
        p->resetAll();
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
    if(event.type == sf::Event::MouseButtonPressed) {
        const sf::Vector2f mpos = getWindow().mapPixelToCoords(sf::Mouse::getPosition(getWindow()));
        for(size_t i=0; i<particles.size(); ++i) {
            float sqDist = vec2::magnitudeSq(particles[i]->emitter - mpos);
            if(sqDist < P_SQRADIUS) {
                active = i;
                dragging = true;
            }
        }
    }
    else
    if(event.type == sf::Event::MouseButtonReleased)
        dragging = false;
}

void App::fixed(float t, float dt)
{}

void App::update(const sf::Time& elapsed)
{
    ParticleEmitter* current = particles[active];

    if(dragging) {
        const sf::Vector2f mpos = getWindow().mapPixelToCoords(sf::Mouse::getPosition(getWindow()));
        vec2::lerp(current->emitter, current->emitter, mpos, 0.2f);
    }

    for(auto p : particles)
        p->update(elapsed);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open")) editor.open(*current);
            if(ImGui::MenuItem("Save")) editor.save(*current);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    editor.update(*current, elapsed);
}

void App::pre_draw()
{
}

void App::draw(const sf::View& view)
{
    getWindow().setView(view);
    for(auto p : particles) {
        getWindow().draw(*p);

        circle.setPosition(p->emitter-sf::Vector2f(P_RADIUS, P_RADIUS));
        getWindow().draw(circle);
    }
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
