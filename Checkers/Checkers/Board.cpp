#include "Board.h"
#include <iostream>

Board::Board() {
    InitializeBoard();
}

Board::Board(const Board& other) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = other.board[i][j];
        }
    }
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = other.board[i][j];
            }
        }
    }
    return *this;
}

void Board::InitializeBoard() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = EMPTY;
        }
    }

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 8; x++) {
            if ((x + y) % 2 == 1) {
                board[x][y] = BLACK_PIECE;
            }
        }
    }

    for (int y = 5; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((x + y) % 2 == 1) {
                board[x][y] = WHITE_PIECE;
            }
        }
    }
}

PieceType Board::GetPiece(int x, int y) const {
    if (!IsValidPosition(x, y)) return EMPTY;
    return board[x][y];
}

void Board::SetPiece(int x, int y, PieceType piece) {
    if (IsValidPosition(x, y)) {
        board[x][y] = piece;
    }
}

bool Board::IsValidPosition(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::IsPlayerPiece(int x, int y, int player) const {
    PieceType piece = GetPiece(x, y);
    if (player == 1) {
        return piece == WHITE_PIECE || piece == WHITE_KING;
    }
    else { 
        return piece == BLACK_PIECE || piece == BLACK_KING;
    }
}

std::vector<Move> Board::GetPossibleMoves(int player) const {
    std::vector<Move> allMoves;
    std::vector<Move> captureMoves;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (IsPlayerPiece(x, y, player)) {
                std::vector<Move> pieceMoves = GetPossibleMovesForPiece(x, y);

                for (const auto& move : pieceMoves) {
                    if (!move.captures.empty()) {
                        captureMoves.push_back(move);
                    }
                    else {
                        allMoves.push_back(move);
                    }
                }
            }
        }
    }

    if (!captureMoves.empty()) {
        return captureMoves;
    }

    return allMoves;
}

std::vector<Move> Board::GetPossibleMovesForPiece(int x, int y) const {
    std::vector<Move> moves;
    PieceType piece = GetPiece(x, y);

    if (piece == EMPTY) return moves;

    GetCaptureMoves(x, y, moves);

    if (moves.empty()) {
        GetRegularMoves(x, y, moves);
    }

    return moves;
}

void Board::GetCaptureMoves(int x, int y, std::vector<Move>& moves) const {
    PieceType piece = GetPiece(x, y);

    if (IsKing(piece)) {
        GetKingCaptures(x, y, moves);
    }
    else {
        int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };

        for (int i = 0; i < 4; i++) {
            int dx = directions[i][0];
            int dy = directions[i][1];

            Move move(x, y, 0, 0);
            if (CanCapture(x, y, dx, dy, move)) {
                FindMultipleCaptures(move.toX, move.toY, move, moves);
                if (move.captures.size() == 1) {
                    moves.push_back(move);
                }
            }
        }
    }
}

void Board::GetRegularMoves(int x, int y, std::vector<Move>& moves) const {
    PieceType piece = GetPiece(x, y);

    if (IsKing(piece)) {
        GetKingMoves(x, y, moves);
    }
    else {
        int dy = IsWhite(piece) ? -1 : 1;

        for (int dx = -1; dx <= 1; dx += 2) {
            int newX = x + dx;
            int newY = y + dy;

            if (IsValidPosition(newX, newY) && GetPiece(newX, newY) == EMPTY) {
                moves.push_back(Move(x, y, newX, newY));
            }
        }
    }
}

void Board::GetKingMoves(int x, int y, std::vector<Move>& moves) const {
    int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];

        for (int dist = 1; dist < 8; dist++) {
            int newX = x + dx * dist;
            int newY = y + dy * dist;

            if (!IsValidPosition(newX, newY)) break;

            if (GetPiece(newX, newY) != EMPTY) break;

            moves.push_back(Move(x, y, newX, newY));
        }
    }
}

void Board::GetKingCaptures(int x, int y, std::vector<Move>& moves) const {
    int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];

        for (int dist = 1; dist < 8; dist++) {
            int checkX = x + dx * dist;
            int checkY = y + dy * dist;

            if (!IsValidPosition(checkX, checkY)) break;

            PieceType targetPiece = GetPiece(checkX, checkY);
            if (targetPiece == EMPTY) continue;

            bool isEnemy = (IsWhite(GetPiece(x, y)) && IsBlack(targetPiece)) ||
                (IsBlack(GetPiece(x, y)) && IsWhite(targetPiece));

            if (isEnemy) {
                for (int landDist = 1; landDist < 8; landDist++) {
                    int landX = checkX + dx * landDist;
                    int landY = checkY + dy * landDist;

                    if (!IsValidPosition(landX, landY)) break;
                    if (GetPiece(landX, landY) != EMPTY) break;

                    Move move(x, y, landX, landY);
                    move.captures.push_back({ checkX, checkY });
                    moves.push_back(move);
                }
            }
            break;
        }
    }
}

