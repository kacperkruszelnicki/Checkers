#include "AI.h"
#include <algorithm>
#include <climits>

AI::AI() {
}

Move AI::GetBestMove(const Board& board, int player, int depth) {
    std::vector<Move> possibleMoves = board.GetPossibleMoves(player);

    if (possibleMoves.empty()) {
        return Move(0, 0, 0, 0);
    }

    Move bestMove = possibleMoves[0];
    int bestScore = INT_MIN;

    for (const auto& move : possibleMoves) {
        Board tempBoard = board;

        std::vector<PieceType> capturedPieces;
        for (const auto& capture : move.captures) {
            capturedPieces.push_back(tempBoard.GetPiece(capture.first, capture.second));
        }

        tempBoard.MakeMove(move);

        int score = minimax(tempBoard, depth - 1, INT_MIN, INT_MAX, false, player);

        tempBoard.UndoMove(move, capturedPieces);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}


int AI::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, int player) {
    if (depth == 0) {
        return EvaluateBoard(board, player);
    }

    int currentPlayer = maximizingPlayer ? player : (player == 1 ? 2 : 1);
    std::vector<Move> moves = board.GetPossibleMoves(currentPlayer);

    if (moves.empty()) {
        if (maximizingPlayer) {
            return INT_MIN + depth;
        }
        else {
            return INT_MAX - depth;
        }
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;

        for (const auto& move : moves) { 
            std::vector<PieceType> capturedPieces;
            for (const auto& capture : move.captures) {
                capturedPieces.push_back(board.GetPiece(capture.first, capture.second));
            }

            board.MakeMove(move);

            int eval = minimax(board, depth - 1, alpha, beta, false, player);

            board.UndoMove(move, capturedPieces);

            maxEval = std::max(maxEval, eval);

            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    }
    else {
        int minEval = INT_MAX;

        for (const auto& move : moves) { 
            std::vector<PieceType> capturedPieces;
            for (const auto& capture : move.captures) {
                capturedPieces.push_back(board.GetPiece(capture.first, capture.second));
            }

            board.MakeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true, player);

            board.UndoMove(move, capturedPieces);

            minEval = std::min(minEval, eval);

            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

int AI::EvaluateBoard(const Board& board, int player) const {
    int score = 0;
    int opponent = (player == 1) ? 2 : 1;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            PieceType piece = board.GetPiece(x, y);
            if (piece != EMPTY) {
                int pieceValue = (piece == WHITE_PIECE || piece == BLACK_PIECE) ? PIECE_VALUE : KING_VALUE;
                int positionValue = GetPositionValue(x, y, piece);
                if ((player == 1 && (piece == WHITE_PIECE || piece == WHITE_KING)) ||
                    (player == 2 && (piece == BLACK_PIECE || piece == BLACK_KING))) {
                    score += pieceValue + positionValue;
                }
                else {
                    score -= pieceValue + positionValue;
                }
            }
        }
    }

    int playerMobility = CountMobility(board, player);
    int opponentMobility = CountMobility(board, opponent);
    score += (playerMobility - opponentMobility) * MOBILITY_BONUS;
    int capturePotential = 0;
    score += capturePotential * 50;

    return score;
}

int AI::GetPositionValue(int x, int y, PieceType piece) const {
    int value = 0;

    int centerDistance = std::min(std::abs(x - 3.5), std::abs(y - 3.5));
    if (centerDistance < 2) {
        value += CENTER_BONUS;
    }

    if (x == 0 || x == 7 || y == 0 || y == 7) {
        value += EDGE_PENALTY;
    }

    if (piece == WHITE_PIECE) {
        if (y == 1) value += 50;
        else if (y == 2) value += 30;
        else if (y == 3) value += 20;
        else if (y == 4) value += 10;
    }
    else if (piece == BLACK_PIECE) {
        if (y == 6) value += 50;
        else if (y == 5) value += 30;
        else if (y == 4) value += 20;
        else if (y == 2) value += 10;

    }

    return value;
}

int AI::CountMobility(const Board& board, int player) const {
    return board.GetPossibleMoves(player).size();
}