#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

int null = 39879;

int cotoin(Vector2 coords) {
    return (int)(coords.y / 80.f) * 8 + (int)(coords.x / 80.f);
}

Vector2 intoco(int in) {
    return {(in % 8) * 80.f, (in / 8) * 80.f};
}

std::string intono(int in, std::vector<std::string> letters) {
    return letters[in % 8] + std::to_string(8 - (in / 8));
}

struct cell {
    Vector2 position;
    Color color;
    cell(Vector2 pos) {
        position = pos;
        color = (Color){238, 238, 210, 255};
    }
};

struct piece {
    Vector2 position;
    Texture2D texture;
    int code; bool en, moved;
    piece(Vector2 pos, Texture2D tex, int NSC) {
        position = pos;
        texture = tex;
        code = NSC;
        en = false; moved = false;
    }
};

struct move {
    int movein, type;
    move(int in, int Type) {
        movein = in;
        type = Type; // 0->Normal, 1->Capture, 2-> Castle, 3->En passant, 4->Promotion, 5->Cap-promo
    }
};

std::vector<int> repre = {151, 141, 131, 191, 181, 231, 241, 251,
                          111, 211, 311, 411, 511, 611, 711, 811,
                          null, null, null, null, null, null, null, null,
                          null, null, null, null, null, null, null, null,
                          null, null, null, null, null, null, null, null,
                          null, null, null, null, null, null, null, null,
                          110, 210, 310, 410, 510, 610, 710, 810,
                          150, 140, 130, 190, 180, 230, 240, 250}; // Currently ID, then indices.

std::vector<piece> pieces;
void moves(int piecein, std::vector<move>& legalmoves, int posin, int code, int col) {
    int score = (code % 100) / 10, opcol = 1 - col, xcomp = posin % 8, ycomp = posin / 8;
    if (score == 1) {
        int vel = 8 * ((col * 2) - 1), lim = 2, promoy = col * 7;
        if (ycomp == opcol * 5 + 1) {lim = 3;}
        for (int cnt = 1; cnt < lim; cnt++) {
            int newposin = posin + vel * cnt;
            if (newposin < 0 || newposin > 63) {break;}
            if (repre[newposin] != null) {break;}
            int mt = (newposin / 8 == promoy) ? 4 : 0;
            legalmoves.push_back(move(newposin, mt));
        } for (int xdis = -1; xdis < 2; xdis += 2) {
            int Nposin = posin + xdis, newposin = Nposin + vel;
            if (newposin < 0 || newposin > 63 || (std::abs((Nposin % 8) - xcomp) != 1)) {continue;}
            int numpro = repre[newposin];
            if (numpro == null) {
                int numberpro = repre[Nposin];
                if (numberpro != null) {
                    if (pieces[numberpro].en) {legalmoves.push_back(move(newposin, 3));}
                }
            } else if (pieces[numpro].code % 10 == opcol) {
                int mt = (newposin / 8 == promoy) ? 5 : 1;
                legalmoves.push_back(move(newposin, mt));
            } 
        }
    } if (score == 3 || score == 9) {
        for (int signy = -8; signy < 9; signy += 16) {
            for (int signx = -1; signx < 2; signx += 2) {
                for (int dis = 1; dis < 8; dis++) {
                    int newposin = posin + dis * (signx + signy);
                    if (newposin < 0 || newposin > 63 || (std::abs((newposin % 8) - xcomp) != dis)) {break;}
                    int numpro = repre[newposin];
                    if (numpro == null) {legalmoves.push_back(move(newposin, 0));}
                    else {
                        if (pieces[numpro].code % 10 == opcol) {legalmoves.push_back(move(newposin, 1));}
                        break;
                    }
                }
            }
        }
    } if (score == 5 || score == 9) {
        for (auto const& i : {-8, -1, 8, 1}) {
            for (int m = 1; m < 8; m++) {
                int newposin = posin + i*m;
                if (newposin < 0 || newposin > 63 || (std::abs(i) == 1 && std::abs((newposin % 8) - xcomp) != m)) {break;}
                int npos = repre[newposin];
                if (npos == null) {legalmoves.push_back(move(newposin, 0));}
                else {
                    if (pieces[npos].code % 10 == opcol) {legalmoves.push_back(move(newposin, 1));}
                    break;
                }
            }
        }
    } if (score == 4) {
        for (auto const& iy : {-1, -2, 2, 1}) {
            for (auto const& ix : {-1, -2, 2, 1}) {
                int absx = std::abs(ix);
                if (absx == std::abs(iy)) {continue;}
                int newposin = posin + ix + iy*8;
                if (newposin < 0 || newposin > 63 || (std::abs((newposin % 8) - xcomp) != absx)) {continue;}
                int npro = repre[newposin];
                if (npro == null) {legalmoves.push_back(move(newposin, 0));}
                else if (pieces[npro].code % 10 == opcol) {legalmoves.push_back(move(newposin, 1));}
            }
        }
    } if (score == 8) {
        for (int ix = -1; ix < 2; ix++) {
            for (int iy = -8; iy < 9; iy += 8) {
                int newposin = posin + ix + iy;
                if (newposin < 0 || newposin > 63 || newposin == posin) {continue;}
                int npro = repre[newposin];
                if (npro == null) {legalmoves.push_back(move(newposin, 0));}
                else if (pieces[npro].code % 10 == opcol) {legalmoves.push_back(move(newposin, 1));}
            }
        } if (!pieces[piecein].moved) {
            for (auto const& Piece : pieces) {
                int CODE = Piece.code;
                if (!Piece.moved && ((CODE % 100) / 10) == 5) {
                    int inc = ((CODE / 100) - 1) * 2 - 1; bool go = true;
                    for (int k = posin + inc; k != cotoin(Piece.position); k += inc) {
                        if (repre[k] != null) {go = false; break;}
                    } if (go) {legalmoves.push_back(move(posin + 2*inc, 2));}
                }
            }
        }
    }
}

