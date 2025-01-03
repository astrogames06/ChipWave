#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

struct Note
{
	Sound sound;
	Rectangle rec;
	Color color;
	bool played = false;
};
std::vector<Note> notes;

const int WIDTH = 800;
const int HEIGHT = 450;

bool playing = false;
Rectangle playing_line = {50, 50, 5, HEIGHT-50};
int line_delay = 0;

void UpdateDrawFrame();

Camera2D camera = { 0 };

const int CUBE_SIZE = 50;

Vector2 mouse_pos;

Sound beep;

struct Instrument
{
	Sound sound;
	std::string icon;
	Rectangle row;
	Color row_clr;
};
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

FilePathList sound_files;

size_t getLength(char **array) {
    size_t length = 0;
    while (array[length] != nullptr) {
        length++;
    }
    return length;
}

int tempo = 30;

int main(void)
{
	InitWindow(WIDTH+50, HEIGHT, "raylib [core] example - basic window");
	InitAudioDevice();
	GuiLoadIcons("iconset.rgi", true);

	sound_files = LoadDirectoryFiles("sounds");

	for (int i = 0; i < sound_files.count; i++)
	{
		// Wave w = LoadWave(sound_files.paths[i]);
		// std::string p = std::string(sound_files.paths[i]) + ".h";
		// ExportWaveAsCode(w, p.c_str());
		Instrument instrument = {LoadSound(sound_files.paths[i]), sound_icons[i].c_str()};
		instrument.row_clr = {
			(unsigned char) GetRandomValue(0, 255),
			(unsigned char) GetRandomValue(0, 255),
			(unsigned char) GetRandomValue(0, 255),
			255
		};
		instruments.push_back(instrument);
	}
	

	camera.zoom = 1.f;

	#if defined(PLATFORM_WEB)
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
	#else
		//SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
		//--------------------------------------------------------------------------------------

		// Main game loop
		while (!WindowShouldClose())    // Detect window close button or ESC key
		{
			UpdateDrawFrame();
		}
	#endif

	CloseWindow();

	return 0;
}

float mapValue(float mouse_pos_y, float min_value = 0.1f, float max_value = 2.0f) {
    float clamped_y = std::clamp(mouse_pos_y, 0.0f, static_cast<float>(WIDTH));
    return min_value + (clamped_y / WIDTH) * (max_value - min_value);
}

void Reset()
{
	line_delay = 0;
	playing = !playing;
	for (Note& n : notes) n.played = false;
	playing_line.x = CUBE_SIZE;
}

bool value_box_select = false;

void UpdateDrawFrame()
{
	// camera.offset.x = Clamp(camera.offset.x, 0, INFINITY);
	mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
	BeginDrawing();
	ClearBackground(RAYWHITE);

	std::cout << camera.offset.x << '\n';

	std::string camera_view_text = "x: " + std::to_string((int)(camera.offset.x / 50)*-1) + ", y: " + std::to_string((int)(camera.offset.y / 50));

	DrawText(camera_view_text.c_str(), 250, 0, 20, BLACK);

	if (camera.offset.x > 0) camera.offset.x = 0;

	for (int x = 0; x < WIDTH/CUBE_SIZE; x++)
	{
		for (int y = 0; y < HEIGHT/CUBE_SIZE; y++)
		{
			Rectangle rec = {50+(float)x*CUBE_SIZE, 50+(float)y*CUBE_SIZE, CUBE_SIZE, CUBE_SIZE};
			DrawRectangleLinesEx(rec, 1.f, GRAY);

			if (CheckCollisionPointRec(mouse_pos, rec))
			{
				mouse_pos = {rec.x+CUBE_SIZE/2, rec.y+CUBE_SIZE/2};
			}
		}
	}

	for (int y = 0; y < instruments.size(); y++)
	{
		Rectangle rect = {0, 50+(float)y*CUBE_SIZE, CUBE_SIZE, CUBE_SIZE};
		instruments[y].row = {50, 50+(float)y*CUBE_SIZE, WIDTH, 50};
		DrawRectangleLinesEx(instruments[y].row, 1.f, RED);
		if (GuiButton(rect, instruments[y].icon.c_str()))
		{
			PlaySound(instruments[y].sound);
		}
	}

	int min = 30;
	int max = INFINITY;

	if (GuiValueBox({0, 0, 100, 50}, "TEMPO", &tempo, min, max, value_box_select))
	{
		value_box_select = !value_box_select;
	}

	// NOTE: STATIC THINGS ARE DRAWN OUTSIDE CAMERA MODE
	BeginMode2D(camera);

	if (IsKeyPressed(KEY_RIGHT))
	{
		camera.offset.x -= 50;
	}

	if (IsKeyPressed(KEY_LEFT))
	{
		camera.offset.x += 50;
	}

	for (Note& note : notes)
	{
		DrawRectangleRec(note.rec, note.color);
	}

	if (!playing)
	{
		DrawCircleV(mouse_pos, CUBE_SIZE/2, Color{0, 0, 0, 100});

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_pos.x >= 50)
		{
			//SetSoundPitch(beep, mapValue(mouse_pos.y, 0.f, 2.f));
			//SetAudioStreamPitch(beep.stream, 5.f);
			//std::cout << mapValue(mouse_pos.y, 0.f, 2.f) << '\n';
			for (Instrument& instrument : instruments)
			{
				if (CheckCollisionPointRec(mouse_pos, instrument.row))
				{
					bool on_note = false;
					int note_i = NULL;
					for (int i = 0; i < notes.size(); i++)
					{
						Note note = notes[i];
						if (CheckCollisionPointRec(mouse_pos, note.rec))
						{
							on_note = true;
							note_i = i;
						}
					}

					if (!on_note)
					{
						notes.push_back(Note{
							instrument.sound,
							Rectangle { mouse_pos.x-CUBE_SIZE/2, mouse_pos.y-CUBE_SIZE/2, CUBE_SIZE, CUBE_SIZE },
							instrument.row_clr
						});
						PlaySound(instrument.sound);
					}
					else
					{
						notes.erase(notes.begin() + note_i);
					}
				}
			}
			//SetAudioStreamPitch(beep.stream, 1.f);
		}
	}

	if (playing)
	{
        float frames_per_beat = (GetFPS() * 60.0f) / tempo;
        float seconds_per_beat = 60.0f / tempo;
        float speed_per_frame = CUBE_SIZE / seconds_per_beat;

        playing_line.x += speed_per_frame * GetFrameTime();

		if (playing_line.x > WIDTH+CUBE_SIZE)
		{
			Reset();
			playing = true;
		}

		for (Note& note : notes)
		{
			if (CheckCollisionRecs(note.rec, playing_line) && !note.played && playing)
			{
				PlaySound(note.sound);
				note.played = true;
			}
		}
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		Reset();
	}

	if (playing) DrawRectangleRec(playing_line, BLACK);

	EndMode2D();
	EndDrawing();
}