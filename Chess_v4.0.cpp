#include <raylib.h>
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <algorithm>
// #include <chrono> <-- You may turn it on and some statements near perft output to test the NPS

#define null 100

struct square {
    Vector2 position;
    Color color;

    square(Vector2 pos, Color col) {
        position = pos;
        color = col;
    }
};

struct inscription {
    uint8_t en; uint16_t kingPositions;
    std::vector<uint8_t> boardposition;

    inscription() {
        en = null; uint16_t kingPositions = null;
        boardposition = std::vector<uint8_t>(64, null);
    }
    
    inscription(uint8_t En, std::vector<uint8_t> position, uint16_t king_pos) {
        en = En; kingPositions = king_pos;
        boardposition = position;
    }
};

enum piececolor {white, black, NONE};
enum movetype {MOVE, CAPTURE, EN_PASSANT, CASTLE}; // Last 2 movetype, 6 before them: index for uint8_t syntax
const std::string note = "KQBNRPkqbnrp", letters = "abcdefgh";
enum piece_type {king, queen, bishop, knight, rook, pawn, none};

const piececolor playcol = white;

std::vector<inscription> history(1024); unsigned int count = 1;
const uint64_t positions = (playcol == black) ? 61152690439 : 69177151; //BR-BK-BR-WR-WK-WR
uint8_t moveds = 255; // BrBRWrWR
uint16_t king_positions = (playcol == white) ? 316 : 3844; // BLCKWHTE

unsigned int getIndexFromPosition(const Vector2 position) {
    return (((unsigned int)(position.x) / 80) + ((unsigned int)(position.y) / 80) * 8);
}

unsigned int LoadHFromLetter(const char chr) {
    for (int idx = 0; idx < 8; idx++) {
        if (chr == letters[(playcol == white) ? idx : 7 - idx]) {return idx;}
    } return null;
}

std::vector<uint8_t> boardstate(64, null);
void ReachKings() {
    king_positions = 0; unsigned int cnt = 0;
    for (unsigned int idx = 0; idx < 64; idx++) {
        const unsigned int val = boardstate[idx];
        if ((val == 0) || (val == 6)) {
            king_positions |= (idx << ((val > 5) ^ playcol) * 6);
            cnt++;
        } if (cnt == 2) {break;}
    }
}

std::string IndexToNotation(const unsigned int& index) {
    return letters[((playcol == white) ? 7 & index : 7-(7 & index))] + std::to_string((playcol == white) ? 8 - (index >> 3) : (index >> 3) + 1);
}

int abs(const int num) {
    if (num < 0) {return (1 + ~num);}
    else {return num;}
}

int sgn(const int num) {return (((num > 0) << 1) - 1);}

bool attack(const piececolor& attacking_col, const unsigned int& squarein);
 