bool Board::CanCapture(int x, int y, int dx, int dy, Move& move) const {
    int enemyX = x + dx;
    int enemyY = y + dy;
    int landX = x + dx * 2;
    int landY = y + dy * 2;

    if (!IsValidPosition(enemyX, enemyY) || !IsValidPosition(landX, landY)) {
        return false;
    }

    PieceType myPiece = GetPiece(x, y);
    PieceType enemyPiece = GetPiece(enemyX, enemyY);
    PieceType landPiece = GetPiece(landX, landY);

    bool isEnemy = (IsWhite(myPiece) && IsBlack(enemyPiece)) ||
        (IsBlack(myPiece) && IsWhite(enemyPiece));

    if (isEnemy && landPiece == EMPTY) {
        move.toX = landX;
        move.toY = landY;
        move.captures.push_back({ enemyX, enemyY });
        return true;
    }

    return false;
}

void Board::FindMultipleCaptures(int x, int y, Move& currentMove, std::vector<Move>& allMoves) const {
    Board tempBoard = *this;

    for (const auto& capture : currentMove.captures) {
        tempBoard.SetPiece(capture.first, capture.second, EMPTY);
    }
    tempBoard.SetPiece(currentMove.fromX, currentMove.fromY, EMPTY);
    tempBoard.SetPiece(x, y, GetPiece(currentMove.fromX, currentMove.fromY));

    bool foundAdditionalCapture = false;
    int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];

        Move additionalMove(x, y, 0, 0);
        if (tempBoard.CanCapture(x, y, dx, dy, additionalMove)) {
            foundAdditionalCapture = true;

            Move newMove = currentMove;
            newMove.toX = additionalMove.toX;
            newMove.toY = additionalMove.toY;
            newMove.captures.insert(newMove.captures.end(),
                additionalMove.captures.begin(),
                additionalMove.captures.end());

            tempBoard.FindMultipleCaptures(additionalMove.toX, additionalMove.toY, newMove, allMoves);
        }
    }

    if (!foundAdditionalCapture) {
        allMoves.push_back(currentMove);
    }
}

bool Board::MakeMove(const Move& move) {
    PieceType piece = GetPiece(move.fromX, move.fromY);
    if (piece == EMPTY) return false;

    for (const auto& capture : move.captures) {
        SetPiece(capture.first, capture.second, EMPTY);
    }

    SetPiece(move.fromX, move.fromY, EMPTY);
    SetPiece(move.toX, move.toY, piece);

    if (ShouldPromoteToKing(move.toX, move.toY, piece)) {
        if (piece == WHITE_PIECE) {
            SetPiece(move.toX, move.toY, WHITE_KING);
        }
        else if (piece == BLACK_PIECE) {
            SetPiece(move.toX, move.toY, BLACK_KING);
        }
    }

    return true;
}

void Board::UndoMove(const Move& move, const std::vector<PieceType>& capturedPieces) {
    PieceType piece = GetPiece(move.toX, move.toY);

    SetPiece(move.toX, move.toY, EMPTY);
    SetPiece(move.fromX, move.fromY, piece);

    for (size_t i = 0; i < move.captures.size() && i < capturedPieces.size(); i++) {
        SetPiece(move.captures[i].first, move.captures[i].second, capturedPieces[i]);
    }
}

bool Board::ShouldPromoteToKing(int x, int y, PieceType piece) const {
    if (IsKing(piece)) return false;

    if (piece == WHITE_PIECE && y == 0) return true;
    if (piece == BLACK_PIECE && y == 7) return true;

    return false;
}

bool Board::IsKing(PieceType piece) const {
    return piece == WHITE_KING || piece == BLACK_KING;
}

bool Board::IsWhite(PieceType piece) const {
    return piece == WHITE_PIECE || piece == WHITE_KING;
}

bool Board::IsBlack(PieceType piece) const {
    return piece == BLACK_PIECE || piece == BLACK_KING;
}

int Board::CountPieces(int player) const {
    int count = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (IsPlayerPiece(x, y, player)) {
                count++;
            }
        }
    }
    return count;
}

bool Board::HasMoves(int player) const {
    return !GetPossibleMoves(player).empty();
}

void Board::PrintBoard() const {
    std::cout << "  0 1 2 3 4 5 6 7\n";
    for (int y = 0; y < 8; y++) {
        std::cout << y << " ";
        for (int x = 0; x < 8; x++) {
            char symbol = '.';
            switch (GetPiece(x, y)) {
            case WHITE_PIECE: symbol = 'w'; break;
            case BLACK_PIECE: symbol = 'b'; break;
            case WHITE_KING: symbol = 'W'; break;
            case BLACK_KING: symbol = 'B'; break;
            default: symbol = '.'; break;
            }
            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}