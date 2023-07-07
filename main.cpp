#include <iostream>
#include <string>

/* Label mapping
    0: Empty;
    1: Black;
    2: White;
    3: Black Pending;
    4: White Pending
*/
char Labal_Map[] = ".xo#@";
int Board[8][8];

enum GameMode { Quitting,
                Menu,
                Gaming };
GameMode Game_Mode = Menu;
/* Player Index: 1 = Black, 2 = White*/
int Player_Turn = 1;
/* Message shown during a game
    0: No message;
    1: Invalid command;
    2: Player cannot place here;
    3: Player cannot make valid move, pass the turn to the other
*/
int Msg_In_Game = 0;
bool Show_Placable = false;
struct ReversableLines {
    char u = 0;
    char d = 0;
    char r = 0;
    char l = 0;
    char ul = 0;
    char ur = 0;
    char dl = 0;
    char dr = 0;
};

/* Function Declaration */
void initGame();
void printBoard(bool showPlacable = false);
GameMode printMenuAndHandleInput();
void gameCycle();
ReversableLines getReversableLines(int playerIdx, int row, int col);
bool checkCellPlacable(int playerIdx, int row, int col);
bool placePiece(int playerIdx, int row, int col);
int* getPlacableArr(int playerIdx);
int countPlayerIdx(int playerIdx);

int main() {
    while (Game_Mode != Quitting) {
        switch (Game_Mode) {
            case Menu:
                Game_Mode = printMenuAndHandleInput();
                break;
            case Gaming:
                gameCycle();
                break;
            default:
                break;
        }
    }
    return 0;
}

void initGame() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Board[r][c] = 0;
        }
    }
    Board[3][3] = 2;
    Board[3][4] = 1;
    Board[4][3] = 1;
    Board[4][4] = 2;

    Player_Turn = 1;
    Msg_In_Game = 0;
}

