#pragma once

#include <SFML/Graphics.hpp>
#include "particlefx.hpp"

struct ParticleEditor
{
    bool setup(const std::string& respath, const std::string& imgpath);

    void update(ParticleEmitter& particles, const sf::Time& elapsed);

    void save(ParticleEmitter& particles);

    void open(ParticleEmitter& particles);

private:
    std::string respath;
    std::string imgpath;
    sf::Texture texture;

    char s512[512];
    char s1024[1024];

    float f1;
    float f2[2];
    float f3[3];

    int i1;
    int i2[2];
    int i3[3];
};