Texture2D texture0;
std::vector<int> check(int posin, int kingcol) {
    std::vector<int> attackers;
    int size = pieces.size(), opcol = 1 - kingcol; Vector2 copos = intoco(posin);
    for (auto const& Score : {10, 30, 40, 50, 90}) {
        std::vector<move> Moves; int vari = Score + kingcol;
        pieces.push_back(piece(copos, texture0, vari));
        moves(size, Moves, posin, vari, kingcol);
        for (auto const& MOVE : Moves) {
            int movetype = MOVE.type;
            if (movetype == 1 || movetype == 5) {
                int q = MOVE.movein;
                if (pieces[repre[q]].code % 100 == Score + opcol) {attackers.push_back(q);}
            }
        } pieces.pop_back();
    } return attackers;
}

bool notlegal(int movetype, int& kingposin, int col, int cellin, int selectedpos, int& selected, std::vector<cell> squares, int& piecein,
    int Score) {
    if (movetype == 2) {
        if (check(kingposin, col).size() == 0) {
            int h = ((cellin - selectedpos) / 2), g = h + selectedpos; Vector2 ing = intoco(g);
            pieces[selected].position = ing;
            repre[g] = selected; repre[selectedpos] = null; kingposin = g;
            if (check(kingposin, col).size() == 0) {
                int t = cellin + h, rookposin = (h > 0) ? t : t + h, rookin = repre[rookposin];
                pieces[selected].position = intoco(cellin);
                pieces[rookin].position = ing; repre[rookposin] = null;
                repre[g] = rookin; repre[cellin] = selected; kingposin = cellin;
            }
        }
    } else {
        if (selectedpos == kingposin) {kingposin = cellin;}
        pieces[selected].position = squares[cellin].position;
        repre[cellin] = selected;
        repre[selectedpos] = null;
        if (movetype == 3) {
            int g = (cellin % 8) - (selectedpos % 8) + selectedpos, h = repre[g];
            repre[g] = null;
            pieces.erase(pieces.begin() + h);
            for (int Idx = h; Idx < pieces.size(); Idx++) {repre[cotoin(pieces[Idx].position)] = Idx;}
            if (selected > h) {selected--;} if (kingposin > h) {kingposin--;}
        }
    } if (piecein == null) { // Move
        if (check(kingposin, col).size() > 0) {return true;} else {return false;}
    } else { // Capture
        pieces.erase(pieces.begin() + piecein);
        for (int Idx = piecein; Idx < pieces.size(); Idx++) {repre[cotoin(pieces[Idx].position)] = Idx;}
        if (selected > piecein) {selected--;} if (kingposin > piecein) {kingposin--;}
        if (check(kingposin, col).size() > 0) {return true;} else {return false;}
    }
}

