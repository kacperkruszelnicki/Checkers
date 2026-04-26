#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr),
whitePieceTexture(nullptr), blackPieceTexture(nullptr),
whiteKingTexture(nullptr), blackKingTexture(nullptr),
possibleMoveTexture(nullptr),
boardTexture(nullptr), isRunning(false), gameOver(false),
winner(0), isPlayerTurn(true), selectedX(-1), selectedY(-1),
pieceSelected(false), isAnimating(false), animationProgress(0.0f), winTextTexture(nullptr), loseTextTexture(nullptr) {
}

Game::~Game() {
    Cleanup();
}

bool Game::Init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Warcaby", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!LoadTextures()) {
        return false;
    }

    board.InitializeBoard();
    isRunning = true;
    return true;
}

bool Game::LoadTextures() {
    whitePieceTexture = LoadTexture("assets/white_piece.png");
    blackPieceTexture = LoadTexture("assets/black_piece.png");
    whiteKingTexture = LoadTexture("assets/white_king.png");
    blackKingTexture = LoadTexture("assets/black_king.png");
    possibleMoveTexture = LoadTexture("assets/gray_piece.png");
    winTextTexture = LoadTexture("assets/win_text.png");
    loseTextTexture = LoadTexture("assets/lose_text.png");

    return (whitePieceTexture && blackPieceTexture && whiteKingTexture && blackKingTexture && possibleMoveTexture && winTextTexture && loseTextTexture);
}

void Game::Run() {
    while (isRunning) {
        HandleEvents();
        Update();
        Render();
        SDL_Delay(16);
    }
}

void Game::HandleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !gameOver && isPlayerTurn) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                float mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                HandleMouseClick((int)mouseX / CELL_SIZE, (int)mouseY / CELL_SIZE);
            }
        }
        else if (e.type == SDL_EVENT_KEY_DOWN && gameOver) {
            if (e.key.key == SDLK_R) {
                board.InitializeBoard();
                gameOver = false;
                winner = 0;
                isPlayerTurn = true;
                pieceSelected = false;
                possibleMoves.clear();
            }
        }
    }
}

void Game::HandleMouseClick(int x, int y) {
    if (!IsValidPosition(x, y)) return;

    if (!pieceSelected) {
        if (IsValidSelection(x, y)) {
            SelectPiece(x, y);
        }
    }
    else {
        if (x == selectedX && y == selectedY) {
            pieceSelected = false;
            possibleMoves.clear();
            return;
        }

        if (IsValidSelection(x, y)) {
            SelectPiece(x, y);
            return;
        }

        MovePiece(x, y);
    }
}

bool Game::IsValidSelection(int x, int y) {
    return board.IsPlayerPiece(x, y, 1);
}

void Game::SelectPiece(int x, int y) {
    selectedX = x;
    selectedY = y;
    pieceSelected = true;
    possibleMoves = board.GetPossibleMovesForPiece(x, y);
}

void Game::MovePiece(int x, int y) {
    for (const auto& move : possibleMoves) {
        if (move.toX == x && move.toY == y) {
            animatedMove = move;
            isAnimating = true;
            animationProgress = 0.0f;
            pieceSelected = false;
            possibleMoves.clear();
            return;
        }
    }
}

bool Game::IsValidPosition(int x, int y) {
    return board.IsValidPosition(x, y);
}

void Game::Update() {
    if (gameOver) return;

    if (isAnimating) {
        animationProgress += 0.05f;
        if (animationProgress >= 1.0f) {
            isAnimating = false;
            board.MakeMove(animatedMove);

            if (aiMoveInProgress) {
                
                aiMoveInProgress = false;
                isPlayerTurn = true;
            }
            else {
               
                isPlayerTurn = false;
            }

            CheckGameOver();
        }
    }

    if (!isPlayerTurn && !isAnimating) {
        ProcessAIMove();
    }
}

void Game::ProcessAIMove() {
    std::vector<Move> aiMoves = board.GetPossibleMoves(2);
    if (!aiMoves.empty()) {
        Move bestMove = ai.GetBestMove(board, 2);
        animatedMove = bestMove;
        isAnimating = true;
        animationProgress = 0.0f;
        aiMoveInProgress = true;
    }
}

void Game::CheckGameOver() {
    int whitePieces = board.CountPieces(1);
    int blackPieces = board.CountPieces(2);
    bool whiteHasMoves = board.HasMoves(1);
    bool blackHasMoves = board.HasMoves(2);

    if (whitePieces == 0 || !whiteHasMoves) {
        gameOver = true;
        winner = 2;
    }
    else if (blackPieces == 0 || !blackHasMoves) {
        gameOver = true;
        winner = 1;
    }
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_RenderClear(renderer);

    RenderBoard();
    RenderPieces();

    if (pieceSelected) {
        RenderPossibleMoves();
    }

    if (gameOver) {
        RenderGameOver();
    }

    SDL_RenderPresent(renderer);
}

