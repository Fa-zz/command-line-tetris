/*
g++ -o main main.cpp \
  -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread -std=c++17
*/

#include <thread>
#include <iostream> // TODO: Remove both include iostream and using std

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PIX_X 2
#define PIX_Y 2

int nScreenWidth = 80;
int nScreenHeight = 30;

int nScore = 0;

class Tetris_PGE : public olc::PixelGameEngine {
public:
    Tetris_PGE() {
        sAppName = "Tetris_PGE";
    }

private:
    std::string tetromino[7];   // stores tetris objects

    // nFieldWidth, nFieldHeight: defines the game width and height respectively
    // pField: Array of unsigned chars that stores the elems of the field.
    int nFieldWidth = 12;
    int nFieldHeight = 18;
    unsigned char *pField = nullptr;

    // Rotate: Determines the position of the pieces of the tetromino depending on what rotation it's in
    int Rotate(int px, int py, int r) {
        /*
        Equations for tetromino rotation are as follows
        
        0)   i = y * w + x
        90)  i = 12 + y - (x * w)
        180) i = 15 - (y * w) - x
        270) i = 3 - y + (x * w)
        */
        
        // here, w = 4

        int pi = 0;
        switch (r % 4) {
            case 0: // 0 degrees
                pi = py * 4 + px;
                break;
            case 1: // 90 degrees
                pi = 12 + py - (px * 4);
                break;
            case 2: // 180 degrees
                pi = 15 - (py * 4) - px;
                break;
            case 3: // 270 degrees
                pi = 3 - py + (px * 4);
                break;
        }

        return pi;
    }

    // DoesPieceFit: 
    bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {

        // px, py: Each tetromino is 4x4. This nested for loop iterates over the space each tetromino occupies.
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++) {
                
                // At each cell of the tetromino, obtain the piece index
                int pi = Rotate(px, py, nRotation);

                // Get index into field
                int fi = (nPosY + py) * nFieldWidth + (nPosX + px); // same formula as y * w + x

                // The following two if statements check to make sure the space to be collision checked is in bounds
                if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                    if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                        // In bounds so do collision check
                        if (tetromino[nTetromino][pi] != '.' && pField[fi] != 0)
                            return false; // fail on first hit
                    }
                }
            }

        return true;
    }

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth/2;
    int nCurrentY = 0;

protected:

    // OnUserCreate
    virtual bool OnUserCreate() {

        // Create assets - 4x4 tetronimos
        tetromino[0].append("..X.");
        tetromino[0].append("..X.");
        tetromino[0].append("..X.");
        tetromino[0].append("..X.");
        
        tetromino[1].append("..X.");
        tetromino[1].append(".XX.");
        tetromino[1].append(".X..");
        tetromino[1].append("....");

        tetromino[2].append(".X..");
        tetromino[2].append(".XX.");
        tetromino[2].append("..X.");
        tetromino[2].append("....");

        tetromino[3].append("....");
        tetromino[3].append(".XX.");
        tetromino[3].append(".XX.");
        tetromino[3].append("....");

        tetromino[4].append("..X.");
        tetromino[4].append(".XX.");
        tetromino[4].append("..X.");
        tetromino[4].append("....");
        
        tetromino[5].append("....");
        tetromino[5].append(".XX.");
        tetromino[5].append("..X.");
        tetromino[5].append("..X.");

        tetromino[6].append("....");
        tetromino[6].append(".XX.");
        tetromino[6].append(".X..");
        tetromino[6].append(".X..");

        // initialize the playing field
        pField = new unsigned char[nFieldWidth*nFieldHeight]; // creates play field of w*h dimensions
        for (int x = 0; x < nFieldWidth; x++) { // board boundary
            for (int y = 0; y < nFieldHeight; y++) {
                // y * nFieldWidth + x => we use this formula because (TODO: add explanation)
                // border is reached when x is 0 (LHS), x is nFieldWidth-1 (RHS), y is nFieldHeight-1 (Bottom)
                // 9 will represent the border, border is made of 9s everything else is zeros
                pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
            }
        }

    return true;

    }

    // OnUserUpdate

};

int main() {
    
    // use olcConcolseGameEngine derived app
    Tetris_PGE game;
    game.Construct(nScreenWidth * 8, nScreenHeight * 8, PIX_X, PIX_Y);
    game.Start();

    // oh dear - game over!
    std::cout << "Game Over!! Score: " << nScore << std::endl;
    system("pause");

    return 0;
    
}
