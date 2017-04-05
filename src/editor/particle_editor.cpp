#include "particle_editor.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "cereal.hpp"
#include "archives/json.hpp"
#include "types/vector.hpp"

#include "tinyfiledialogs.h"

#include <iostream>
#include <fstream>

bool ParticleEditor::setup(const std::string& respath, const std::string& imgpath)
{
    this->respath = respath;
    this->imgpath = imgpath;

    if(!texture.loadFromFile(respath+imgpath)) return false;

    return true;
}

void ParticleEditor::save(ParticleEmitter& particles)
{
    const char* path = tinyfd_saveFileDialog("Save", "", 0, NULL, NULL);
    if (path) {
        std::ofstream os(path);
        cereal::JSONOutputArchive archive(os);
        archive(imgpath);
        archive(particles);
    }
}

void ParticleEditor::open(ParticleEmitter& particles)
{
    const char* path = tinyfd_openFileDialog("Open", "", 0, NULL, NULL, 0);
    if (path) {
        std::ifstream is(path);
        cereal::JSONInputArchive archive(is);
        archive(imgpath);
        archive(particles);

        particles.resize(particles.count);
        texture.loadFromFile(respath+imgpath);
    }
}

void ParticleEditor::update(ParticleEmitter& particles, const sf::Time& elapsed)
{
    ImGui::Begin("Particle Editor");

    strcpy(s512, imgpath.c_str());
    if (ImGui::InputText("Image file", s512, 512)) {
        imgpath = s512;
    }

    if (ImGui::Button("Load image", ImVec2(100, 20))) {
        texture.loadFromFile(respath+imgpath);
    }

    i1 = particles.count;
    if(ImGui::InputInt("Count", &i1)) {
        particles.count = i1;
    }

    if (ImGui::Button("Reset count", ImVec2(100, 20))) {
        particles.resize(particles.count);
    }

    f2[0] = particles.emitter.x;
    f2[1] = particles.emitter.y;

    if (ImGui::InputFloat2("Emitter", f2)) {
        particles.emitter.x = f2[0];
        particles.emitter.y = f2[1];
    }

    f2[0] = particles.offset.x;
    f2[1] = particles.offset.y;

    if (ImGui::InputFloat2("Offset", f2)) {
        particles.offset.x = f2[0];
        particles.offset.y = f2[1];
    }

    f2[0] = particles.force.x;
    f2[1] = particles.force.y;

    if (ImGui::InputFloat2("Force", f2)) {
        particles.force.x = f2[0];
        particles.force.y = f2[1];
    }

    f1 = particles.torque;
    if (ImGui::InputFloat("Torque", &f1)) {
        particles.torque = f1;
    }

    f2[0] = particles.minLife;
    f2[1] = particles.maxLife;

    if (ImGui::InputFloat2("Life", f2)) {
        particles.minLife = f2[0];
        particles.maxLife = f2[1];
    }

    f2[0] = particles.minAngle;
    f2[1] = particles.maxAngle;

    if (ImGui::InputFloat2("Angle", f2)) {
        particles.minAngle = f2[0];
        particles.maxAngle = f2[1];
    }

    f2[0] = particles.minSpeed;
    f2[1] = particles.maxSpeed;

    if (ImGui::InputFloat2("Speed", f2)) {
        particles.minSpeed = f2[0];
        particles.maxSpeed = f2[1];
    }

    f2[0] = particles.minSize;
    f2[1] = particles.maxSize;

    if (ImGui::InputFloat2("Size", f2)) {
        particles.minSize = f2[0];
        particles.maxSize = f2[1];
    }

    f3[0] = particles.startColor.r / 255.f;
    f3[1] = particles.startColor.g / 255.f;
    f3[2] = particles.startColor.b / 255.f;

    if (ImGui::ColorEdit3("Start Color", f3)) {
        particles.startColor.r = static_cast<sf::Uint8>(f3[0] * 255.f);
        particles.startColor.g = static_cast<sf::Uint8>(f3[1] * 255.f);
        particles.startColor.b = static_cast<sf::Uint8>(f3[2] * 255.f);
    }

    f3[0] = particles.endColor.r / 255.f;
    f3[1] = particles.endColor.g / 255.f;
    f3[2] = particles.endColor.b / 255.f;

    if (ImGui::ColorEdit3("End Color", f3)) {
        particles.endColor.r = static_cast<sf::Uint8>(f3[0] * 255.f);
        particles.endColor.g = static_cast<sf::Uint8>(f3[1] * 255.f);
        particles.endColor.b = static_cast<sf::Uint8>(f3[2] * 255.f);
    }

    if (ImGui::RadioButton("BlendAdd", &i1, 0)) {
        particles.blendMode = sf::BlendAdd;
    }
    if (ImGui::RadioButton("BlendAlpha", &i1, 1)) {
        particles.blendMode = sf::BlendAlpha;
    }
    if (ImGui::RadioButton("BlendMultiply", &i1, 2)) {
        particles.blendMode = sf::BlendMultiply;
    }
    if (ImGui::RadioButton("BlendNone", &i1, 3)) {
        particles.blendMode = sf::BlendNone;
    }

    ImGui::End();

    particles.setTexture(&texture);
}