void Game::RenderBoard() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            SDL_FRect rect = {
                (float)(x * CELL_SIZE),
                (float)(y * CELL_SIZE),
                (float)CELL_SIZE,
                (float)CELL_SIZE
            };

            if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
            }
            SDL_RenderFillRect(renderer, &rect);

            if (pieceSelected && x == selectedX && y == selectedY) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void Game::RenderPieces() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            PieceType piece = board.GetPiece(x, y);
            if (piece != EMPTY) {
                SDL_FRect destRect;

                if (isAnimating && animatedMove.fromX == x && animatedMove.fromY == y) {
                    float startX = animatedMove.fromX * CELL_SIZE;
                    float startY = animatedMove.fromY * CELL_SIZE;
                    float endX = animatedMove.toX * CELL_SIZE;
                    float endY = animatedMove.toY * CELL_SIZE;

                    destRect.x = startX + (endX - startX) * animationProgress + (CELL_SIZE - PIECE_SIZE - 110) / 2;
                    destRect.y = startY + (endY - startY) * animationProgress + (CELL_SIZE - PIECE_SIZE) / 2;
                }
                else {
                    destRect.x = (float)(x * CELL_SIZE + (CELL_SIZE - PIECE_SIZE - 110) / 2);
                    destRect.y = (float)(y * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2);
                }

                destRect.w = (float)PIECE_SIZE + 110;
                destRect.h = (float)PIECE_SIZE;

                SDL_Texture* texture = nullptr;
                switch (piece) {
                case WHITE_PIECE:
                    texture = whitePieceTexture;
                    break;
                case BLACK_PIECE:
                    texture = blackPieceTexture;
                    break;
                case WHITE_KING:
                    texture = whiteKingTexture;
                    break;
                case BLACK_KING:
                    texture = blackKingTexture;
                    break;
                }

                if (texture) {
                    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
                }
            }
        }
    }
}

void Game::RenderPossibleMoves() {
    for (const auto& move : possibleMoves) {
        SDL_FRect destRect = {
            (float)(move.toX * CELL_SIZE + (CELL_SIZE - PIECE_SIZE  - 100) / 2),
            (float)(move.toY * CELL_SIZE + (CELL_SIZE - PIECE_SIZE) / 2),
            (float)PIECE_SIZE + 160,
            (float)PIECE_SIZE
        };

        destRect.x += PIECE_SIZE * 0.2f;
        destRect.y += PIECE_SIZE * 0.2f;
        destRect.w *= 0.6f;
        destRect.h *= 0.6f;

        SDL_RenderTexture(renderer, possibleMoveTexture, nullptr, &destRect);
    }
}

void Game::RenderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect overlay = { 0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_FRect messageRect = {
        WINDOW_WIDTH / 6.0f, 
        WINDOW_HEIGHT / 4.0f,
        WINDOW_WIDTH * 2.0f / 3.0f, 
        WINDOW_HEIGHT / 2.0f  
    };
    SDL_RenderFillRect(renderer, &messageRect);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderRect(renderer, &messageRect);
    SDL_Texture* textTexture = nullptr;
    if (winner == 1) {
        textTexture = winTextTexture;
    }
    else if (winner == 2) {
        textTexture = loseTextTexture;
    }

    if (textTexture) {
        float textWidth, textHeight;
        SDL_GetTextureSize(textTexture, &textWidth, &textHeight);

        float scale = 1.0f;
        float maxWidth = messageRect.w * 0.9f;
        float maxHeight = messageRect.h * 0.75f;

        if (textWidth > maxWidth) {
            scale = maxWidth / textWidth;
        }
        if (textHeight * scale > maxHeight) {
            scale = maxHeight / textHeight;
        }

        if (scale < 0.5f) scale = 0.5f;

        float scaledWidth = textWidth * scale;
        float scaledHeight = textHeight * scale;

        SDL_FRect textRect = {
            messageRect.x + (messageRect.w - scaledWidth) / 2,
            messageRect.y + 30,
            scaledWidth,
            scaledHeight
        };

        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_FRect restartRect = {
        messageRect.x + 30,
        messageRect.y + messageRect.h - 50,
        messageRect.w - 60,
        35
    };
    SDL_RenderFillRect(renderer, &restartRect);
}

void Game::Cleanup() {
    if (whitePieceTexture) SDL_DestroyTexture(whitePieceTexture);
    if (blackPieceTexture) SDL_DestroyTexture(blackPieceTexture);
    if (whiteKingTexture) SDL_DestroyTexture(whiteKingTexture);
    if (blackKingTexture) SDL_DestroyTexture(blackKingTexture);
    if (possibleMoveTexture) SDL_DestroyTexture(possibleMoveTexture);
    if (winTextTexture) SDL_DestroyTexture(winTextTexture);
    if (loseTextTexture) SDL_DestroyTexture(loseTextTexture);
    if (boardTexture) SDL_DestroyTexture(boardTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    

    SDL_Quit();
}

SDL_Texture* Game::LoadTexture(const char* path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << SDL_GetError() << std::endl;
    }
    else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_DestroySurface(loadedSurface);
    }
    return newTexture;
}