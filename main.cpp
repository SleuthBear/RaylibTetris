#include <iostream>
#include "raylib.h"
#include "raymath.h"

float CELLSIZE = 30.0f;
const int GRIDWIDTH = 10;
const int GRIDHEIGHT = 20;
int FALLSPEED = 1;
int DROPSPEED = 1;

enum Shape {
    O = 0,
    I = 1,
    S = 2,
    Z = 3,
    L = 4,
    J = 5,
    T = 6
};

Color COLORS[] = {RED, BLUE, GREEN, YELLOW};

typedef struct IntVector2 {
    int x;
    int y;
} IntVector2;

float pi = 3.141592f;

class Tetromino {
    public:
    Shape shape;
    IntVector2 blocks[4];
    Color color;

    void rotate() {
        //rotate to maintain the height
        int height = blocks[0].y;
        for(int i = 1; i < 4; i++) {
            if(blocks[i].y > height) height = blocks[i].y;
        }

        IntVector2 rotationPoint = blocks[0];
        for(int i = 1; i < 4; i++) {
            IntVector2 relativePosition = blocks[i];
            relativePosition.x -= rotationPoint.x;
            relativePosition.y -= rotationPoint.y;
            //Vector2Subtract(blocks[i], rotationPoint);
            blocks[i].x = std::round(rotationPoint.x + relativePosition.x*std::cos(pi/2) - relativePosition.y*std::sin(pi/2));
            blocks[i].y = std::round(rotationPoint.y + relativePosition.y*std::cos(pi/2) + relativePosition.x*std::sin(pi/2));
        }

        // Shift vertically if needed
        int low = blocks[0].y;
        for(int i = 1; i < 4; i++) {
            if(blocks[i].y > low) low = blocks[i].y;
        }
        shiftY(height-low);
        boundX();
    }

    void boundX() {
        // keep inside grid
        int left = GRIDWIDTH-1;
        int right = 0;
        for(int i = 0; i < 4; i++) {
            if(blocks[i].x < left) left = blocks[i].x;
            if(blocks[i].x > right) right = blocks[i].x;
        }
        if(left < 0) {
            shiftX(-left);
        }else if(right > GRIDWIDTH-1) {
            shiftX(GRIDWIDTH-1-right);
        }
    }

    void shiftX(const int shift) {
        if(shift == 0) {
            return;
        }
        for(int i = 0; i < 4; i++) {
            blocks[i].x += shift;
        }
        boundX();
    }

    void shiftY(const int shift) {
        for(int i = 0; i < 4; i++) {
            blocks[i].y += shift;
        }
    }

    void draw() {
        for(int i = 0; i < 4; i++) {
            Rectangle rec = {blocks[i].x*CELLSIZE, blocks[i].y*CELLSIZE, CELLSIZE, CELLSIZE};
            DrawRectangleRounded(rec, 0.25, 5, color);
        }
    }
};

void ReadInput(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
bool CheckCollision(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
void DrawGrid(bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
bool ValidPosition(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
void ClearGrid(bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
Tetromino NewTetro(int color, int shape);

int main() {
    const int screenWidth = GRIDWIDTH*CELLSIZE;
    const int screenHeight = GRIDHEIGHT*CELLSIZE;
    InitWindow(screenWidth, screenHeight, "Tetris");
    int nTetros = 0;
    Tetromino tetro = NewTetro(nTetros, rand() % 7);
    bool grid[GRIDHEIGHT][GRIDWIDTH] = {false};

    // start game loop
    float timer = 0.0f;
    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        SetTargetFPS(60);
        timer += deltaTime;
        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            ReadInput(&tetro, grid);
            if(timer > 0.5) {
                timer = 0.0f;
                tetro.shiftY(1);
            }
            bool collision = CheckCollision(&tetro, grid);
            if(collision) {
                nTetros = (nTetros + 1) % 4;

                tetro = NewTetro(nTetros, rand() % 7);
            }
            tetro.draw();
            ClearGrid(grid);
            DrawGrid(grid);
        EndDrawing();
    }
    CloseWindow();

}

void ReadInput(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]) {
    Tetromino oldPosition = *tetro;
    if(IsKeyPressed(KEY_UP)) tetro->rotate();
    else if(IsKeyPressed(KEY_RIGHT)) tetro->shiftX(1);
    else if(IsKeyPressed(KEY_LEFT)) tetro->shiftX(-1);
    else if(IsKeyPressed(KEY_DOWN)) tetro->shiftY(DROPSPEED);

    if(!ValidPosition(tetro, grid)) {
        *tetro = oldPosition;

    }
}

bool CheckCollision(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]) {
    for(int i = 0; i < 4; i++) {
        if(tetro->blocks[i].y+1 >= GRIDHEIGHT || grid[tetro->blocks[i].y+1][tetro->blocks[i].x]) {
            for(int i = 0; i < 4; i++) {
                grid[tetro->blocks[i].y][tetro->blocks[i].x] = true;
            }
            return true;
        }
    }
    return false;
}

void DrawGrid(bool (&grid)[GRIDHEIGHT][GRIDWIDTH]) {
    for(int i = 0; i < GRIDHEIGHT; i++) {
        for(int j = 0; j < GRIDWIDTH; j++) {
            if(grid[i][j]) {
                DrawRectangle(j*CELLSIZE, i*CELLSIZE, CELLSIZE, CELLSIZE, GRAY);
            }
        }
    }
}

bool ValidPosition(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]) {
    for(int i = 0; i < 4; i++) {
        if(grid[tetro->blocks[i].y][tetro->blocks[i].x]) {
            return false;
        }
    }
    return true;
}

void ClearGrid(bool (&grid)[GRIDHEIGHT][GRIDWIDTH]) {
    for(int i = 0; i < GRIDHEIGHT; i++) {
        bool isFull = true;
        for(int j = 0; j < GRIDWIDTH; j++) {
            if(grid[i][j] == false) {
                isFull = false;
            }
        }
        if(isFull) {
            for(int k = i; k > 0; k--) {
                for(int j = 0; j < GRIDWIDTH; j++) {
                    grid[k][j] = grid[k-1][j];
                }
            }
        }
    }
}

Tetromino NewTetro(int color, int shape) {
    switch(shape) {
        case I:
            return Tetromino{I, {{4,0}, {5,0}, {6,0}, {7,0}}, COLORS[color]};
        case L:
            return Tetromino{L, {{4,1}, {4,0}, {5,0}, {6,0}}, COLORS[color]};
        case J:
            return Tetromino{J, {{4,0}, {4,1}, {5,1}, {6,1}}, COLORS[color]};
        case S:
            return Tetromino{S, {{4,1}, {5,1}, {5,0}, {6,0}}, COLORS[color]};
        case Z:
            return Tetromino{Z, {{4,0}, {5,0}, {5,1}, {6,1}}, COLORS[color]};
        case O:
            return Tetromino{O, {{4,0}, {5,0}, {4,1}, {5,1}}, COLORS[color]};
        case T:
            return Tetromino{T, {{4,0}, {5,0}, {5,1}, {6,0}}, COLORS[color]};
    }
}
