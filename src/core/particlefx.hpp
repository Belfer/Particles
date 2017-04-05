#pragma once

#include <SFML/Graphics.hpp>
#include "cereal.hpp"

struct Particle
{
    sf::Color color;
    sf::Vector2f velocity;
    sf::Vector2f position;
    float size;
    float rotation;
    float lifetime;
    float life;
};

class ParticleEmitter : public sf::Drawable, public sf::Transformable
{
public:
    ParticleEmitter(size_t count = 100);
    ~ParticleEmitter();

    size_t count;

    sf::Vector2f emitter;
    sf::Vector2f offset;
    sf::Vector2f force;
    float torque;

    sf::BlendMode blendMode;
    sf::Color startColor;
    sf::Color endColor;

    float minLife;
    float maxLife;

    float minAngle;
    float maxAngle;

    float minSpeed;
    float maxSpeed;

    float minTorque;
    float maxTorque;

    float minSize;
    float maxSize;

    void resize(size_t count);

    void resetAll();

    void setTexture(sf::Texture* texture);

    void update(const sf::Time& elapsed);

    inline size_t getCount() const { return particles.size(); }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void updateVertices(size_t index);

    void resetParticle(size_t index);

    std::vector<Particle> particles;
    sf::VertexArray vertices;
    sf::Texture* texture;

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(count);
        ar(emitter.x); ar(emitter.y);
        ar(offset.x); ar(offset.y);
        ar(force.x); ar(force.y);
        ar(torque);
        ar(blendMode.colorSrcFactor);
        ar(blendMode.colorDstFactor);
        ar(blendMode.colorEquation);
        ar(blendMode.alphaSrcFactor);
        ar(blendMode.alphaDstFactor);
        ar(blendMode.alphaEquation);
        ar(startColor.r); ar(startColor.g); ar(startColor.b);
        ar(endColor.r); ar(endColor.g); ar(endColor.b);
        ar(minLife);
        ar(maxLife);
        ar(minAngle);
        ar(maxAngle);
        ar(minSpeed);
        ar(maxSpeed);
        ar(minTorque);
        ar(maxTorque);
        ar(minSize);
        ar(maxSize);
    }
};
