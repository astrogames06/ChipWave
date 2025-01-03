g++ src/main.cpp src/App/App.cpp -o game.exe -O1 -Wall -std=c++17 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
.\game.exe