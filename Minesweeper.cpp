#include "raylib.h"
#include <vector>
#include <random>
#include <cmath>

const int max_minecount = 31, remaining = 200 - max_minecount;
#define M 9

struct cell {
    Vector2 position;
    bool containsMine, open, flag;
    int figure;
    cell(Vector2 pos) {
        position = pos;
        containsMine = false;
        open = false;
        flag = false;
    }
};

int randnum() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, 199);
    return dist(gen);
}

const std::vector<Color> colors = {WHITE, BLUE, GREEN, RED, DARKBLUE, BROWN, {0, 255, 255, 255}, BLACK, LIGHTGRAY};
int DrawFigure(int figure, Vector2 position) {
    if (figure == 0) {return 0;}
    int x = (int)position.x + 8, y = (int)position.y + 8;
    if (figure == M) {DrawRectangle(x, y, 24, 24, BLACK);}
    else {DrawText(TextFormat("%i", figure), x, y, 24, colors[figure]);}
    return figure;
}

void DrawFlagV(Vector2 position) {
    int x = (int)position.x + 8, y = (int)position.y + 8;
    DrawRectangle(x, y, 2, 24, BLACK);
    DrawRectangle(x, y, 24, 16, RED);
}

int main() {
    InitWindow(800, 400, "Minesweeper v2.0");
    SetTargetFPS(60);

    std::vector<cell> cells;
    for (float y = 0.f; y < 400.f; y += 40.f) {
        for (float x = 0.f; x < 800.f; x += 40.f) {
            cell Cell({x, y});
            cells.push_back(Cell);
        }
    }

    bool EndGame = false, Win = false; int clicks = 0;
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !EndGame && !Win) {
            Vector2 mousepos = GetMousePosition();
            int Index = (int)mousepos.x/40 + ((int)mousepos.y/40)*20;
            if (!cells[Index].flag) {cells[Index].open = true; clicks++;}
            if (clicks == 1) {
                std::vector<int> random_indices;
                while (random_indices.size() < max_minecount) {
                    bool go = true; int idx = randnum(); if (idx == Index) {go = false;}
                    if (go) {for (auto const& index : random_indices) {if (index == idx) {go = false; break;}}}
                    if (go) {random_indices.push_back(idx);}
                } for (auto const& index : random_indices) {cells[index].containsMine = true;}

                for (int idx = 0; idx < 200; idx++) {
                    if (cells[idx].containsMine) {cells[idx].figure = M; continue;}
                    else {
                        int minecount = 0;
                        for (int xdis = -1; xdis < 2; xdis++) {
                            for (int ydis = -20; ydis < 21; ydis += 20) {
                                int newin = idx + xdis + ydis;
                                if (newin < 0 || newin > 199 || std::abs((newin % 20) - (idx % 20)) > 1 || newin == idx) {continue;}
                                if (cells[newin].containsMine) {minecount++;}
                            }
                        } cells[idx].figure = minecount;
                    }
                }
            }
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !EndGame && !Win){
            Vector2 mousepos = GetMousePosition();
            int Index = (int)mousepos.x/40 + ((int)mousepos.y/40)*20;
            cell Cell = cells[Index];
            if (!Cell.open) {
                if (Cell.flag) {cells[Index].flag = false;}
                else {cells[Index].flag = true;}
            }
        }

        BeginDrawing();
            ClearBackground(WHITE);
            for (int Idx = 0; Idx < 200; Idx++) {
                cell Cell = cells[Idx];
                Vector2 position = Cell.position;
                if (!Cell.open) {
                    DrawRectangleV(position, {40.f, 40.f}, GRAY);
                    if (Cell.flag) {DrawFlagV(position);}
                } else {
                    int figure = Cell.figure;
                    if (DrawFigure(figure, position) == M) {EndGame = true;}
                    if (figure == 0) {
                        for (auto const& dis : {-21, -20, -19, -1, 1, 19, 20, 21}) {
                            int newin = Idx + dis;
                            if (newin < 0 || newin > 199 || std::abs((newin % 20) - (Idx % 20)) > 1 || newin == Idx) {continue;}
                            cells[newin].open = true;
                        }
                    }
                } DrawRectangleLines((int)position.x, (int)position.y, 40, 40, {100, 100, 100, 255});
            } if (EndGame) {DrawText("Game Over", 320, 190, 30, BLACK);}
            else if (Win) {DrawText("You Won!", 340, 190, 30, BLACK);}
        EndDrawing();

        if (!EndGame) {
            int cnt = 0;
            for (auto const& Cell : cells) {if (Cell.open) {cnt++;}}
            if (cnt == remaining) {Win = true;}
        }
    }

    CloseWindow();
    return 0;
}