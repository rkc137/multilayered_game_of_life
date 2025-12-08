#include <SFML/Graphics.hpp>

#include "sfml-gui/gui.hpp"
#include "sfml-gui/widgets/button.hpp"
#include "sfml-gui/widgets/label.hpp"
#include "sfml-gui/widgets/checkbox.hpp"
#include "sfml-gui/widgets/listbox.hpp"

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

int main()
{
    Universe maps{std::size_t(config.past_size_ref + 1), {}};
    MapsInOrder maps_in_order{maps.begin(), maps.end()};
    setup(maps, full_frame);

    sf::RenderWindow window{sf::VideoMode{window_size}, "game of life raylib" };
    window.setFramerateLimit(target_fps);

    bool is_introverts = true;
    bool is_pause = false;
    int rule_idx = 0;


    /////   ui setup
    SFGUI::SFMLGUI gui(window, "configuration");
    SFGUI::Label size_label{""};
    auto update_size_label = [&](){
        size_label.setText("past size: " + std::to_string(config.past_size_ref));
    };
    update_size_label();
    auto add_past_size = [&](int delta){
        //for now its just reassign
        int psize = config.past_size_ref + delta;
        if(psize < 1 || psize > 1024)
            return;
        config.set_past_size(psize);
        maps.resize(psize + 1); // TODO: WTF
        maps_in_order.assign(maps.begin(), maps.end());
        setup(maps, full_frame);
        update_size_label();
    };
    SFGUI::Button button_past_incr("past size + 1", [&](){ add_past_size(1); });
    SFGUI::Button button_past_dicr("past size - 1", [&](){ add_past_size(-1); });
    SFGUI::Listbox listbox(
        "Rule",
        0,
        std::invoke([&](){
            std::vector<std::string> variants;
            for(size_t i = 0; i < Rules::extraverts.size(); i++)
                variants.emplace_back("extravert " + std::to_string(i));
            for(size_t i = 0; i < Rules::introverts.size(); i++)
                variants.emplace_back("introvert " + std::to_string(i));
            return variants;
        }),
        [&](int indx){
            is_introverts = indx + 1 > static_cast<int>(Rules::extraverts.size());
            rule_idx = indx - is_introverts * Rules::extraverts.size();
        }
    );
    SFGUI::Checkbox checkbox("Pause", &is_pause);
    // gui.Add(checkbox);
    gui.Add(size_label);
    gui.Add(button_past_incr);
    gui.Add(button_past_dicr);
    gui.Add(listbox);

    auto update_title = [&](){
        window.setTitle(
            "past size: " + std::to_string(config.past_size_ref) +
            std::string(is_introverts ? "  introvert" : "  extravert") +
            " rule: " + std::to_string(rule_idx));
    };
    for(unsigned int turn = 0; window.isOpen(); turn++)
    {
        while(const std::optional event = window.pollEvent())
        {
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
            else if(const auto* k = event->getIf<sf::Event::KeyPressed>())
            {
                using namespace sf::Keyboard;

                auto is_ctrl_down = isKeyPressed(Key::LControl) || isKeyPressed(Key::RControl);

                switch(k->code)
                {
                case Key::U:
                    if(++config.premitiva == prims.end())
                        config.premitiva = prims.begin();
                break;
                case Key::R:
                    setup(maps, (is_ctrl_down ? little_frame : full_frame));
                break;
                case Key::C:
                    clear(maps, (is_ctrl_down ? little_frame : full_frame));
                break;
                case Key::P:
                    if(is_ctrl_down)
                    {
                        static bool idop = false;
                        idop = !idop;
                        config.set_drawing_only_present(idop);   
                    }
                    else
                    {
                        config.draw_mode = static_cast<Config::DrawMode>(
                            (static_cast<int>(config.draw_mode) + 1) % Config::DrawModes_size
                        );
                    }
                break;
                case Key::Grave:
                {
                    
                }
                break;
                default:
                }
            }
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
        
        gui.Update();
        gui.Draw();
        // if (button.isClicked())
        //     label.setText("Just clicked that button!"); 

        window.display();
    }

    return 0;
}