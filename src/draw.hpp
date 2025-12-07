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

    static auto ball = std::invoke([&](){
        static constexpr auto wsize = static_cast<sf::Vector2f>(window_size);
        static struct {
            sf::Vertex head{
                .position = {wsize.x / 2, wsize.y / 2},
                .color = config.get_alive_color()
            };
            std::array<std::array<sf::Vertex, X>, Y> vertexes;
        } ball;
        static constexpr auto &vertexes = ball.vertexes;
        static constexpr auto R = std::min(wsize.x, wsize.y) / 2;
        static constexpr float size = vertexes.size() * vertexes.begin()->size();
        for(int y = 0; y < Y; y++)
        for(int x = 0; x < X; x++)
        {
            sf::Angle angle = sf::degrees(static_cast<float>(x + X * y) / size * 360.f);
            vertexes[y][x].position = {
                R * cos(angle.asRadians()) + wsize.x / 2,
                R * sin(angle.asRadians()) + wsize.y / 2
            };
        }
        return ball;
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
            
        ball.vertexes[y][x].color = color;

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
    window.draw(reinterpret_cast<const sf::Vertex*>(&ball), ball.vertexes.begin()->size() * ball.vertexes.size(), *config.premitiva);
}
