#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

enum class PieceType { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING };
enum class PieceColor { WHITE, BLACK, NONE };

struct Piece {
    PieceType type;
    PieceColor color;

    Piece(PieceType t = PieceType::EMPTY, PieceColor c = PieceColor::NONE)
        : type(t), color(c) {}

    string getSymbol() const {
        switch (type) {
            case PieceType::PAWN: return color == PieceColor::WHITE ? "♙" : "♟︎";
            case PieceType::ROOK: return color == PieceColor::WHITE ? "♖" : "♜";
            case PieceType::KNIGHT: return color == PieceColor::WHITE ? "♘" : "♞";
            case PieceType::BISHOP: return color == PieceColor::WHITE ? "♗" : "♝";
            case PieceType::QUEEN: return color == PieceColor::WHITE ? "♕" : "♛";
            case PieceType::KING: return color == PieceColor::WHITE ? "♔" : "♚";
            default: return " ";
        }
    }

    string getColorCode() const {
        if (color == PieceColor::WHITE) return "\033[97m";  // White
        if (color == PieceColor::BLACK) return "\033[30m";  // Black
        return "\033[0m";  // Default color (for empty squares)
    }
};

struct Board {
    Piece board[8][8];

   void printBoard() {
    cout << "  a b c d e f g h\n";
    for (int i = 0; i < 8; ++i) {
        cout << 8 - i << " ";
        for (int j = 0; j < 8; ++j) {
            cout << board[i][j].getSymbol() << " ";
        }
        cout << 8 - i << "\n";
    }
    cout << "  a b c d e f g h\n";
}


    void initialize() {
        // White pieces
        board[0][0] = Piece(PieceType::ROOK, PieceColor::WHITE);
        board[0][1] = Piece(PieceType::KNIGHT, PieceColor::WHITE);
        board[0][2] = Piece(PieceType::BISHOP, PieceColor::WHITE);
        board[0][3] = Piece(PieceType::QUEEN, PieceColor::WHITE);
        board[0][4] = Piece(PieceType::KING, PieceColor::WHITE);
        board[0][5] = Piece(PieceType::BISHOP, PieceColor::WHITE);
        board[0][6] = Piece(PieceType::KNIGHT, PieceColor::WHITE);
        board[0][7] = Piece(PieceType::ROOK, PieceColor::WHITE);
        for (int i = 0; i < 8; i++) board[1][i] = Piece(PieceType::PAWN, PieceColor::WHITE);

        // Black pieces
        board[7][0] = Piece(PieceType::ROOK, PieceColor::BLACK);
        board[7][1] = Piece(PieceType::KNIGHT, PieceColor::BLACK);
        board[7][2] = Piece(PieceType::BISHOP, PieceColor::BLACK);
        board[7][3] = Piece(PieceType::QUEEN, PieceColor::BLACK);
        board[7][4] = Piece(PieceType::KING, PieceColor::BLACK);
        board[7][5] = Piece(PieceType::BISHOP, PieceColor::BLACK);
        board[7][6] = Piece(PieceType::KNIGHT, PieceColor::BLACK);
        board[7][7] = Piece(PieceType::ROOK, PieceColor::BLACK);
        for (int i = 0; i < 8; i++) board[6][i] = Piece(PieceType::PAWN, PieceColor::BLACK);

        // Empty spaces
        for (int i = 2; i < 6; ++i) {
            for (int j = 0; j < 8; ++j) {
                board[i][j] = Piece();
            }
        }
    }
};

struct Move {
    int from_x, from_y, to_x, to_y;

    Move(int fx, int fy, int tx, int ty) : from_x(fx), from_y(fy), to_x(tx), to_y(ty) {}
};

class ChessGame {
private:
    Board board;
    PieceColor currentPlayer;

    bool isValidMove(const Move &m) {
        Piece &fromPiece = board.board[m.from_x][m.from_y];
        Piece &toPiece = board.board[m.to_x][m.to_y];

        // Check if the move is within bounds and valid piece
        if (m.from_x < 0 || m.from_x >= 8 || m.from_y < 0 || m.from_y >= 8 ||
            m.to_x < 0 || m.to_x >= 8 || m.to_y < 0 || m.to_y >= 8) {
            return false;
        }

        // Check if moving an empty square
        if (fromPiece.type == PieceType::EMPTY) {
            return false;
        }

        // Can't move to a square with the same color
        if (fromPiece.color == toPiece.color) {
            return false;
        }

        // Simplified movement rules: check for any piece (no specific movement rules for each type)
        return true;
    }

    void makeRandomMove() {
        vector<Move> possibleMoves;

        // Generate possible moves
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &piece = board.board[i][j];
                if (piece.color == currentPlayer) {
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            Move move(i, j, x, y);
                            if (isValidMove(move)) {
                                possibleMoves.push_back(move);
                            }
                        }
                    }
                }
            }
        }

        if (!possibleMoves.empty()) {
            // Select a random move
            Move move = possibleMoves[rand() % possibleMoves.size()];
            board.board[move.to_x][move.to_y] = board.board[move.from_x][move.from_y];
            board.board[move.from_x][move.from_y] = Piece();  // Empty the from square
        }
    }

    bool isCheck(PieceColor player) {
        // Find the king of the player
        int king_x = -1, king_y = -1;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (board.board[i][j].type == PieceType::KING && board.board[i][j].color == player) {
                    king_x = i;
                    king_y = j;
                    break;
                }
            }
        }

        // Check if any of the opponent's pieces can attack the king
        PieceColor opponent = (player == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &attacker = board.board[i][j];
                if (attacker.color == opponent) {
                    // Here we are simplifying: just check if the attacker can move to the king's square
                    Move move(i, j, king_x, king_y);
                    if (isValidMove(move)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool hasValidMoves(PieceColor player) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Piece &piece = board.board[i][j];
                if (piece.color == player) {
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            Move move(i, j, x, y);
                            if (isValidMove(move)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool isCheckmate(PieceColor player) {
        return isCheck(player) && !hasValidMoves(player);
    }

    bool isStalemate(PieceColor player) {
        return !isCheck(player) && !hasValidMoves(player);
    }

public:
    ChessGame() : currentPlayer(PieceColor::WHITE) {
        srand(time(0));  // Seed for random moves
        board.initialize();
    }

    void playGame() {
        while (true) {
            board.printBoard();
            cout << "Player " << (currentPlayer == PieceColor::WHITE ? "White" : "Black") << "'s move:" << endl;

            // Bot makes a random move
            makeRandomMove();

            // Check for checkmate or stalemate
            if (isCheckmate(currentPlayer)) {
                cout << "Checkmate! " << (currentPlayer == PieceColor::WHITE ? "Black" : "White") << " wins!" << endl;
                break;
            }

            if (isStalemate(currentPlayer)) {
                cout << "Stalemate! It's a draw!" << endl;
                break;
            }

            // Change player
            currentPlayer = (currentPlayer == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

            // Wait for user input to continue (simulate a turn)
            cout << "Press Enter for the next move..." << endl;
            cin.get();
        }
    }
};

int main() {
    ChessGame game;
    game.playGame();
    return 0;
}
