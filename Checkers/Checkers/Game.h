#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <memory>
#include "Board.h"
#include "AI.h"

class Game {
public:
    Game();
    ~Game();

    bool Init();
    void Run();
    void Cleanup();

private:


    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* whitePieceTexture;
    SDL_Texture* blackPieceTexture;
    SDL_Texture* whiteKingTexture;
    SDL_Texture* blackKingTexture;
    SDL_Texture* possibleMoveTexture;
    SDL_Texture* boardTexture;
    SDL_Texture* winTextTexture;
    SDL_Texture* loseTextTexture;

    Board board;
    AI ai;
    Move animatedMove = Move{ 0,0,0,0 };

    bool isAnimating;
    float animationProgress;
    bool aiMoveInProgress = false; 

    bool isRunning;
    bool gameOver;
    int winner;
    bool isPlayerTurn;

    int selectedX, selectedY;
    bool pieceSelected;
    std::vector<Move> possibleMoves;

  
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 800;
    static const int BOARD_SIZE = 8;
    static const int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;
    static const int PIECE_SIZE = 110;

    bool LoadTextures();
    void HandleEvents();
    void Update();
    void Render();
    void RenderBoard();
    void RenderPieces();
    void RenderPossibleMoves();
    void RenderGameOver();

    void HandleMouseClick(int x, int y);
    bool IsValidSelection(int x, int y);
    void SelectPiece(int x, int y);
    void MovePiece(int x, int y);
    bool IsValidPosition(int x, int y);

    void CheckGameOver();
    void ProcessAIMove();

    SDL_Texture* LoadTexture(const char* path);
};

#endif