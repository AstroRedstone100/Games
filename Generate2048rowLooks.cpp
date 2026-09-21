#include <cstdint>
#include <fstream>
#include <iostream>

uint16_t row_moves[65536];

void compress(uint16_t& num) {
    for (int slot = 4; slot < 13; slot += 4) {
        const auto extracted = 15 & (num >> slot);
        for (int slot2 = 0; slot2 < 9; slot2 += 4) {
            if ((15 & (num >> slot2)) == 0) {
                num |= extracted << slot2;
                num &= ~(15ULL << slot);
                break;
            }
        }
    }
}

void merge(uint16_t& num) {
    auto right = 15 & (num >> 4), left = 15 & (num >> 8);
    if ((right == (15 & num)) && (right != 0)) {
        num++;
        num &= ~(15ULL << 4);
        right = 15 & (num >> 4);
    } if ((right == left) && (right != 0)) {
        num += 16;
        num &= ~(15ULL << 8);
        left = 15 & (num >> 8);
    } if ((left == (15 & (num >> 12))) && (left != 0)) {
        num += 256;
        num &= ~(15ULL << 12);
    }
}

int main() {
    for (int input = 0; input <= UINT16_MAX; input++) {
        uint16_t input1 = input;
        compress(input1); merge(input1); compress(input1);
        row_moves[input] = input1;
    }
    
    std::ofstream out("row_moved.bin", std::ios::binary);
    if (!out) {std::cerr << "\nError 101: File Could Not Be Created"; return -1;}
    out.write(reinterpret_cast<const char*>(row_moves), sizeof(uint16_t) * 65536);
    out.close();
    std::cout << "\nAuthored by C++";

    return 0;
}