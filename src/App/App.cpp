#include "App.hpp"
#include <raygui.h>

bool value_box_select = false;

void App::Init()
{
    InitWindow(app.WIDTH+50, app.HEIGHT, "raylib [core] example - basic window");
	InitAudioDevice();
	GuiLoadIcons("iconset.rgi", true);

	app.sound_files = LoadDirectoryFiles("sounds");

	for (int i = 0; i < app.sound_files.count; i++)
	{
		// Wave w = LoadWave(sound_files.paths[i]);
		// std::string p = std::string(sound_files.paths[i]) + ".h";
		// ExportWaveAsCode(w, p.c_str());
		Instrument instrument = {LoadSound(app.sound_files.paths[i]), app.sound_icons[i].c_str()};
		instrument.row_clr = {
			(unsigned char) GetRandomValue(0, 255),
			(unsigned char) GetRandomValue(0, 255),
			(unsigned char) GetRandomValue(0, 255),
			255
		};
		app.instruments.push_back(instrument);
	}
	

	app.camera.zoom = 1.f;
}

void App::Update()
{
    app.mouse_pos = GetScreenToWorld2D(GetMousePosition(), app.camera);
}

void App::Draw()
{
    BeginDrawing();
	ClearBackground(RAYWHITE);

	//std::cout << app.camera.offset.x << '\n';

	std::string camera_view_text = "x: " + std::to_string((int)(app.camera.offset.x / 50)*-1) + ", y: " + std::to_string((int)(app.camera.offset.y / 50));

	DrawText(camera_view_text.c_str(), 250, 0, 20, BLACK);

	if (app.camera.offset.x > 0) app.camera.offset.x = 0;

	for (int x = 0; x < app.WIDTH/app.CUBE_SIZE; x++)
	{
		for (int y = 0; y < app.HEIGHT/app.CUBE_SIZE; y++)
		{
			Rectangle rec = {50+(float)x*app.CUBE_SIZE, 50+(float)y*app.CUBE_SIZE, app.CUBE_SIZE, app.CUBE_SIZE};
			DrawRectangleLinesEx(rec, 1.f, GRAY);

			if (CheckCollisionPointRec(app.mouse_pos, rec))
			{
				app.mouse_pos = {rec.x+app.CUBE_SIZE/2, rec.y+app.CUBE_SIZE/2};
			}
		}
	}

	for (int y = 0; y < app.instruments.size(); y++)
	{
		Rectangle rect = {0, 50+(float)y*app.CUBE_SIZE, app.CUBE_SIZE, app.CUBE_SIZE};
		app.instruments[y].row = {50, 50+(float)y*app.CUBE_SIZE, (float)app.WIDTH, 50};
		DrawRectangleLinesEx(app.instruments[y].row, 1.f, RED);
		if (GuiButton(rect, app.instruments[y].icon.c_str()))
		{
			PlaySound(app.instruments[y].sound);
		}
	}

	int min = 30;
	int max = 330;

	if (GuiValueBox({0, 0, 100, 50}, "TEMPO", &app.tempo, min, max, value_box_select))
	{
		value_box_select = !value_box_select;
	}

	// NOTE: STATIC THINGS ARE DRAWN OUTSIDE CAMERA MODE
	BeginMode2D(app.camera);

	if (IsKeyPressed(KEY_RIGHT))
	{
		app.camera.offset.x -= 50;
	}

	if (IsKeyPressed(KEY_LEFT))
	{
		app.camera.offset.x += 50;
	}

	for (Note& note : app.notes)
	{
		DrawRectangleRec(note.rec, note.color);
	}

	if (!app.playing)
	{
		DrawCircleV(app.mouse_pos, app.CUBE_SIZE/2, Color{0, 0, 0, 100});

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && app.mouse_pos.x >= 50)
		{
			//SetSoundPitch(beep, mapValue(mouse_pos.y, 0.f, 2.f));
			//SetAudioStreamPitch(beep.stream, 5.f);
			//std::cout << mapValue(mouse_pos.y, 0.f, 2.f) << '\n';
			for (Instrument& instrument : app.instruments)
			{
				if (CheckCollisionPointRec(app.mouse_pos, instrument.row))
				{
					bool on_note = false;
					int note_i = NULL;
					for (int i = 0; i < app.notes.size(); i++)
					{
						Note note = app.notes[i];
						if (CheckCollisionPointRec(app.mouse_pos, note.rec))
						{
							on_note = true;
							note_i = i;
						}
					}

					if (!on_note)
					{
						app.notes.push_back(Note{
							instrument.sound,
							Rectangle {
								app.mouse_pos.x-app.CUBE_SIZE/2,
								app.mouse_pos.y-app.CUBE_SIZE/2,
								(float)app.CUBE_SIZE,
								(float)app.CUBE_SIZE
							},
							instrument.row_clr
						});
						PlaySound(instrument.sound);
					}
					else
					{
						app.notes.erase(app.notes.begin() + note_i);
					}
				}
			}
			//SetAudioStreamPitch(beep.stream, 1.f);
		}
	}

	if (app.playing)
	{
        float frames_per_beat = (GetFPS() * 60.0f) / app.tempo;
        float seconds_per_beat = 60.0f / app.tempo;
        float speed_per_frame = app.CUBE_SIZE / seconds_per_beat;

        app.playing_line.x += speed_per_frame * GetFrameTime();

		if (app.playing_line.x > app.WIDTH+app.CUBE_SIZE)
		{
			app.Reset();
			app.playing = true;
		}

		for (Note& note : app.notes)
		{
			if (CheckCollisionRecs(note.rec, app.playing_line) && !note.played && app.playing)
			{
				PlaySound(note.sound);
				note.played = true;
			}
		}
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		app.Reset();
	}

	if (app.playing) DrawRectangleRec(app.playing_line, BLACK);

	EndMode2D();
	EndDrawing();
}