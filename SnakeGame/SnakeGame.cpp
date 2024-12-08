/*Snake easy :
1. Starts at length two(thus it cannot collide with itself)
2. Death upon collision with a wall
3. Death upon collision with itself
4. Eating an apple increases length by one
5. Victory upon filling the entire game board with the snake*/

#include <iostream>
#include <deque>
#include <random>
#include <conio.h>
#include <windows.h>

using namespace std;

const int INITIAL_SNAKE_HEAD_X = 4;
const int INITIAL_SNAKE_HEAD_Y = 2;
const int INITIAL_SNAKE_BODY_X = 5;
const int INITIAL_SNAKE_BODY_Y = 2;
const char MOVE_UP = 'w';
const char MOVE_DOWN = 's';
const char MOVE_LEFT = 'a';
const char MOVE_RIGHT = 'd';
const char INITIAL_SNAKE_DIRECTION = MOVE_LEFT;
const int FIELD_ROWS = 16;
const int FIELD_COLS = 8;
const char CHAR_FIELD = '.';
const char CHAR_APPLE = '*';
const char CHAR_SNAKE_HEAD = '@';
const char CHAR_SNAKE_BODY = 'o';
const char EXIT_GAME = 'q';
const int GAME_SPEED = 200;

typedef pair<int, int> Position; 

struct SnakeSegment {
    Position pos;
    char direction;
};
Position GenerateRandomPosition() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> randomRow(0, FIELD_ROWS - 1);
    uniform_int_distribution<int> randomCol(0, FIELD_COLS - 1);
    return { randomCol(gen), randomRow(gen) };
}

bool IsPositionOccupied(const Position &pos, const deque<SnakeSegment> &snake) {
    for (const auto& segment : snake) {
        if (segment.pos == pos) {
            return true;
        }
    }
    return false;
}

Position SpawnApple(const deque<SnakeSegment> &snake) {
    Position applePos;
    while (IsPositionOccupied(applePos, snake)) {
        applePos = GenerateRandomPosition();
    };
    return applePos;
}
void InitializeField(vector<vector<char>> &field) {
    field.assign(FIELD_ROWS, vector<char>(FIELD_COLS, CHAR_FIELD));
}
void InitializeGame(vector<vector<char>> &field, deque<SnakeSegment> &snake, Position &apple, char &currentDirection) {
    InitializeField(field);

    snake = {
        {{INITIAL_SNAKE_HEAD_X, INITIAL_SNAKE_HEAD_Y}, INITIAL_SNAKE_DIRECTION},
        {{INITIAL_SNAKE_BODY_X, INITIAL_SNAKE_BODY_Y}, INITIAL_SNAKE_DIRECTION}
    };

    apple = SpawnApple(snake);
    currentDirection = INITIAL_SNAKE_DIRECTION;
}

bool IsOppositeDirection(char current, char next) {
    return (current == MOVE_UP && next == MOVE_DOWN) ||
        (current == MOVE_DOWN && next == MOVE_UP) ||
        (current == MOVE_LEFT && next == MOVE_RIGHT) ||
        (current == MOVE_RIGHT && next == MOVE_LEFT);
}

void ProcessInput(char &currentDirection) {
    if (_kbhit()) {
        char input = _getch();
        if (input == EXIT_GAME) {
            exit(0);
        }
        if (!IsOppositeDirection(currentDirection, input)) {
            currentDirection = input;
        }
    }
}

void DisplayField(const vector<vector<char>> &field) {
    system("cls");
    for (const auto& row : field) {
        for (char cell : row) {
            cout << cell;
        }
        cout << endl;
    }
}

void UpdateField(vector<vector<char>> &field, const deque<SnakeSegment> &snake, const Position &apple) {
    InitializeField(field); 
    field[apple.second][apple.first] = CHAR_APPLE;

    for (size_t i = 0; i < snake.size(); ++i) {
        const auto& segment = snake.at(i);
        if (segment.pos.second >= 0 && segment.pos.second < FIELD_ROWS &&
            segment.pos.first >= 0 && segment.pos.first < FIELD_COLS) {
            field[segment.pos.second][segment.pos.first] = (i == 0) ? CHAR_SNAKE_HEAD : CHAR_SNAKE_BODY;
        }
    }
}

bool IsWallCollision(const Position &pos) {
    return pos.first < 0 || pos.first >= FIELD_COLS || pos.second < 0 || pos.second >= FIELD_ROWS;
}

bool IsSelfCollision(const deque<SnakeSegment> &snake) {
    const Position &head = snake.front().pos;
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake.at(i).pos == head) {
            return true;
        }
    }
    return false;
}

void DisplayGameOver() {
    cout << "Game Over!" << endl;
    exit(0);
}

bool EatApple(deque<SnakeSegment> &snake, const Position &apple) {
    return snake.front().pos == apple;
}

void GrowSnake(deque<SnakeSegment> &snake) {

    SnakeSegment tail = snake.back();
    Position newTail = tail.pos;

    switch (tail.direction) {
        case MOVE_UP:    ++newTail.second; break;
        case MOVE_DOWN:  --newTail.second; break;
        case MOVE_LEFT:  ++newTail.first; break;
        case MOVE_RIGHT: --newTail.first; break;
    }

    snake.push_back({ newTail, tail.direction });
}

void DisplayWin() {
    cout << "You Win!" << endl;
    exit(0);
}

void CheckApple(deque<SnakeSegment> &snake, Position &apple) {
    if (EatApple(snake, apple)) {
        GrowSnake(snake);
        apple = SpawnApple(snake);
    } else { snake.pop_back(); }
}

bool CheckWin(const deque<SnakeSegment>& snake) {
    return snake.size() == FIELD_ROWS * FIELD_COLS;
}
void CheckEndGame(const deque<SnakeSegment>& snake) {
    if (IsSelfCollision(snake)) {
        DisplayGameOver();
    } else if (CheckWin(snake)) {
        DisplayWin();
    }
    return;
}
void MoveSnake(deque<SnakeSegment> &snake, char direction, Position &apple) {
    Position nextHead = snake.front().pos;

    switch (direction) {
    case MOVE_UP:    --nextHead.second; break;
    case MOVE_DOWN:  ++nextHead.second; break;
    case MOVE_LEFT:  --nextHead.first; break;
    case MOVE_RIGHT: ++nextHead.first; break;
    }

    if (IsWallCollision(nextHead)) {
        DisplayGameOver();
        return;
    }

    snake.push_front({ nextHead, direction });

    CheckApple(snake, apple);
    CheckEndGame(snake);

}

int main() {

    vector<vector<char>> field;
    deque<SnakeSegment> snake;
    Position apple;
    char currentDirection;

    InitializeGame(field, snake, apple, currentDirection);

    while (true) {
        Sleep(GAME_SPEED);
        ProcessInput(currentDirection);
        MoveSnake(snake, currentDirection, apple);
        UpdateField(field, snake, apple);
        DisplayField(field);
    }

    return 0;
}