bool movailable(int partycol, int kingposin, std::vector<int> attacks, std::vector<cell> squares) {
    int attacksize = attacks.size();
    auto copyrepre = repre; auto copypieces = pieces;
    std::vector<move> maskmoves;
    if (attacksize >= 2) {
        int num = repre[kingposin];
        moves(num, maskmoves, kingposin, 180 + partycol, partycol);
        for (auto const& Move : maskmoves) {
            if (!notlegal(Move.type, kingposin, partycol, Move.movein, kingposin, num, squares, null, 8)) {
                repre = copyrepre; pieces = copypieces;
                return true;
            } repre = copyrepre; pieces = copypieces;
        }
    } else {
        maskmoves.clear();
        for (int idx = 0; idx < pieces.size(); idx++) {
            int coded = pieces[idx].code, currentposin = cotoin(pieces[idx].position);
            if (coded % 10 == partycol) {
                moves(idx, maskmoves, currentposin, coded, partycol);
                for (auto const& Move : maskmoves) {
                    int o = Move.movein, disguise = kingposin, numk = idx, numj = repre[o];
                    if (!notlegal(Move.type, disguise, partycol, o, currentposin, numk, squares, numj, (coded % 100)/10)) {
                        repre = copyrepre; pieces = copypieces;
                        return true;
                    } repre = copyrepre; pieces = copypieces;
                } maskmoves.clear();
            }
        }
    } return false;
}

std::vector<std::string> frontlets = {"K", "Q", "B", "N", "R", ""};
std::string notation(int score, int movetype, int cellin, int origin, std::vector<int> indexfix, int code,
    std::vector<std::string> letters, int transform, std::vector<int> opcheck, int opkingpos, int col, std::vector<cell> squares) {
    std::string string;
    if (movetype == 2) {string = (cellin - origin == 2) ? "O-O" : "O-O-O";}
    else {
        string = frontlets[indexfix[score]];
        if (score != 1 && score != 8) {string = std::to_string(code / 100) + string;} // Custom Disambiguation
        if (movetype == 1 || movetype == 5) {
            if (score == 1) {string += intono(origin, letters)[0];}
            string += "x";
        } string += intono(cellin, letters);
        if (movetype == 4 || movetype == 5) {
            string += "=" + frontlets[transform];
        }
    } bool Movailable = movailable(1-col, opkingpos, opcheck, squares);
    if (opcheck.size() > 0) {if (Movailable) {string += "+";} else {string += "#";}} else if (!Movailable) {string += " 1/2-1/2";}
    return string;
}

