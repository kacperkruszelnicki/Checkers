#ifndef BOARD_H
#define BOARD_H

#include <vector>

enum PieceType {
    EMPTY = 0,
    WHITE_PIECE = 1,
    BLACK_PIECE = 2,
    WHITE_KING = 3,
    BLACK_KING = 4
};

struct Move {
    int fromX, fromY;
    int toX, toY;
    std::vector<std::pair<int, int>> captures;
    bool isKingMove;

    Move(int fx, int fy, int tx, int ty) :
        fromX(fx), fromY(fy), toX(tx), toY(ty), isKingMove(false) {}
};

class Board {
public:
    Board();

    void InitializeBoard();
    PieceType GetPiece(int x, int y) const;
    void SetPiece(int x, int y, PieceType piece);

    std::vector<Move> GetPossibleMoves(int player) const;
    std::vector<Move> GetPossibleMovesForPiece(int x, int y) const;

    bool MakeMove(const Move& move);
    void UndoMove(const Move& move, const std::vector<PieceType>& capturedPieces);

    bool IsValidPosition(int x, int y) const;
    bool IsPlayerPiece(int x, int y, int player) const;

    int CountPieces(int player) const;
    bool HasMoves(int player) const;

    void PrintBoard() const;

    Board(const Board& other);
    Board& operator=(const Board& other);

private:
    PieceType board[8][8];

    void GetCaptureMoves(int x, int y, std::vector<Move>& moves) const;
    void GetRegularMoves(int x, int y, std::vector<Move>& moves) const;
    void GetKingMoves(int x, int y, std::vector<Move>& moves) const;
    void GetKingCaptures(int x, int y, std::vector<Move>& moves) const;

    bool CanCapture(int x, int y, int dx, int dy, Move& move) const;
    void FindMultipleCaptures(int x, int y, Move& currentMove, std::vector<Move>& allMoves) const;

    bool ShouldPromoteToKing(int x, int y, PieceType piece) const;
    bool IsKing(PieceType piece) const;
    bool IsWhite(PieceType piece) const;
    bool IsBlack(PieceType piece) const;
};

#endif
