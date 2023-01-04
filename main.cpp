/*
g++ -o main main.cpp \
  -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread -std=c++17
*/


#include <chrono>
#include <thread>
#include <iostream> // TODO: Remove both include iostream and using std

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PIX_X 2
#define PIX_Y 2

using namespace std::chrono_literals;

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

    // Rotate: Given the current rotation and the x and y values of asset space, returns the appropriate element at the asset space after factoring in a rotation
    // px: x val of asset space, py: y val of asset space, r: current rotation
    int Rotate(int px, int py, int r) {
    /*
        Equations for tetromino rotation are as follows. Here, w = 4
        
        0)   i = y * w + x
        90)  i = 12 + y - (x * w)
        180) i = 15 - (y * w) - x
        270) i = 3 - y + (x * w)
    */

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

    // DoesPieceFit: Given a tetromino piece and its current rotation, nPosX, nPosY
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

    int nCurrentPiece = 0;          // nCurrentPiece: in array of tetromino pieces, this is the index value of the current piece
    int nCurrentRotation = 0;       // nCurrentRotation: angle that piece is currently rotated
    int nCurrentX = nFieldWidth/2;  // nCurrentX: 
    int nCurrentY = 0;              // nCurrentY: 
    int nSpeed = 20;                // nSpeed: 
    int nSpeedCount = 0;            // nSpeedCount: 
    bool bForceDown = false;        // bForceDown: 
    bool bRotateHold = true;        // bRotateHold: Flag for if the player is holding down the rotate key.
    int nPieceCount = 0;            // nPieceCount: 
    std::vector<int> vLines;        // vLines: 
    bool bGameOver = false;         // bGameOver: 

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

        int flagToReturn = 0;
        if (flagToReturn == 1)
            std::cout << "Flag to return is 1, OnUserCreate" << std::endl;
            return true;

    }

    // OnUserUpdate
    virtual bool OnUserUpdate(float fElapsedTime) {
        // TIMING ============================================

		std::this_thread::sleep_for(50ms); // Small Step = 1 Game Tick
        nSpeedCount++;
        bForceDown = (nSpeedCount == nSpeed);

        // INPUT =============================================

        // Handle player movement
        auto key_touched = [=]( olc::Key nKey ) {
            return (GetKey( nKey ).bPressed || GetKey( nKey ).bHeld);
        };

        if (key_touched( olc::Key::RIGHT ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY )) {
                nCurrentX = nCurrentX + 1;
        }
        if (key_touched( olc::Key::LEFT ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY )) {
                nCurrentX = nCurrentX - 1;
        }
        if (key_touched( olc::Key::DOWN ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1 )) {
                nCurrentY = nCurrentY + 1;
        }

        // Handling rotation
		if (key_touched( olc::Key::Z )) {
			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = false;
		}
		else
			bRotateHold = true;


        // GAME LOGIC ========================================

        // DISPLAY ===========================================

        Clear(olc::BLACK);

        // lambda to emulate CGE's Draw() call
        auto CGE_Draw = [=](int x, int y, char c) {
            DrawString(x * 8, y * 8, std::string(1, c));
        };

        // Draws field
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y <nFieldHeight; y++)
                CGE_Draw(x + 2, y + 2, " ABCDEFG=#"[pField[y*nFieldWidth+x]] );

        // Draw current piece
        for (int px = 0; px < 4; px++)  // These 2 for loops loop over the entire 4x4 space occupied by a piece
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != '.')
                    // If space isn't empty, CGE_Draw() draws correct ASCII piece at appropriate X and Y
                    // X arg = nCurrentX (updated by user input) + px (place in asset space) + an offset of 2
                    // Y arg = nCurrentY (updated by user input) + py (place in asset space) + an offset of 2
                    CGE_Draw( nCurrentX + px + 2, nCurrentY + py + 2, nCurrentPiece + 65 );


        int flagToReturn = 0;
        if (flagToReturn == 1)
            std::cout << "Flag to return is 1, OnUserCreate" << std::endl;
            return true;


    }

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
