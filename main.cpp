/*
g++ -Wall -o main main.cpp \
  -I/usr/X11/include -L/usr/X11/lib -lX11 -lGL -lpng -lpthread -std=c++17
*/

#include <chrono>
#include <thread>
#include <time.h>
#include <stdlib.h>
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

    int nCurrentPiece = rand() % 7; // nCurrentPiece: in array of tetromino pieces, this is the index value of the current piece
    int nCurrentRotation = 0;       // nCurrentRotation: angle that piece is currently rotated
    int nCurrentX = nFieldWidth/2;  // nCurrentX: Top left corner of the current piece
    int nCurrentY = 0;              // nCurrentY: Top left corner of the current piece
    int nSpeed = 20;                // nSpeed: Current level/difficulty of the game
    int nSpeedCount = 0;            // nSpeedCount: When the number of game ticks equals this value, we want to force the piece down
    bool bForceDown = false;        // bForceDown: Flag to check if we're going to force the piece down
    bool bRotateHold = true;        // bRotateHold: Flag to check if the player is holding down the rotate key.
    std::vector<int> vLines;        // vLines: 
    bool bGameOver = false;         // bGameOver: 
    int nPieceCount = 0;            // nPieceCount: Keeps track of how many pieces have been delivered to the player
                                    // Every 10 pieces, we'll increase the speed


protected:

    // ON USER CREATE
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

        // if (bGameOver)
        //     std::cout << "Game Over in OnUserCreate" << std::endl;
        //     return true;

    }

    // ON USER UPDATE
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

        // When bForceDown == true, force the piece down until it can't be forced down
        if (bForceDown) {

            //Update difficulty every 50 pieces
            nSpeedCount = 0;  
            nPieceCount++;
            if (nPieceCount % 50 == 0)
                if (nSpeed >= 10)
                    nSpeed--;

            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY+1)) {
                nCurrentY++;

            } else {

                // Piece can no longer be forced down. Then, lock piece in place.
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != '.')
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                
                // Check for lines
                // Only need to check for where the last piece was placed. For loop first goes down the cols of the asset space
                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < nFieldHeight - 1) { // So long as we're in bounds
                        bool bLine = true;  // Start by assuming that there is a line

                        for (int px = 1; px < nFieldWidth - 1; px++)
                            bLine &= (pField[(nCurrentY+py) * nFieldWidth + px]) != 0; // bLine set to false if there is an empty space

                        if (bLine) {
                            for (int px = 1; px < nFieldWidth - 1; px++)
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            vLines.push_back(nCurrentY + py); 
                        }
                    }

                nScore += 25;
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

                // Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // If piece does not fit, game over
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
        }

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
                    // If space isn't empty, CGE_Draw() draws correct ASCII char at appropriate X and Y
                    // X arg = nCurrentX (updated by user input) + px (place in asset space) + an offset of 2
                    // Y arg = nCurrentY (updated by user input) + py (place in asset space) + an offset of 2
                    CGE_Draw( nCurrentX + px + 2, nCurrentY + py + 2, nCurrentPiece + 65 );

        // Draw score
        std::string sScore = "SCORE: " + std::to_string( nScore );
        DrawString( (nFieldWidth + 6) * 8, 2 * 8, sScore );

        // Use vLines to check for lines
        if (!vLines.empty()) {
            for (auto &v : vLines)
                for (int px = 1; px < nFieldWidth - 1; px++) {
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py-1) * nFieldWidth + px];
                    pField[px] = 0;
                }
            vLines.clear();
        }

        // if (bGameOver)
        //     std::cout << "Game Over in OnUserUpdate" << std::endl;
        //     return true;


    }

};

int main() {
    // inits random seed
    srand( time( NULL ) );

    // use olcConcolseGameEngine derived app
    Tetris_PGE game;
    game.Construct(nScreenWidth * 8, nScreenHeight * 8, PIX_X, PIX_Y);
    game.Start();

    // oh dear - game over!
    std::cout << "Game Over!! Score: " << nScore << std::endl;
    system("pause");

    return 0;
    
}
