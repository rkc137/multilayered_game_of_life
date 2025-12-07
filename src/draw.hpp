#pragma once

#include <SFML/Graphics.hpp>

#include "config.hpp"
#include <iostream>

void draw(sf::RenderWindow &window, const MapsInOrder &maps)
{
    sf::Color rainbow_color;
    if(config.draw_mode == Config::DrawMode::normal)
    {
        window.clear(dead_color);
    }
    else
    {
        //you cant make shadow with clear, cause when clean calls, its CLEAR it all
        static auto shadow = std::invoke([&](){
            sf::RectangleShape rs{static_cast<sf::Vector2f>(window_size)};
            rs.setFillColor(shadow_color);
            return rs;
        });
        window.draw(shadow);
        
        if(config.draw_mode == Config::DrawMode::rainbow)
            rainbow_color = Color{
                static_cast<uint8_t>(rand() % 255),
                static_cast<uint8_t>(rand() % 255),
                static_cast<uint8_t>(rand() % 255),
            255};
        // else if constexpr (draw_mode == DrawMode::rainbow_porridge);
    }

    static auto squares = std::invoke([&](){
        // static constexpr auto wsize = static_cast<sf::Vector2f>(window_size);
        static struct {
            // sf::Vertex head{
            //     .position = {wsize.x / 2, wsize.y / 2},
            //     .color = config.get_alive_color()
            // };
            using Square = std::array<sf::Vertex, 4>;
            std::array<std::array<Square, X>, Y> vertexes;
        } squares;
        static constexpr auto &vertexes = squares.vertexes;
        // static constexpr auto R = std::min(wsize.x, wsize.y) / 2;
        // sf::Angle angle = sf::degrees(static_cast<float>(x + X * y) / size * 360.f);
        // R * cos(angle.asRadians()) + wsize.x / 2,
        // R * sin(angle.asRadians()) + wsize.y / 2
        // static constexpr float size = vertexes.size() * vertexes.begin()->size();
        for(int y = 0; y < Y; y++)
        for(int x = 0; x < X; x++)
        {
            float rx = x * rect_size;
            float ry = y * rect_size;
            auto &v = vertexes[y][x];
            v[0].position = {rx, ry};
            v[1].position = {rx + rect_size, ry};
            v[2].position = {rx, ry + rect_size};
            v[3].position = {rx + rect_size, ry + rect_size};
        }
        return squares;
    });

    auto alive_color = config.get_alive_color();
    for(int y = 0; y < Y; y++)
    for(int x = 0; x < X; x++)
    {
        auto color = alive_color;
        uint8_t lives = 0;
        for(auto &map : maps)
            lives += map.get() [y][x];
        color.a = config.get_alive_color_alpha() * lives;
        if(config.draw_mode == Config::DrawMode::normal)
            ;
        else if(config.draw_mode == Config::DrawMode::rainbow)
            color = rainbow_color;
        else if(config.draw_mode == Config::DrawMode::rainbow_porridge)
            color = {
                static_cast<uint8_t>(rand() % 255),
                static_cast<uint8_t>(rand() % 255),
                static_cast<uint8_t>(rand() % 255),
                255};
        for(auto &v : squares.vertexes[y][x])
            v.color = color;

        // if(map[y + 1][x + 1])
        //     ctx.draw_rect(
        //         {rect_size * x, rect_size * y},
        //         {rect_size,     rect_size},
        //         [&]() -> rayplus::Color {
        //             if(config.draw_mode == Config::DrawMode::normal)
        //                 return config.get_alive_color();
        //             else if(config.draw_mode == Config::DrawMode::rainbow)
        //                 return rainbow_color;
        //             else if(config.draw_mode == Config::DrawMode::rainbow_porridge)
        //                 return {rand() % 255, rand() % 255, rand() % 255, 255};
                    
        //             throw std::runtime_error("the behavior for this rule (if its rule) is undefined");
        //             return {};
        //         }()
        //     );
    }
    // if(config.is_drawing_only_present())
    //     draw_map(maps.back().get());
    // else
    //     for(auto &map_ref : maps)
    //         draw_map(map_ref.get());
    window.draw(
        reinterpret_cast<const sf::Vertex*>(&squares),
        squares.vertexes.size() * squares.vertexes.begin()->size() * squares.vertexes.begin()->begin()->size(),
        *config.premitiva
    );
}
