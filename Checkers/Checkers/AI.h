#ifndef AI_H
#define AI_H

#include "Board.h"
#include <limits>

class AI {
public:
    AI();

    Move GetBestMove(const Board& board, int player, int depth = 6);

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, int player);
    int EvaluateBoard(const Board& board, int player) const;


    static const int PIECE_VALUE = 100;
    static const int KING_VALUE = 300;
    static const int POSITION_BONUS = 5;
    static const int CENTER_BONUS = 10;
    static const int EDGE_PENALTY = -5;
    static const int MOBILITY_BONUS = 2;

    int GetPositionValue(int x, int y, PieceType piece) const;
    int CountMobility(const Board& board, int player) const;
};

#endif
