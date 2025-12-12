#include <SFML/Graphics.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include <ranges>
#include <random>
#include <thread>
#include <chrono>

#include "config.hpp"
#include "rules.hpp"

#include "draw.hpp"
#include "sim_frame.hpp"

void setup(Universe &maps, Frame frame)
{
    const auto [pos, size] = frame;
    for(auto &map : maps)
        for(int y = pos.y; y < size.y; y++)
        for(int x = pos.x; x < size.x; x++)
            map[y][x] = !(rand() % born_chance);
}

void clear(Universe &maps, Frame frame)
{
    const auto [pos, size] = frame;
    for(auto &map : maps)
        for(int y = pos.y; y < size.y; y++)
        for(int x = pos.x; x < size.x; x++)
            map[y][x] = false;
}

constexpr std::string constexpr_to_string(int v)
{
    // +1 for minus, +1 for digits10
    constexpr size_t bufsize{std::numeric_limits<int>::digits10 + 2};
    char buf[bufsize];
    const auto res = std::to_chars(buf, buf + bufsize , v);
    return std::string(buf, res.ptr);
}

template <size_t N>
constexpr auto arr_names(std::string_view str)
{
    std::array<std::string, N> names;
    for(auto [i, s] : names | std::views::enumerate)
        s = std::string{str}.append(constexpr_to_string(i));
    return names;
}

int main()
{
    Universe maps{std::size_t(state.past_size + 1), {}};
    MapsInOrder maps_in_order{maps.begin(), maps.end()};
    setup(maps, full_frame);

    sf::RenderWindow window{sf::VideoMode{window_size}, "game of life" };
    window.setFramerateLimit(target_fps);
    if(!ImGui::SFML::Init(window))
        throw std::runtime_error("imgui init failure");
    ImGui::GetIO().FontGlobalScale = 1.5f;


    bool is_pause = false;
    bool is_introverts = true;
    size_t rule_idx = 0;

    sf::Clock clock;
    for(unsigned int turn = 0; window.isOpen(); turn++)
    {
        while(const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);
            if(event->is<sf::Event::Closed>()) window.close();
            else if(const auto* resized = event->getIf<sf::Event::Resized>())
            {
                auto size = resized->size;
                static const sf::Vector2u minimal_size = {400, 300};
                size = {
                    size.x < minimal_size.x ? minimal_size.x : size.x,
                    size.y < minimal_size.x ? minimal_size.y : size.y
                };
                window.setSize(size);
                window.setView({
                    static_cast<sf::Vector2f>(size) / 2.f,
                    static_cast<sf::Vector2f>(size)
                });
            }
            
        }

        // ui
        {        
            ImGui::SFML::Update(window, clock.restart());     
            ImGui::Begin("Configuration");
            ImGui::Checkbox("Pause", &is_pause);
            ImGui::Checkbox("Draw only present", &state.is_present_draw);
            if(ImGui::Button("Randomize little frame"))
                setup(maps, little_frame);
            if(ImGui::Button("Randomize all cells"))
                setup(maps, full_frame);
            if(ImGui::Button("clear little frame"))
                clear(maps, little_frame);
            if(ImGui::Button("clear all cells"))
                clear(maps, full_frame);
            ImGui::Text("layers of past: ");
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            
            auto ui_counter = [&](auto f, auto counter)
            {
                float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
                if(ImGui::ArrowButton("##left", ImGuiDir_Left))
                    f(-1);
                ImGui::SameLine(0.0f, spacing);
                ImGui::Text("%d", counter());
                ImGui::SameLine(0.0f, spacing);
                if(ImGui::ArrowButton("##right", ImGuiDir_Right))
                    f(1);
                ImGui::PopItemFlag();
            };
            
            ui_counter([&](int k){
                int new_psize = state.past_size + k;
                if(1 > new_psize || new_psize > 1023) return;
                state.past_size = new_psize;
                maps.resize(new_psize + 1);
                maps_in_order.assign(maps.begin(), maps.end());
                setup(maps, full_frame);
            }, [&]{ return state.past_size; });
            
            if(ImGui::BeginListBox("Rule"))
            {
                for(size_t i = 0; i < Rules::introverts.size(); i++)
                {
                    const bool is_selected = (rule_idx == i) && is_introverts;
                    static auto names = arr_names<Rules::introverts.size()>("Introverts #");
                    if(ImGui::Selectable(names[i].c_str(), is_selected))
                    {
                        rule_idx = i;
                        is_introverts = true;
                    }
                    if(is_selected) ImGui::SetItemDefaultFocus();
                }
                for(size_t i = 0; i < Rules::extraverts.size(); i++)
                {
                    const bool is_selected = (rule_idx == i) && !is_introverts;
                    static auto names = arr_names<Rules::extraverts.size()>("Extraverts #");
                    if(ImGui::Selectable(names[i].c_str(), is_selected))
                    {
                        rule_idx = i;
                        is_introverts = false;
                    }    
                    if(is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
            if(ImGui::BeginListBox("Vertex draw mode"))
            {
                for(auto it = prims.begin(); it != prims.end(); ++it)
                {
                    const bool is_selected = (it == state.premitiva);
                    if(ImGui::Selectable(magic_enum::enum_name(*it).data(), is_selected))
                        state.premitiva = it;
                    if(is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
            if(ImGui::BeginListBox("Drawing mode"))
            {
                for(auto it = draw_modes.begin(); it != draw_modes.end(); ++it)
                {
                    const bool is_selected = (it == state.draw_mode);
                    if(ImGui::Selectable(magic_enum::enum_name(*it).data(), is_selected))
                        state.draw_mode = it;
                    if(is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
            static float color[3] = {
                static_cast<float>(state.alive_color.r) / 255.f,
                static_cast<float>(state.alive_color.g) / 255.f,
                static_cast<float>(state.alive_color.b) / 255.f
            };
            ImGui::ColorPicker3("alive color", color);
            state.alive_color.r = static_cast<uint8_t>(color[0] * 255.f);
            state.alive_color.g = static_cast<uint8_t>(color[1] * 255.f);
            state.alive_color.b = static_cast<uint8_t>(color[2] * 255.f);
            ImGui::End();
        }


        maps_in_order.splice(
            maps_in_order.end(),
            maps_in_order,
            maps_in_order.begin()
        );
        if(!is_pause)
        {
            if(is_introverts)
                sim_frame(maps_in_order, Rules::introverts[rule_idx]);
            else
                sim_frame(maps_in_order, Rules::extraverts[rule_idx]);
        }
        if(!(turn % (howmh_frames_skip + 1)))
            draw(window, maps_in_order);
        
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    
    return 0;
}