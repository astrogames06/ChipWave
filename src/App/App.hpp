#ifndef APP_H
#define APP_H

#include <vector>
#include <raylib.h>

#include "../Instrument.hpp"
#include "../Note.hpp"

class App
{
public:
    const int WIDTH = 800;
    const int HEIGHT = 450;

    std::vector<Note> notes;
    std::vector<Instrument> instruments;
    std::vector<std::string> sound_icons = {
        "#124#",
        "#0#",
        "#147#",
        "#220#",
        "#222#",
        "#223#",
        "#152#",
        "#221#"
    };
    Camera2D camera = { 0 };
    Vector2 mouse_pos;
    FilePathList sound_files;
    int tempo = 30;
    const int CUBE_SIZE = 50;

    bool playing = false;
    Rectangle playing_line = {50, 50, 5, HEIGHT-50};
    int line_delay = 0;

    void Init();
    void Update();
    void Draw();
    void Reset()
    {
        this->line_delay = 0;
        this->playing = !playing;
        for (Note& n : this->notes) n.played = false;
        this->playing_line.x = this->CUBE_SIZE;
    }
};
extern App app;

#endif