#pragma once

#include <bitset>
#include <array>
#include <vector>
#include <list>

#include <SFML/Graphics.hpp>
using vec2i = sf::Vector2i;
using vec2u = sf::Vector2u;
using vec2f = sf::Vector2f;
using Color = sf::Color;

constexpr double target_fps = 40;
constexpr auto frame_duration = std::chrono::duration<double>(1.0 / target_fps);
constexpr int rect_size = 50;
constexpr vec2u window_size = {1800, 900};
constexpr int X = window_size.x / rect_size;
constexpr int Y = window_size.y / rect_size;
constexpr int howmh_frames_skip = 0;
constexpr int born_chance = 2;
constexpr auto shadow_color = Color{0, 0, 0, 10};
const auto dead_color = Color::Black;
constexpr std::array prims = {
    sf::PrimitiveType::Points,        //!< List of individual points
    sf::PrimitiveType::Lines,         //!< List of individual lines
    sf::PrimitiveType::LineStrip,     //!< List of connected lines, a point uses the previous point to form a line
    sf::PrimitiveType::Triangles,     //!< List of individual triangles
    sf::PrimitiveType::TriangleStrip, //!< List of connected triangles, a point uses the two previous points to form a triangle
    sf::PrimitiveType::TriangleFan  
};
enum class DrawMode { normal, rainbow, rainbow_porridge };
constexpr std::array draw_modes = {
    DrawMode::normal,
    DrawMode::rainbow,
    DrawMode::rainbow_porridge
};

using Map = std::array<std::bitset<X + 2>, Y + 2>;
using Universe = std::vector<Map>;
using MapsInOrder = std::list<std::reference_wrapper<Map>>;
using Frame = std::pair<vec2i, vec2i>;

constexpr Frame full_frame = {
    {1, 1},
    {X, Y}
};

constexpr Frame little_frame = {
    {   X / 2 - Y / 10,
        Y / 2 - Y / 10},
    {   X / 2 + Y / 10,
        Y / 2 + Y / 10}
};

struct State
{
public:
    [[nodiscard]] uint8_t get_alive_color_alpha() 
    {
        return static_cast<uint8_t>(255 / (is_present_draw ? 1 : (past_size + 1)));
    }

    decltype(prims.begin()) premitiva = prims.begin() + static_cast<int>(sf::PrimitiveType::TriangleStrip);
    decltype(draw_modes.begin()) draw_mode = draw_modes.begin();
    int past_size = 1;
    bool is_present_draw = true;
    Color alive_color = Color::Red;
} state;
