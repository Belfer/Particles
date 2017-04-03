#include "particlefx.hpp"
#include "vec2.hpp"
#include "hsl.hpp"

ParticleEmitter::ParticleEmitter(size_t count)
    : emitter(0, 0),
      offset(0, 0),
      force(0, -3),
      torque(0),
      blendMode(sf::BlendAdd),
      startColor(sf::Color::Red),
      endColor(sf::Color::Yellow),
      minLife(0.1f),
      maxLife(1.0f),
      minAngle(225),
      maxAngle(315),
      minSpeed(30),
      maxSpeed(60),
      minSize(5),
      maxSize(20),
      particles(count),
      vertices(sf::Quads, count*4),
      texture(nullptr)
{
}
ParticleEmitter::~ParticleEmitter() {}

void ParticleEmitter::resetAll()
{
    for(size_t i=0; i<particles.size(); ++i)
        resetParticle(i);
}

void ParticleEmitter::setTexture(sf::Texture* texture)
{
    this->texture = texture;
    const float tw = texture->getSize().x;
    const float th = texture->getSize().y;

    size_t vidx;
    for(size_t i=0; i<particles.size(); ++i) {
        vidx = i*4;
        vertices[vidx+0].texCoords = sf::Vector2f(0, 0);
        vertices[vidx+1].texCoords = sf::Vector2f(0, th);
        vertices[vidx+2].texCoords = sf::Vector2f(tw, th);
        vertices[vidx+3].texCoords = sf::Vector2f(tw, 0);
    }
}

void ParticleEmitter::update(const sf::Time& elapsed)
{
    for(size_t i=0; i<particles.size(); ++i)
    {
        // update the particle lifetime
        Particle& p = particles[i];
        p.life -= elapsed.asSeconds();

        // if the particle is dead, respawn it
        if (p.life <= 0)
            resetParticle(i);

        p.rotation += torque * elapsed.asSeconds();
        p.velocity += force;
        p.position += p.velocity * elapsed.asSeconds();

        // update the alpha (transparency) of the particle according to its lifetime
        float ratio = p.life / p.lifetime;
        p.color.r = static_cast<sf::Uint8>((1-ratio)*endColor.r + ratio*startColor.r);
        p.color.g = static_cast<sf::Uint8>((1-ratio)*endColor.g + ratio*startColor.g);
        p.color.b = static_cast<sf::Uint8>((1-ratio)*endColor.b + ratio*startColor.b);
        p.color.a = static_cast<sf::Uint8>(ratio * 255);

        // update particle
        updateVertices(i);
    }
}

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.blendMode = blendMode;
    states.transform *= getTransform();
    states.texture = texture;

    target.draw(vertices, states);
}

void ParticleEmitter::updateVertices(size_t index)
{
    Particle& p = particles[index];
    size_t vidx = index*4;

    // update the position and color of vertices
    vertices[vidx+0].position = p.position + vec2::rotate(sf::Vector2f(-p.size,-p.size), p.rotation);
    vertices[vidx+1].position = p.position + vec2::rotate(sf::Vector2f(-p.size, p.size), p.rotation);
    vertices[vidx+2].position = p.position + vec2::rotate(sf::Vector2f( p.size, p.size), p.rotation);
    vertices[vidx+3].position = p.position + vec2::rotate(sf::Vector2f( p.size,-p.size), p.rotation);

    vertices[vidx+0].color = p.color;
    vertices[vidx+1].color = p.color;
    vertices[vidx+2].color = p.color;
    vertices[vidx+3].color = p.color;
}

void ParticleEmitter::resetParticle(size_t index)
{
    // give a random velocity and lifetime to the particle
    float random = ((float)rand()) / (float)RAND_MAX;
    float lifetime = minLife + (random * (maxLife-minLife));

    random = ((float)rand()) / (float)RAND_MAX;
    float speed = minSpeed + (random * (maxSpeed-minSpeed));

    random = ((float)rand()) / (float)RAND_MAX;
    float size = minSize + (random * (maxSize-minSize));

    random = ((float)rand()) / (float)RAND_MAX;
    float angle = (minAngle + (random * (maxAngle-minAngle))) * DEG2RAD;

    Particle& p = particles[index];
    p.color = startColor;
    p.velocity = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);
    p.position = emitter + offset;
    p.size = size;
    p.rotation = 0;
    p.lifetime = lifetime;
    p.life = lifetime;

    updateVertices(index);
}