std::vector<uint8_t> getPseudoLegalMoves(const unsigned int& selectedin, const piece_type& piecetype, const piececolor& selcol) {
    const unsigned int iy = selectedin >> 3, ix = 7 & selectedin;
    std::vector<uint8_t> moves;
    if (piecetype == pawn) {
        const unsigned int important = selcol ^ playcol; int dest = selectedin;
        const int dir8 = (important << 4) - 8;
        const unsigned int lim = (iy == (6 - 5 * important)) ? 2 : 1; int dest2 = selectedin + dir8;
        const bool should = (dest2 < 0) || (dest2 > 63);
        if (!should) {
            if (boardstate[dest2] == null) {
                moves.push_back((dest2 << 2) | MOVE);
                dest = dest2 + dir8;
                if ((iy == 5 * (1 & ~important) + 1) && (dest > -1) && (dest < 64) && (boardstate[dest] == null)) {
                    moves.push_back((dest << 2) | MOVE);
                }
            } for (int dx = -1; dx < 2; dx += 2) {
                const unsigned int side = dest2 + dx;
                if ((side < 0) || (side > 63) || ((7 & side) - ix != dx)) {continue;}
                const unsigned int value = boardstate[side];
                if (value != null && ((value > 5) ^ selcol)) {moves.push_back((side << 2) | CAPTURE);}
            } const unsigned int EN = history[count - 1].en;
            if ((EN != null) && (abs((7 & EN) - ix) == 1) && ((EN >> 3) == iy)) {moves.push_back(((EN + dir8) << 2) | EN_PASSANT);}
        }
    } else if (piecetype == bishop || piecetype == queen) {
        int dest = selectedin;
        for (int signy = -8; signy < 9; signy += 16) {
            for (int signx = -1; signx < 2; signx += 2) {
                const int dir = signx + signy;
                for (unsigned int mag = 1; mag < 8; mag++) {
                    dest += dir;
                    if ((dest < 0) || (dest > 63) || (sgn((7 & dest) - ix) != signx)) {break;}
                    const unsigned int value = boardstate[dest];
                    if (value == null) {moves.push_back((dest << 2) | MOVE);}
                    else {if ((value > 5) ^ selcol) {moves.push_back((dest << 2) | CAPTURE);} break;}
                } dest = selectedin;
            }
        }
    } if (piecetype == rook || piecetype == queen) {
        int dest = selectedin;
        for (auto const& dir : {-1, -8, 1, 8}) {
            for (unsigned int mag = 1; mag < 8; mag++) {
                dest += dir;
                const bool Bool = (abs(dir) == 1) && (sgn((7 & dest) - ix) != dir);
                if ((dest < 0) || (dest > 63) || (Bool)) {break;}
                const unsigned int value = boardstate[dest];
                if (value == null) {moves.push_back((dest << 2) | MOVE);}
                else {if ((value > 5) ^ selcol) {moves.push_back((dest << 2) | CAPTURE);} break;}
            } dest = selectedin;
        }
    } else if (piecetype == knight) {
        for (auto const& dis : {-10, 6, -6, 10, -17, -15, 15, 17}) {
            const int dist = selectedin + dis;
            if ((dist < 0) || (dist > 63) || (abs((7 & dist) - ix) > 2)) {continue;}
            const unsigned int value = boardstate[dist];
            if (value == null) {moves.push_back((dist << 2) | MOVE);}
            else if ((value > 5) ^ selcol) {moves.push_back((dist << 2) | CAPTURE);}
        }
    } else if (piecetype == king) {
        for (auto const& dis : {-9, -8, -7, -1, 1, 7, 8, 9}) {
            const int dist = selectedin + dis;
            if ((dist < 0) || (dist > 63) || (abs((7 & dist) - ix) > 1)) {continue;}
            const unsigned int value = boardstate[dist];
            if (value == null) {moves.push_back((dist << 2) | MOVE);}
            else if ((value > 5) ^ selcol) {moves.push_back((dist << 2) | CAPTURE);}
        }

        const unsigned int rsh = selcol << 2, important = selcol ^ playcol, colR6 = selcol * 6 + 4, selcol18 = selcol * 18;
        if (moveds >> rsh) {
            const unsigned int index = 262143 & (positions >> selcol18);
            if (boardstate[63 & (index >> 6)] != (colR6 - 4)) {moveds &= ~(15 << rsh);}
            else {
                if (boardstate[63 & index] != colR6) {moveds &= ~(3 << rsh);}
                if (boardstate[63 & (index >> 12)] != colR6) {moveds &= ~(12 << rsh);}
            }
        }

        const unsigned int k = 15 & (moveds >> rsh);
        if (k && (!attack(selcol, selectedin))) {
            for (unsigned int cnt = 0; cnt < 2; cnt++) {
                if (3 & (k >> (cnt << 1))) {
                    const unsigned int rookpos = 63 & ((positions >> selcol18) >> (cnt * 12));
                    const int dir = 1 - (cnt << 1);

                    bool yes = true;
                    for (unsigned int pos = selectedin + dir; pos != rookpos; pos += dir) {
                        if (boardstate[pos] != null) {yes = false; break;}
                        else if ((abs(pos - selectedin) < 3) && attack(selcol, pos)) {std::cout << "\nerror2"; yes = false; break;}
                    } if (yes) {moves.push_back(((selectedin + (dir << 1)) << 2) | CASTLE);}
                }
            }
        }
    } return moves;
}

bool attack(const piececolor& king_col, const unsigned int& squarein) {
    for (auto const& piecetype : {bishop, rook, knight, pawn}) {
        const std::vector<uint8_t> moves = getPseudoLegalMoves(squarein, piecetype, king_col);
        for (auto const& move : moves) {
            if ((3 & move) == CAPTURE) {
                const unsigned int val = boardstate[move >> 2], modval = val - (val > 5) * 6;
                if (modval == piecetype) {return true;}
                if ((modval == queen) && ((piecetype == bishop) || (piecetype == rook))) {return true;}
            }
        }
    } return false;
}

