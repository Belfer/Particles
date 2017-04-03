#pragma once

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>

#include "constants.hpp"

struct HSL
{
    double hue;
    double saturation;
    double luminance;

    HSL() : hue(0), saturation(0), luminance(0) {}
    HSL(int h, int s, int l)
    {
        // Range control for Hue.
        if (h <= 360 && h >= 0) { hue = h; }
        else
        {
            if (h > 360) { hue = h%360; }
            else if (h < 0 && h > -360) { hue = -h; }
            else if (h < 0 && h < -360) { hue = -(h%360); }
        }

        // Range control for Saturation.
        if (s <= 100 && s >= 0) { saturation = s; }
        else
        {
            if (s > 100) { saturation = s%100;}
            else if (s < 0 && s > -100) { saturation = -s; }
            else if (s < 0 && s < -100) { saturation = -(s%100); }
        }

        // Range control for Luminance
        if (l <= 100 && l >= 0) { luminance = l; }
        else
        {
            if (l > 100) { luminance = l%100;}
            if (l < 0 && l > -100) { luminance = -l; }
            if (l < 0 && l < -100) { luminance = -(l%100); }
        }
    }

    sf::Color turnToRGB()
    {
        // Reconvert to range [0,1]
        double h = hue/360.0;
        double s = saturation/100.0;
        double l = luminance/100.0;

        float arg1, arg2;

        if (s <= EPSILON)
        {
            sf::Color C(l*255, l*255, l*255);
            return C;
        }
        else
        {
            if ( l < 0.5 ) { arg2 = l * ( 1 + s ); }
            else { arg2 = ( l + s ) - ( s * l ); }
            arg1 = 2 * l - arg2;

            sf::Uint8 r = ( 255 * hueToRGB( arg1, arg2, (h + 1.0/3.0 ) ) );
            sf::Uint8 g = ( 255 * hueToRGB( arg1, arg2, h ) );
            sf::Uint8 b = ( 255 * hueToRGB( arg1, arg2, (h - 1.0/3.0 ) ) );
            sf::Color c(r,g,b);
            return c;
        }
    }

private:
    double hueToRGB(double arg1, double arg2, double h)
    {
       if ( h < 0 ) h += 1;
       if ( h > 1 ) h -= 1;
       if ( ( 6 * h ) < 1 ) { return (arg1 + ( arg2 - arg1 ) * 6 * h); }
       if ( ( 2 * h ) < 1 ) { return arg2; }
       if ( ( 3 * h ) < 2 ) { return ( arg1 + ( arg2 - arg1 ) * ( ( 2.0 / 3.0 ) - h ) * 6 ); }
       return arg1;
    }
};

HSL turnToHSL(const sf::Color& c)
{
    // Trivial cases.
    if (c == sf::Color::White)
    { return HSL(0, 0, 100); }

    if (c == sf::Color::Black)
    { return HSL(0, 0, 0); }

    if (c == sf::Color::Red)
    { return HSL(0, 100, 50); }

    if (c == sf::Color::Yellow)
    { return HSL(60, 100, 50); }

    if (c == sf::Color::Green)
    { return HSL(120, 100, 50); }

    if (c == sf::Color::Cyan)
    { return HSL(180, 100, 50); }

    if (c == sf::Color::Blue)
    { return HSL(240, 100, 50); }

    if (c == sf::Color::Cyan)
    { return HSL(300, 100, 50); }

    double r, g, b;
    r = c.r/255.0;
    g = c.g/255.0;
    b = c.b/255.0;
    // Casos no triviales.
    double max, min, l, s;

    // Maximos
    max = std::max(std::max(r,g),b);

    // Minimos
    min = std::min(std::min(r,g),b);

    HSL A;
    l = ((max + min)/2.0);

    if (max - min <= EPSILON )
    {
        A.hue = 0;
        A.saturation = 0;
    }
    else
    {
        double diff = max - min;

        if(A.luminance < 0.5)
            s = diff/(max + min);
        else
            s = diff/(2 - max - min);

        double diffR = ( (( max - r ) * 60) + (diff/2.0) ) / diff;
        double diffG = ( (( max - g ) * 60) + (diff/2.0) ) / diff;
        double diffB = ( (( max - b ) * 60) + (diff/2.0) ) / diff;


        if (max - r <= EPSILON) { A.hue = diffB - diffG; }
        else if (max - g <= EPSILON) { A.hue = (1*360)/3.0 + (diffR - diffB); }
        else if (max - b <= EPSILON) { A.hue = (2*360)/3.0 + (diffG - diffR); }

        if (A.hue <= 0 || A.hue >= 360) { fmod(A.hue, 360); }

        s *= 100;
    }

    l *= 100;
    A.saturation = s;
    A.luminance = l;
    return A;
}
