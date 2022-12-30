#include <iostream>
using namespace std;

// Wide strings are similar to strings except they store more data, so that unicode characters can be used in the string
// Tetrominos are the shapes used in tetris
wstring tetromino[7];

// Rotate: Simulates rotating a tetromino piece using a 1d tetromino array. px: original x value of elem, py: original y value of elem, r: degrees of rotation
// Preconditions:
// Postconditions:
int Rotate(int px, int py, int r) {
    
    /*
    Equations for tetromino rotation are as follows
     
    0)   i = y * w + x
    90)  i = 12 + y - (x * w)
    180) i = 15 - (y * w) - x
    270) i = 3 - y + (x * w)
    */
    
    // here, w = 4
    switch (r % 4) {
        case 0:
            return py * 4 + px; // 0 degrees
        case 1:
            return 12 + py - (px * 4); // 90 degrees
        case 2:
            return 15 - (py * 4) - px; // 180 degrees
        case 3:
            return 3 - py + (px * 4); // 270 degrees
    }
    return -1;
}

int main() {
    
    // Create assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    
    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");
    
    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    
    return 0;
    
}
