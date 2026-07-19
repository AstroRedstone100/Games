// This time, I made it!
#include "raylib.h"
#include <vector>
#include <string>
#include <random>
#include <iostream>

struct cell {
    float side;
    Vector2 position;
    Color color;
    bool lock;
    cell(Vector2 Pos, bool LOCK) {
        side = 20.f;
        position = Pos;
        color = BLACK;
        lock = LOCK;
    }
};

struct tetromino {
    std::string type;
    int currentR;
    Vector2 position;
    std::vector<std::vector<int>> structures;
    Color color;
    tetromino(std::string species, int rotation, Vector2 pos) {
        type = species;
        currentR = rotation;
        position = pos;
        if (type == "O") {
            structures = {{0, 1, 10, 11}, {0, 1, 10, 11}, {0, 1, 10, 11}, {0, 1, 10, 11}};
            color = (Color){253, 249, 0, 255};
        } else if (type == "I") {
            structures = {{10, 11, 12, 13}, {2, 12, 22, 32}, {20, 21, 22, 23}, {1, 11, 21, 31}};
            color = (Color){0, 255, 255, 255};
        } else if (type == "L") {
            structures = {{2, 10, 11, 12}, {1, 11, 21, 22}, {10, 11, 12, 20}, {0, 1, 11, 21}};
            color = (Color){255, 161, 0, 255};
        } else if (type == "J") {
            structures = {{0, 10, 11, 12}, {1, 2, 11, 21}, {10, 11, 12, 22}, {1, 11, 20, 21}};
            color = (Color){0, 82, 172, 255};
        } else if (type == "S") {
            structures = {{1, 2, 10, 11}, {1, 11, 12, 22}, {11, 12, 20, 21}, {0, 10, 11, 21}};
            color = (Color){0, 255, 0, 255};
        } else if (type == "T") {
            structures = {{1, 10, 11, 12}, {1, 11, 12, 21}, {10, 11, 12, 21}, {1, 10, 11, 21}};
            color = (Color){112, 31, 126, 255};
        } else {
            structures = {{0, 1, 11, 12}, {2, 11, 12, 21}, {10, 11, 21, 22}, {1, 10, 11, 20}};
            color = (Color){255, 0, 0, 255};
        }
    }
};

int randint(int min, int max) {
    std::random_device randomi;
    std::mt19937 gen(randomi());
    std::uniform_int_distribution<> distr(min, max);
    return (distr(gen));
}