void printBoard(bool showPlacable) {
    std::cout << "  a b c d e f g h" << std::endl;
    int labelPlacable = 1;
    for (int r = 0; r < 8; r++) {
        std::cout << r + 1;
        bool leftIsPlacable = false;
        for (int c = 0; c < 8; c++) {
            if (showPlacable && checkCellPlacable(Player_Turn, r, c)) {
                std::cout << (leftIsPlacable ? "|" : "[") << (char)(labelPlacable < 10 ? labelPlacable + '0' : labelPlacable - 10 + 'a');
                ++labelPlacable;
                leftIsPlacable = true;
            } else {
                std::cout << (leftIsPlacable ? "]" : " ") << Labal_Map[Board[r][c]];
                leftIsPlacable = false;
            }
        }
        std::cout << (leftIsPlacable ? "]" : " ") << r + 1 << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl;
}

GameMode printMenuAndHandleInput() {
    std::cout << "===Game Menu===" << std::endl;
    std::cout << "1: Start New Game" << std::endl;
    std::cout << "0: Exit" << std::endl;

    std::string inputStr;
    std::cin >> inputStr;
    char input = inputStr.at(0);
    switch (input) {
        case '1':
            initGame();
            return Gaming;
        case '0':
            return Quitting;
        default:
            return Menu;
    }
}

void gameCycle() {
    std::cout << std::endl
              << "===================" << std::endl
              << std::endl;
    printBoard(true);
    std::cout << std::endl;

    // Print in-game message
    switch (Msg_In_Game) {
        case 1:
            std::cout << "Invalid Command!" << std::endl;
            break;
        case 2:
            std::cout << (Player_Turn == 1 ? "Black" : "White") << " Cannot Place Here!" << std::endl;
            break;
        case 3:
            std::cout << (Player_Turn == 1 ? "Black" : "White") << " Cannot make a valid move, " << (Player_Turn == 1 ? "White" : "Black") << " Continue!" << std::endl;
            break;
        default:
            std::cout << std::endl;
            break;
    }

    int* placableArr = getPlacableArr(Player_Turn);

    // Handle player cannot make valid move
    if (placableArr[60] <= 0) {
        int* placableArrOppo = getPlacableArr(3 - Player_Turn);
        if (placableArrOppo[60] <= 0) {  // Both players cannot make valid move, game ends
            int numBlack = countPlayerIdx(1);
            int numWhite = countPlayerIdx(2);
            std::cout << "Game Ended: "
                      << (numBlack > numWhite ? "Black Win!" : numBlack < numWhite ? "White Win!"
                                                                                   : "Tie!")
                      << " " << numBlack << ":" << numWhite << std::endl;
            std::string cmd;
            std::cin >> cmd;

            Game_Mode = Menu;
        } else {  // Opponent can place, handle player turn to them
            Player_Turn = 3 - Player_Turn;
            Msg_In_Game = 3;
        }

        delete[] placableArr;
        delete[] placableArrOppo;
        return;
    }

    // Prompt the player to place a piece
    std::cout << (Player_Turn == 1 ? "Black" : "White") << " Place: ";

    std::string cmd;
    std::cin >> cmd;

    if (cmd.length() <= 0) {
        Msg_In_Game = 1;
        delete[] placableArr;
        return;
    }

    if (cmd.length() == 1) {
        // Input 0 to exit to menu
        if (cmd.at(0) == '0') {
            Game_Mode = Menu;
            delete[] placableArr;
            return;
        }
        // Handle quick select (single char command)
        if (cmd.at(0) >= '1' && cmd.at(0) <= '9') {
            if (cmd.at(0) - '1' >= placableArr[60]) {
                Msg_In_Game = 1;
                delete[] placableArr;
                return;
            }
            int row = placableArr[cmd.at(0) - '1'] / 10;
            int col = placableArr[cmd.at(0) - '1'] % 10;
            if (placePiece(Player_Turn, row, col)) {
                Player_Turn = 3 - Player_Turn;
            }
            delete[] placableArr;
            return;
        }
        if (cmd.at(0) >= 'a' && cmd.at(0) <= 'z') {
            if (cmd.at(0) - 'a' + 9 >= placableArr[60]) {
                Msg_In_Game = 1;
                delete[] placableArr;
                return;
            }
            int row = placableArr[cmd.at(0) - 'a' + 9] / 10;
            int col = placableArr[cmd.at(0) - 'a' + 9] % 10;
            if (placePiece(Player_Turn, row, col)) {
                Player_Turn = 3 - Player_Turn;
            }
            delete[] placableArr;
            return;
        }
        Msg_In_Game = 1;
        delete[] placableArr;
        return;
    }

    delete[] placableArr;
    // Handle coordinates command
    char cmd0 = cmd.at(0);
    char cmd1 = cmd.at(1);
    if (cmd0 >= '1' && cmd0 <= '8') {
        if (cmd1 >= 'a' && cmd1 <= 'h') {
            if (placePiece(Player_Turn, cmd0 - '1', cmd1 - 'a')) {
                Player_Turn = 3 - Player_Turn;
            }
            return;
        }
        Msg_In_Game = 1;
        return;
    }
    if (cmd1 >= '1' && cmd1 <= '8') {
        if (cmd0 >= 'a' && cmd0 <= 'h') {
            if (placePiece(Player_Turn, cmd1 - '1', cmd0 - 'a')) {
                Player_Turn = 3 - Player_Turn;
            }
            return;
        }
        Msg_In_Game = 1;
        return;
    }
    Msg_In_Game = 1;
    return;
}

ReversableLines getReversableLines(int playerIdx, int row, int col) {
    ReversableLines result;
    // Check up
    for (int i = 0; i < row; ++i) {
        int checkingRow = row - 1 - i;
        if (Board[checkingRow][col] == playerIdx) {  // found a same color piece, end searching
            result.u = i;
            break;
        }
        if (Board[checkingRow][col] == 0) break;  // found an empty space, end searching
    }
    // Check down
    for (int i = 0; i < 7 - row; ++i) {
        int checkingRow = row + 1 + i;
        if (Board[checkingRow][col] == playerIdx) {  // found a same color piece, end searching
            result.d = i;
            break;
        }
        if (Board[checkingRow][col] == 0) break;  // found an empty space, end searching
    }
    // Check left
    for (int i = 0; i < col; ++i) {
        int checkingCol = col - 1 - i;
        if (Board[row][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.l = i;
            break;
        }
        if (Board[row][checkingCol] == 0) break;  // found an empty space, end searching
    }
    // Check right
    for (int i = 0; i < 7 - col; ++i) {
        int checkingCol = col + 1 + i;
        if (Board[row][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.r = i;
            break;
        }
        if (Board[row][checkingCol] == 0) break;  // found an empty space, end searching
    }
    // Check up-left
    for (int i = 0; i < row && i < col; ++i) {
        int checkingRow = row - 1 - i;
        int checkingCol = col - 1 - i;
        if (Board[checkingRow][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.ul = i;
            break;
        }
        if (Board[checkingRow][checkingCol] == 0) break;  // found an empty space, end searching
    }
    // Check up-right
    for (int i = 0; i < row && i < 7 - col; ++i) {
        int checkingRow = row - 1 - i;
        int checkingCol = col + 1 + i;
        if (Board[checkingRow][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.ur = i;
            break;
        }
        if (Board[checkingRow][checkingCol] == 0) break;  // found an empty space, end searching
    }
    // Check down-left
    for (int i = 0; i < 7 - row && i < col; ++i) {
        int checkingRow = row + 1 + i;
        int checkingCol = col - 1 - i;
        if (Board[checkingRow][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.dl = i;
            break;
        }
        if (Board[checkingRow][checkingCol] == 0) break;  // found an empty space, end searching
    }
    // Check up-right
    for (int i = 0; i < 7 - row && i < 7 - col; ++i) {
        int checkingRow = row + 1 + i;
        int checkingCol = col + 1 + i;
        if (Board[checkingRow][checkingCol] == playerIdx) {  // found a same color piece, end searching
            result.dr = i;
            break;
        }
        if (Board[checkingRow][checkingCol] == 0) break;  // found an empty space, end searching
    }

    return result;
}

bool checkCellPlacable(int playerIdx, int row, int col) {
    if (row >= 8 || row < 0) return false;
    if (col >= 8 || col < 0) return false;
    if (playerIdx < 1 || playerIdx > 2) return false;

    if (Board[row][col] != 0) return false;

    ReversableLines r = getReversableLines(Player_Turn, row, col);
    return r.u + r.d + r.l + r.r + r.ul + r.ur + r.dl + r.dr;
}

bool placePiece(int playerIdx, int row, int col) {
    // playerIdx should be 1 or 2
    if (playerIdx < 1 || playerIdx > 2) {
        std::cout << "Error: Invalid playerIdx" << std::endl;
        return false;
    }
    // check if placing coord is empty
    if (Board[row][col] != 0) {
        Msg_In_Game = 2;
        return false;
    }

    ReversableLines r = getReversableLines(playerIdx, row, col);
    // check if placing here can perform valid reversing
    if (!(r.u + r.d + r.l + r.r + r.ul + r.ur + r.dl + r.dr)) {
        Msg_In_Game = 2;
        return false;
    }

    // checking passed: perform placing and reversing
    Board[row][col] = playerIdx;
    for (int i = 0; i < r.u; ++i)  // reverse up
        Board[row - 1 - i][col] = playerIdx;
    for (int i = 0; i < r.d; ++i)  // reverse down
        Board[row + 1 + i][col] = playerIdx;
    for (int i = 0; i < r.l; ++i)  // reverse left
        Board[row][col - 1 - i] = playerIdx;
    for (int i = 0; i < r.r; ++i)  // reverse right
        Board[row][col + 1 + i] = playerIdx;
    for (int i = 0; i < r.ul; ++i)  // reverse up-left
        Board[row - 1 - i][col - 1 - i] = playerIdx;
    for (int i = 0; i < r.ur; ++i)  // reverse up-right
        Board[row - 1 - i][col + 1 + i] = playerIdx;
    for (int i = 0; i < r.dl; ++i)  // reverse down-left
        Board[row + 1 + i][col - 1 - i] = playerIdx;
    for (int i = 0; i < r.dr; ++i)  // reverse down-right
        Board[row + 1 + i][col + 1 + i] = playerIdx;

    Msg_In_Game = 0;
    Show_Placable = false;
    return true;
}

int* getPlacableArr(int playerIdx) {
    int* result = new int[61];
    result[60] = 0;  // use as counter

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (checkCellPlacable(playerIdx, r, c)) {
                result[result[60]] = r * 10 + c;
                ++result[60];
            }
        }
    }

    return result;
}

int countPlayerIdx(int playerIdx) {
    int counter = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (Board[r][c] == playerIdx)
                ++counter;
    return counter;
}