int main() {
    InitWindow(640, 640, "Chess v2.0.1");
    SetTargetFPS(60);

    std::vector<std::string> letters = {"a", "b", "c", "d", "e", "f", "g", "h"}, types = {"white", "black"}, roles = {"king", "queen", "bishop",
        "knight", "rook", "pawn"}; std::vector<int> indexfix = {null, 5, null, 2, 3, 4, null, null, 0, 1}, promovec = {9, 3, 4, 5};

    std::vector<cell> squares;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cell square({x * 80.f, y * 80.f});
            if ((x + y) % 2 != 0) {square.color = (Color){118, 150, 86, 255};}
            squares.push_back(square);
        }
    }

    std::vector<Texture2D> textures;
    for (auto const& type : types) {
        for (auto const& role : roles) {
            std::string filepath = "C:/test_sfml/" + type + role + ".png";
            Texture2D tex = LoadTexture(filepath.c_str());
            textures.push_back(tex);
        }
    }

    for (int idx = 0; idx < 64; idx++) {
        int nbr = repre[idx];
        if (nbr == null) {continue;}
        repre[idx] = pieces.size();
        piece newpiece(squares[idx].position, textures[(nbr % 10) * 6 + indexfix[(nbr % 100) / 10]], nbr);
        pieces.push_back(newpiece);
    }

    std::cout << "\nWhite | Black\n";
    std::vector<move> legalmoves;
    int selected = null, rec = null, promoposin = null, promocol = null, movetype = null; Color defcol = WHITE;
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int cellin = cotoin(GetMousePosition()), piecein = repre[cellin];
            if (promoposin != null) {
                int num = cellin - 34;
                if (num > -1 && num < 4) {
                    int Ni = 3, score = promovec[num], r = score*10 + promocol;
                    for (auto const& Piece : pieces) {
                        int nsc = Piece.code;
                        if (nsc % 100 == r) {int u = nsc / 100; if (u > Ni) {Ni = u+1;}}
                    } int earliercode = pieces[repre[promoposin]].code;
                    pieces[repre[promoposin]] = piece(intoco(promoposin), textures[promocol*10 + indexfix[score]], Ni*100 + r);
                    promoposin = null;
                    int opkingposin = null;
                    for (int idx = 0; idx < 64; idx++) {
                        int N = repre[idx];
                        if (N == null) {continue;}
                        else if (pieces[N].code == 181 - promocol) {opkingposin = idx; break;}
                    } std::cout << notation(1, movetype, cellin, promoposin, indexfix, earliercode,
                        letters, null, check(opkingposin, 1 - promocol), opkingposin, promocol, squares);
                    if (promocol == 1) {std::cout << std::endl;} else {std::cout << "     ";}
                }
            } if (selected == null && promoposin == null) { // Selection
                if (piecein != null) {
                    selected = piecein;
                    defcol = squares[cellin].color; rec = cellin;
                    squares[cellin].color = (Color){186, 202, 68, 255};
                }
            } else if (selected != null && promoposin == null) {
                int CODE = pieces[selected].code, col = CODE % 10, Score = (CODE % 100) / 10, kingposin = null; movetype = null;
                bool done = false;
                for (int idx = 0; idx < 64; idx++) {
                    int N = repre[idx];
                    if (N == null) {continue;}
                    else if (pieces[N].code == 180 + col) {kingposin = idx; break;}
                } legalmoves.clear(); int selectedpos = cotoin(pieces[selected].position);
                moves(selected, legalmoves, selectedpos, CODE, col);
                for (auto& Piece : pieces) {Piece.en = false;}
                for (int idx = 0; idx < legalmoves.size(); idx++) {
                    move Move = legalmoves[idx];
                    if (Move.movein == cellin) {movetype = Move.type; break;}
                } if (movetype != null) {
                    auto copyrepre = repre; auto copypieces = pieces;
                    if (piecein == null) { // Move
                        if (notlegal(movetype, kingposin, col, cellin, selectedpos, selected, squares, piecein, Score)) {
                            repre = copyrepre; pieces = copypieces;
                        } else {
                            if (Score == 1 && std::abs(cellin - selectedpos) == 16) {pieces[selected].en = true;}
                            if (movetype == 4) {promoposin = cellin; promocol = col;}
                            done = true;
                        }
                    } else { // Capture
                        if (notlegal(movetype, kingposin, col, cellin, selectedpos, selected, squares, piecein, Score)) {
                            repre = copyrepre; pieces = copypieces;
                        } else {
                            if (movetype == 5) {promoposin = cellin; promocol = col;}
                            done = true;
                        }
                    } if (done && promoposin == null) {
                        pieces[selected].moved = true;
                        int opkingposin = null;
                        for (int idx = 0; idx < 64; idx++) {
                            int N = repre[idx];
                            if (N == null) {continue;}
                            else if (pieces[N].code == 181 - col) {opkingposin = idx; break;}
                        } std::cout << notation(Score, movetype, cellin, selectedpos, indexfix, CODE,
                            letters, null, check(opkingposin, 1 - col), opkingposin, col, squares);
                        if (col == 1) {std::cout << std::endl;} else {std::cout << "     ";}
                    }
                } selected = null;
                squares[rec].color = defcol;
                rec = null;
            }
        } BeginDrawing();
            for (auto const& square : squares) { // Board generation
                DrawRectangleV(square.position, {80.f, 80.f}, square.color);
            } for (auto const& Piece : pieces) { // Pieces
                Vector2 pos = Piece.position;
                DrawTexturePro(Piece.texture, {0.f, 0.f, 10.f, 10.f}, {pos.x, pos.y, 80.f, 80.f}, {0.f, 0.f}, 0.f, WHITE);
            } for (int num = 0; num < 8; num++) { // Labeling
                int num0 = num + 1, num1 = 600 - (num * 80), num2 = num*80 + 40;
                std::string letter = letters[num];
                DrawText(TextFormat("%i", num0), 2, num1, 12, BLACK);
                DrawText(TextFormat("%i", num0), 626, num1, 12, BLACK);
                DrawText(letter.c_str(), num2, 2, 12, BLACK);
                DrawText(letter.c_str(), num2, 626, 12, BLACK);
            } if (promoposin != null) {
                DrawRectangle(160, 320, 320, 80, WHITE);
                DrawRectangleLines(160, 320, 320, 80, BLACK);
                for (int index = 1; index < 5; index++) {
                    DrawTexturePro(textures[promocol*6 + index], {0.f, 0.f, 10.f, 10.f}, 
                        {index*80.f + 80.f, 320.f, 80.f, 80.f}, {0.f, 0.f}, 0.f, WHITE);
                }
            }
        EndDrawing();
    }

    for (auto& texture : textures) {
        UnloadTexture(texture);
    } CloseWindow();
    return 0;
}