int main() {
    std::cout << "Spamming keys might cause some game functions to fail :|" << std::endl;

    InitWindow(380, 440, "Tetris (The final try)");
    SetTargetFPS(60);

    Vector2 Default_pos = {-200.f, 0.f};
    std::vector<std::string> types = {"I", "J", "L", "O", "S", "Z", "T"};
    std::vector<int> randombag, rowbag, patrick;
    std::vector<tetromino> tetrobag;

    std::vector<cell> maincells, nextcells;
    for (float y = 0.f; y < 420.f; y += 20.f) {
        for (float x = 20.f; x < 220.f; x += 20.f) {
            cell square = cell((Vector2){x, y}, false);
            maincells.push_back(square);
        }
    } for (float y = 20.f; y < 340.f; y += 20.f) {
        for (float x = 240.f; x < 360.f; x += 20.f) {
            cell squar_e = cell((Vector2){x, y}, false);
            nextcells.push_back(squar_e);
        }
    }
    
    float timefill = 0;
    int startin = 3, score = 0;
    bool somebool = true, End = false;
    while (!WindowShouldClose()) {
        if (!End) {
            while (randombag.size() != 7) {
                int check = randint(0, 6);
                if (randombag.size() > 0) {
                    bool checkbool = true;
                    for (auto const& num : randombag) {
                        if (num == check) {
                            checkbool = false;
                        }
                    } if (checkbool) {randombag.push_back(check);}
                } else {randombag.push_back(check);}
            } while (tetrobag.size() != 7) {
                std::string reqd_string = types[randombag[tetrobag.size()]];
                tetromino retro = tetromino(reqd_string, 0, Default_pos);
                tetrobag.push_back(retro);
            }

            for (int idx = 3; idx > -1; idx--) {
                for (auto const& val : (tetrobag[idx].structures)[0]) {
                    int correct_val = (val/10 * 6) + (val % 10);
                    nextcells[7 + (3-idx)*24 + correct_val].color = tetrobag[idx].color;
                }
            }

            std::vector<int> veriable = tetrobag[0].structures[tetrobag[0].currentR];
            if ((timefill < 0.5) && somebool) {
                for (auto const& val : veriable) {
                    maincells[startin + val].color = tetrobag[0].color;
                }
            } else if (timefill >= 0.5) {
                somebool = false;
                for (auto& Square : maincells) {
                    if (!(Square.lock)) {
                        Square.color = BLACK;
                    }
                } for (auto const& val : veriable) {
                    maincells[startin + 10 + val].color = tetrobag[0].color;
                } timefill -= 0.5;
                startin += 10;
            }
            timefill += GetFrameTime();

            bool Lock = false;
            for (int idx = 0; idx < 210; idx++) {
                cell k = maincells[idx];
                if (!(k.lock)) {
                    if ((k.color.g != 0) || (k.color.g == 0 && k.color.r == 255)) {
                        if (idx > 199) {
                            Lock = true;
                            break;
                        }
                    }
                }
            } for (auto const& val : veriable) {
                if (maincells[startin + val + 10].lock) {
                    Lock = true;
                    break;
                }
            } if (Lock) {
                for (auto& Cell : maincells) {
                    if (!(Cell.color.r == 0 && Cell.color.g == 0)) {
                        Cell.lock = true;
                    }
                } startin = 3;
                for (auto& Cell : nextcells) {
                    Cell.color = BLACK;
                } randombag.erase(randombag.begin());
                tetrobag.erase(tetrobag.begin());
                somebool = true;
            }

            for (int idx = 13; idx < 17; idx++) {
                if (maincells[idx].lock) {
                    End = true;
                    break;
                }
            }

            int Button = GetKeyPressed();
            if ((Button == KEY_A) || (Button == KEY_D)) {
                int reqd_val = 0, motion = 0;
                if (IsKeyPressed(KEY_A)) {
                    reqd_val = 0; motion = -1;
                } else {reqd_val = 9; motion = 1;}
                bool go = true;
                for (auto const& val : veriable) {
                    if ((val + startin) % 10 == reqd_val || maincells[val + startin + motion].lock) {
                        go = false;
                        break;
                    }
                } if (go) {startin += motion;}
            } if ((Button == KEY_Q) || (Button == KEY_E)) {
                int rotat = tetrobag[0].currentR, move = -1;
                if (Button == KEY_Q) {move = 1;}
                if (rotat + move == 4) {move = -3;} else if (rotat + move == -1) {move = 3;}
                std::vector<int> exemple = (tetrobag[0].structures)[rotat + move];
                bool GO = true;
                for (auto const& val : exemple) {
                    int value = startin + val;
                    if (value > -1 && value < 210) {
                        if (maincells[value].lock) {
                            GO = false;
                            break;
                        } if (abs((value % 10) - (startin % 10)) > 3) {
                            GO = false;
                            break;
                        }
                    } else {GO = false; break;}
                } if (GO) {tetrobag[0].currentR += move;}
            }

            for (int idx1 = 209; idx1 > -1; idx1 -= 10) {
                bool Check = true;
                for (int idx2 = idx1; idx2 > idx1 - 10; idx2--) {
                    if (!(maincells[idx2].lock)) {
                        Check = false;
                        break;
                    }
                } if (Check) {
                    for (int idx = idx1; idx > idx1 - 10; idx--) {
                        maincells[idx].color = BLACK;
                        maincells[idx].lock = false;
                    }
                    rowbag.push_back(idx1);
                }
            } int n = rowbag.size();
            if (n != 0) {score += 145*n*n*n - 925*n*n + 1920*n - 1100;}
            for (int cnt = 0; cnt < n; cnt++) {
                for (int idx1 = rowbag[n - 1 - cnt] - 10; idx1 > 9; idx1--) {
                    if (!(idx1 > 12 && idx1 < 17)) {
                        maincells[idx1 + 10].color = maincells[idx1].color;
                        maincells[idx1 + 10].lock = maincells[idx1].lock;

                        maincells[idx1].color = BLACK;
                        maincells[idx1].lock = false;
                    }
                }
            } rowbag.clear();
        }

        BeginDrawing();
            ClearBackground(GRAY);
            for (auto const& Square : maincells) {
                DrawRectangle((int)Square.position.x, (int)Square.position.y, (int)Square.side, (int)Square.side, Square.color);
            } DrawRectangle(0, 0, 380, 20, GRAY);
            for (auto const& Square : nextcells) {
                DrawRectangle((int)Square.position.x, (int)Square.position.y, (int)Square.side, (int)Square.side, Square.color);
            } DrawText(("SCORE: " + std::to_string(score)).c_str(), 240, 2, 16, WHITE);
            DrawText("NEXT:", (int)nextcells[4].position.x - 6, (int)nextcells[4].position.y + 3, 15, WHITE);
            DrawText("HOLD:", (int)nextcells[76].position.x - 6, (int)nextcells[76].position.y + 3, 15, WHITE);
            if (End) {DrawText("GAME OVER", 100, 210, 30, YELLOW);}
        EndDrawing();
    }
    return 0;
}