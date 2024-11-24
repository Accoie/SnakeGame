#include <iostream>
#include <deque>
#include <random>
#include <conio.h>
#include <windows.h>

using namespace std;

const int FIELD_ROWS = 16;
const int FIELD_COLS = 8;
const char CHAR_EMPTY = '.';
const char CHAR_APPLE = '*';
const char CHAR_SNAKE_HEAD = '@';
const char CHAR_SNAKE_BODY = 'o';
const char MOVE_UP = 'w';
const char MOVE_DOWN = 's';
const char MOVE_LEFT = 'a';
const char MOVE_RIGHT = 'd';
const char EXIT_GAME = 'q';
const int GAME_SPEED = 600;

typedef pair<int, int> Position; // using pair for Position

struct SnakeSegment {
    Position pos;
    char direction;
};

void initializeField(vector<vector<char>>& field) {
    field.assign(FIELD_ROWS, vector<char>(FIELD_COLS, CHAR_EMPTY));
}

void displayField(const vector<vector<char>>& field) {
    system("cls");
    for (const auto& row : field) {
        for (char cell : row) {
            cout << cell;
        }
        cout << endl;
    }
}

Position generateRandomPosition() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> rowDist(0, FIELD_ROWS - 1);
    uniform_int_distribution<> colDist(0, FIELD_COLS - 1);
    return { colDist(gen), rowDist(gen) };
}

bool isPositionOccupied(const Position& pos, const deque<SnakeSegment>& snake) {
    for (const auto& segment : snake) {
        if (segment.pos == pos) {
            return true;
        }
    }
    return false;
}

Position spawnApple(const deque<SnakeSegment>& snake) {
    Position applePos;
    while (isPositionOccupied(applePos, snake)) {
        applePos = generateRandomPosition();
    };
    return applePos;
}

void updateField(vector<vector<char>>& field, const deque<SnakeSegment>& snake, const Position& apple) {
    initializeField(field);
    field[apple.second][apple.first] = CHAR_APPLE;

    for (size_t i = 0; i < snake.size(); ++i) {
        const auto& segment = snake[i];
        if (segment.pos.second >= 0 && segment.pos.second < FIELD_ROWS &&
            segment.pos.first >= 0 && segment.pos.first < FIELD_COLS) {
            field[segment.pos.second][segment.pos.first] = (i == 0) ? CHAR_SNAKE_HEAD : CHAR_SNAKE_BODY;
        }
    }
}

bool isWallCollision(const Position& pos) {
    return pos.first < 0 || pos.first >= FIELD_COLS || pos.second < 0 || pos.second >= FIELD_ROWS;
}

bool isSelfCollision(const deque<SnakeSegment>& snake) {
    const Position& head = snake.front().pos;
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i].pos == head) {
            return true;
        }
    }
    return false;
}

void displayGameOver() {
    cout << "Game Over!" << endl;
    exit(0);
}

void moveSnake(deque<SnakeSegment>& snake, char direction) {
    Position nextHead = snake.front().pos;

    switch (direction) {
    case MOVE_UP:    --nextHead.second; break;
    case MOVE_DOWN:  ++nextHead.second; break;
    case MOVE_LEFT:  --nextHead.first; break;
    case MOVE_RIGHT: ++nextHead.first; break;
    }

    if (isWallCollision(nextHead)) {
        displayGameOver();
        return;
    }


    snake.push_front({ nextHead, direction });
    snake.pop_back();
}

bool eatApple(deque<SnakeSegment>& snake, const Position& apple) {
    return snake.front().pos == apple;
}

void growSnake(deque<SnakeSegment>& snake) {

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

void displayWin() {
    cout << "You Win!" << endl;
    exit(0);
}

bool isOppositeDirection(char current, char next) {
    return (current == MOVE_UP && next == MOVE_DOWN) ||
        (current == MOVE_DOWN && next == MOVE_UP) ||
        (current == MOVE_LEFT && next == MOVE_RIGHT) ||
        (current == MOVE_RIGHT && next == MOVE_LEFT);
}

int main() {
    vector<vector<char>> field;
    initializeField(field);


    deque<SnakeSegment> snake = { {{4, 2}, MOVE_LEFT}, {{5, 2}, MOVE_LEFT} };
    Position apple = spawnApple(snake);

    char currentDirection = MOVE_LEFT;

    while (true) {
        Sleep(GAME_SPEED);
        if (_kbhit()) {
            char input = _getch();
            if (input == EXIT_GAME) {
                break;
            }
            if (!isOppositeDirection(currentDirection, input)) {
                currentDirection = input;
            }
        }

        moveSnake(snake, currentDirection);

        if (isWallCollision(snake.front().pos) || isSelfCollision(snake)) {
            displayGameOver();
        }

        if (eatApple(snake, apple)) {
            growSnake(snake);
            apple = spawnApple(snake);
        }

        if (snake.size() == FIELD_ROWS * FIELD_COLS) {
            displayWin();
        }

        updateField(field, snake, apple);
        displayField(field);
    }

    return 0;
}