void undo_move() {
    count--;
    const inscription temp = history[count - 1];
    boardstate = temp.boardposition;
    king_positions = temp.kingPositions;
}

uint8_t Promotion = 0; // first 6 bits: pos | next 1 bit: Color | last 1 bit: Promotion?
bool make_move(const piececolor& selcol, const piece_type& piecetype, const movetype& move_type, const unsigned int& pos,
    const unsigned int& selectedin) {
    const unsigned int selcol6 = selcol * 6, species = selcol6 + piecetype;
    if (move_type == EN_PASSANT) {boardstate[pos - ((selcol ^ playcol) << 4) + 8] = null;}
    else if (move_type == CASTLE) {
        boardstate[(selectedin + pos) >> 1] = species + 4;
        boardstate[((7 & pos) > (7 & selectedin)) ? 7 + ((pos >> 3) << 3) : ((pos >> 3) << 3)] = null;
    }
            
    boardstate[pos] = species; boardstate[selectedin] = null;

    if (piecetype == pawn) {
        const unsigned int k = pos >> 3;
        if (k == 0 || k == 7) {Promotion = (pos << 2) | (selcol << 1) | 1;}
    }

    history[count] = inscription(((piecetype == pawn) && (abs(pos - selectedin) == 16)) ? pos : null, boardstate, king_positions);
    count++;

    if (attack(selcol, (piecetype != king) ? 63 & (king_positions >> selcol6) : pos)) {undo_move(); return false;}
    else if (piecetype == king) {
        king_positions &= ~(63 << selcol6); king_positions |= (pos << selcol6); history[count - 1].kingPositions = king_positions;
    } return true;
}

bool move(const unsigned int& selectedin, const piece_type& piecetype, 
    const piececolor& selcol, const unsigned int& pos) {
    const std::vector<uint8_t> moves = getPseudoLegalMoves(selectedin, piecetype, selcol);
    for (auto const& move : moves) {
        if ((move >> 2) == pos) {return make_move(selcol, piecetype, static_cast<movetype>(3 & move), pos, selectedin);}
    } return false;
}

int perft(const unsigned int depth, const piececolor current_col) {
    if (depth == 0) {return 1;}
    int nodes = 0;
    for (unsigned int index = 0; index < 64; index++) {
        const unsigned int val = boardstate[index];
        const piececolor piececol = static_cast<piececolor>(val > 5);
        if ((val != null) && (piececol == current_col)) {
            const piece_type piecetype = static_cast<piece_type>(val - piececol * 6);
            const std::vector<uint8_t> moves = getPseudoLegalMoves(index, piecetype, piececol);
            for (auto const& move : moves) {
                const unsigned int dest = move >> 2;
                if (make_move(piececol, piecetype, static_cast<movetype>(3 & move), dest, index)) {
                    nodes += perft(depth - 1, static_cast<piececolor>(1 & ~current_col));
                    undo_move();
                }
            }
        }
    } return nodes;
}

