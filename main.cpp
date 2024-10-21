#include <iostream>
#include "raylib.h"
#include "raymath.h"

float CELLSIZE = 30.0f;
const int GRIDWIDTH = 10;
const int GRIDHEIGHT = 20;
int FALLSPEED = 1;
int DROPSPEED = 1;

enum Shape {
    SQUARE,
    LINE
};

typedef struct IntVector2 {
    int x;
    int y;
} IntVector2;

float pi = 3.141592f;

class Tetromino {
    public:
    Shape shape;
    IntVector2 blocks[4];

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
            DrawRectangleRounded(rec, 0.25, 5, BLUE);
        }
    }
};

void ReadInput(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
bool CheckCollision(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
void DrawGrid(bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);
bool ValidPosition(Tetromino *tetro, bool (&grid)[GRIDHEIGHT][GRIDWIDTH]);

int main() {
    const int screenWidth = GRIDWIDTH*CELLSIZE;
    const int screenHeight = GRIDHEIGHT*CELLSIZE;
    InitWindow(screenWidth, screenHeight, "Tetris");
    Tetromino tetro = {LINE, {{1,5},{1,6}, {1,7}, {1,8}}};
    bool grid[GRIDHEIGHT][GRIDWIDTH] = {false};
    grid[15][5] = true;

    // start game loop
    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        SetTargetFPS(60);

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            ReadInput(&tetro, grid);
            bool collision = CheckCollision(&tetro, grid);
            if(collision) {
                tetro = Tetromino {LINE, {{2, 1}, {3, 1}, {4,1}, {5, 1}}};
            }
            tetro.draw();
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