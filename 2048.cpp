#include "raylib.h"
#include <cstdint>
#include <random>
#include <iostream>
#include <fstream>
static std::mt19937 gen(std::random_device{}());
static std::uniform_int_distribution<> choose24(1, 10);

const unsigned char rgbs[39] = {204, 192, 179, 238, 228, 218, 237, 224, 200, 242, 177, 121,
                                245, 149, 99, 246, 124, 95, 246, 94, 59, 237, 207, 114, 237,
                                204, 97, 237, 200, 80, 237, 197, 63, 237, 194, 46, 60, 58, 50};

const int sizes[30] = {32, 26, 32, 26, 32, 26, 32, 20, 32, 19, 32, 18, 32, 12, 32, 10, 32, 11, 28, 9, 28, 3, 28, 3, 28, 9, 22, 7, 22, 3};
const Color bordercol = (Color){187, 173, 160, 255};

void addTile(uint64_t& state) {
    uint64_t openways = 0ULL; int count = -1;
    for (int index = 0; index < 16; index++) {
        if ((15ULL & (state >> (index << 2))) == 0) {
            openways = (openways << 4) | index;
            count++;
        }
    } if (count == -1) {return;}
    std::uniform_int_distribution<> where(0, count);
    state |= (uint64_t)((choose24(gen) == 10) ? 2 : 1) << ((15 & (openways >> (where(gen) << 2))) << 2);
}

uint64_t transpose(const uint64_t& state) {
    uint64_t newstate = 0ULL;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            newstate |= (15 & (state >> (60 - (y << 4) - (x << 2)))) << (60 - (x << 4) - (y << 2));
        }
    } return newstate;
}

uint64_t swapRows(const uint64_t& state) {
    return (((state & 0x000000000000FFFFULL) << 48) | ((state & 0x00000000FFFF0000ULL) << 16) |
            ((state & 0x0000FFFF00000000ULL) >> 16) | ((state & 0xFFFF000000000000ULL) >> 48));
}

uint16_t row_moves[65536];
uint64_t rs(const uint64_t& state) {
    return (row_moves[state & 0x000000000000FFFFULL] | (row_moves[((state & 0x00000000FFFF0000ULL) >> 16)] << 16) |
            ((uint64_t)(row_moves[((state & 0x0000FFFF00000000ULL) >> 32)]) << 32) |
            ((uint64_t)(row_moves[((state & 0xFFFF000000000000ULL) >> 48)]) << 48));
}

int main() {
    InitWindow(300, 300, "2048 v2.0");
    SetTargetFPS(60);

    std::ifstream in("row_moved.bin", std::ios::binary);
    if (!in) {std::cerr << "\nError 404: row_moved.bin not found"; return -1;}
    in.read(reinterpret_cast<char*>(row_moves), sizeof(uint16_t) * 65536);
    in.close();

    uint64_t boardstate = 0ULL;
    addTile(boardstate); addTile(boardstate);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_D)) {boardstate = rs(boardstate); addTile(boardstate);}
        if (IsKeyPressed(KEY_A)) {
            boardstate = transpose(swapRows(transpose(rs(transpose(swapRows(transpose(boardstate))))))); addTile(boardstate);
        } if (IsKeyPressed(KEY_W)) {boardstate = swapRows(transpose(rs(transpose(swapRows(boardstate))))); addTile(boardstate);}
        if (IsKeyPressed(KEY_S)) {boardstate = transpose(rs(transpose(boardstate))); addTile(boardstate);}
        BeginDrawing();
            ClearBackground(bordercol);
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    const int shift = 60 - (y << 4) - (x << 2),
                    value = (15 & (boardstate >> shift)),
                    ix = ((x*37) << 1) + 4, iy = ((y*37) << 1) + 4;

                    const int value2 = (value < 13) ? value*3 : 36;
                    DrawRectangle(ix, iy, 70, 70, (Color){rgbs[value2], rgbs[value2+1], rgbs[value2+2], 255});
                    if (value > 0) {
                        const int value3 = (value << 1) - 2;
                        DrawText(TextFormat("%i", (1 << value)), ix+sizes[value3+1], iy+18, sizes[value3],
                        (value < 9) ? (Color){119, 110, 101, 255} : (Color){249, 246, 242, 255});
                    }
                }
            } DrawText(TextFormat("FPS: %i", GetFPS()), 2, 2, 12, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}