int main() {
    /*--- Data collection ---*/
    const std::string fen_0 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w --"; /* Special format of FEN which takes the main notation, 
                                                                                     turn color and en square */
    
    std::string fen = fen_0; std::cin >> fen;
    if (fen == "Default" || fen == "default" || fen == "DEFAULT") {fen = fen_0;}

    const unsigned int size2 = fen.size() - 2; const char end = fen.back();
    piececolor turncol = (fen[size2 - 2] == 'w') ? white : black;
    if (playcol == black) {std::reverse(fen.begin(), fen.end() - 2);}

    uint8_t index = 0;
    for (auto const& chr : fen) {
        if (chr == ' ') {break;}
        else if (chr != '/') {
            if (chr < '9') {index += chr - '0';}
            else {
                for (unsigned int idx = 0; idx < 12; idx++) {
                    if (note[idx] == chr) {
                        boardstate[index] = idx;
                        break;
                    }
                } index++;
            }
        }
    } const unsigned int en_square = (end == '-') ? null : 
    ((((playcol == white) ? 8 - end + '0' : end - '0' - 1) << 3) + LoadHFromLetter(fen[size2]));
    if (fen != fen_0) {ReachKings();}
    history[0] = inscription(en_square, boardstate, king_positions);

    /*--- Graphical Initialization ---*/
    InitWindow(640, 640, "Chess v4.0.0");
    SetTargetFPS(60);

    std::vector<square> cells;
    for (unsigned int y = 0; y < 8; y++) {
        for (unsigned int x = 0; x < 8; x++) {
            cells.push_back(square({(x << 4) * 5.f, (y << 4) * 5.f},
            (((x + y) % 2 == 0) ? (Color){223, 203, 171, 255} : (Color){164, 126, 91, 255})));
        }
    }

    const std::vector<std::string> colors = {"white", "black"}, names = {"king", "queen", "bishop", "knight", "rook", "pawn"};
    std::vector<Texture2D> textures;
    for (auto const& color : colors) {
        for (auto const& name : names) {
            const std::string filepath = color + "-" + name + ".png";
            textures.push_back(LoadTexture(filepath.c_str()));
        }
    }

    /* auto start_time = std::chrono::high_resolution_clock::now(); YOU MAY TURN THESE ON
    const float nodes = perft(4, white);
    std::cout << "\n" << nodes;
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "\nNPS = ~" << ((nodes * 1e9) / float(std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count())); */
    /*--- Interaction & Rendering ---*/
    unsigned int selectedin = null; piece_type piecetype = none; piececolor selcol = NONE;
    while (!WindowShouldClose()) {
        const bool Bool = 1 & Promotion; unsigned int promocol6 = (1 & (Promotion >> 1)) * 6;
        if (IsKeyPressed(KEY_Z)) {undo_move();}
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const unsigned int pos = getIndexFromPosition(GetMousePosition());
            
            if (!Bool) {
                const unsigned int value = boardstate[pos];
                const piececolor col = static_cast<piececolor>(value > 5);

                if (selectedin != null && move(selectedin, piecetype, selcol, pos)) {
                    turncol = static_cast<piececolor>(1 & (~turncol));
                    selectedin = null;
                } else if (value != null && turncol == col) {
                    selectedin = pos; piecetype = static_cast<piece_type>((value > 5) ? (value - 6) : value); selcol = col;}
            } else if (pos > 25 && pos < 30) {
                Promotion &= 254; const unsigned int pos63 = Promotion >> 2, type = promocol6 + pos - 25;
                boardstate[pos63] = type; history[count - 1].boardposition[pos63] = type;
            }
        }

        BeginDrawing();
            ClearBackground(BLUE);
            for (auto const& cell : cells) {DrawRectangleV(cell.position, {80.f, 80.f}, cell.color);}
            if (selectedin != null) {DrawRectangleV(cells[selectedin].position, {80.f, 80.f}, (Color){130, 151, 105, 178});}
            for (uint8_t index = 0; index < 64; index++) {
                const uint8_t value = boardstate[index];
                if (value != null) {
                    const Vector2 pos = cells[index].position;
                    DrawTexturePro(textures[value], {0.f, 0.f, 128.f, 128.f}, {pos.x, pos.y, 80.f, 80.f}, {0.f, 0.f}, 0.f, WHITE);
                }
            } for (unsigned int k = 0; k < 8; k++) {
                const unsigned int use = (playcol == black) ? (7 & (~k)) : k,
                num = 8 - use; const std::string str(1, letters[use]);

                const float coord = (k << 4) * 5.f + 40.f;
                DrawText(TextFormat("%i", num), 2, coord, 12, BLACK);
                DrawText(TextFormat("%i", num), 630, coord, 12, BLACK);

                DrawText(str.c_str(), coord, 2, 12, BLACK);
                DrawText(str.c_str(), coord, 627, 12, BLACK);
            }

            if (Bool) {
                DrawRectangle(160, 240, 320, 80, WHITE); DrawRectangleLines(160, 240, 320, 80, BLACK);
                for (unsigned int add = 1; add < 5; add++) {
                    DrawTexturePro(textures[promocol6 + add], {0.f, 0.f, 120.f, 120.f}, {80.f + add * 80.f, 240.f, 80.f, 80.f}, {0.f, 0.f},
                                   0.f, WHITE);
                }
            }
        EndDrawing();
    }

    CloseWindow();
    for (auto& texture : textures) {UnloadTexture(texture);}
    return 0;
}