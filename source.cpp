#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define SCREEN_WIDTH    (30)
#define SCREEN_HEIGHT   (20)
#define FPS             (60)
#define INTERVAL        (1000 / FPS)

#define INVADER_COLUMN  (7)   
#define INVADER_ROW     (7)   
#define BULLET_MAX      (5) 

enum {
    TILE_NONE,
    TILE_INVADER,
    TILE_BULLET,
    TILE_PLAYER,
    TILE_MAX
};

typedef struct {
    int x, y;
} INVADER;

typedef struct {
    int x, y;
    int active; 
} BULLET;

int screen[SCREEN_HEIGHT][SCREEN_WIDTH];
INVADER invaders[INVADER_ROW][INVADER_COLUMN];
BULLET bullets[BULLET_MAX];

int invaderDirection; 
int frameCount; 
int playerX; 
int gameRunning; 
int gameOver; 
int rightMoveCount;

const char* tileAA[TILE_MAX] = {
    " ", // TILE_NONE
    "I", // TILE_INVADER
    "|", // TILE_BULLET
    "P"  // TILE_PLAYER
};

void ClearScreen() {
    system("cls");
}
// コンソール画面をクリアする関数ClearScreenの中にsystem関数をいれてclsを指定している。
// system関数は引数に指定したコマンドを実行する。clsはWindowsでコンソールの内容を消去するコマンド。

void DrawScreen() {
    memset(screen, 0, sizeof(screen));

    for (int y = 0; y < INVADER_ROW; y++)
        for (int x = 0; x < INVADER_COLUMN; x++)
            if (invaders[y][x].y != -1)
                screen[invaders[y][x].y][invaders[y][x].x] = TILE_INVADER;

    for (int i = 0; i < BULLET_MAX; i++) {
        if (bullets[i].active && bullets[i].y >= 0 && bullets[i].y < SCREEN_HEIGHT && bullets[i].x >= 0 && bullets[i].x < SCREEN_WIDTH) {
            screen[bullets[i].y][bullets[i].x] = TILE_BULLET; 
        }
    }
    // 画面を描画する関数DrawScreenの中にmemset関数をいれてscreen配列を初期化している。
    // forでyとxの位置をおって確認して-1でない場合はインベーダーを描画する(0＝インベーダーが存在　-1＝存在しない)

    screen[SCREEN_HEIGHT - 1][playerX] = TILE_PLAYER; 

    if (gameOver) {
        ClearScreen();
        printf("GAME OVER\n");
        printf("Press 'R' to Restart or 'Q' to Quit\n");
        return;
    }

    int allCleared = 1; 
    for (int y = 0; y < INVADER_ROW; y++) {
        for (int x = 0; x < INVADER_COLUMN; x++) {
            if (invaders[y][x].y != -1) {
                allCleared = 0; 
                break;
            }
        }
    }

    if (allCleared) {
        ClearScreen();
        printf("GAME CLEAR\n");
        printf("Press 'R' to Restart or 'Q' to Quit\n");
        return;
    }

    if (!gameRunning) {
        ClearScreen();
        printf("Press 'S' to Start\n");
        printf("Press 'Q' to Quit\n");
    } else {
        ClearScreen();
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++)
                printf("%s", tileAA[screen[y][x]]);
            printf("\n");
        }
    }
}

void Init() {
    int startX = (SCREEN_WIDTH - (INVADER_COLUMN * 2 - 1)) / 2; 
    for (int y = 0; y < INVADER_ROW; y++)
        for (int x = 0; x < INVADER_COLUMN; x++) {
            invaders[y][x].x = startX + x * 2; 
            invaders[y][x].y = y + 1;  
        }

    for (int i = 0; i < BULLET_MAX; i++)
        bullets[i].active = 0; 

    invaderDirection = 1; 
    frameCount = 0; 
    playerX = SCREEN_WIDTH / 2; 
    gameOver = 0; 
    rightMoveCount = 0; 
    DrawScreen();
}

void MoveInvaders() {
    if (frameCount % 3 == 0) {
        for (int y = 0; y < INVADER_ROW; y++) {
            for (int x = 0; x < INVADER_COLUMN; x++) {
                if (invaders[y][x].y != -1) {
                    invaders[y][x].x += invaderDirection; 
                }
            }
        }

        rightMoveCount++;
        if (rightMoveCount >= 8) { 
            invaderDirection = -invaderDirection; 
            for (int y = 0; y < INVADER_ROW; y++) {
                for (int x = 0; x < INVADER_COLUMN; x++) {
                    if (invaders[y][x].y != -1) {
                        invaders[y][x].y++;
                    }
                }
            }
            rightMoveCount = 0; 
        }
    }
    frameCount++; 

    for (int y = 0; y < INVADER_ROW; y++) {
        for (int x = 0; x < INVADER_COLUMN; x++) {
            if (invaders[y][x].y >= SCREEN_HEIGHT - 1) {
                gameOver = 1;
                break;
            }
        }
    }
}

void UpdateBullets() {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (bullets[i].active) {
            bullets[i].y--;
            if (bullets[i].y < 0) {
                bullets[i].active = 0; 
            }
        }
    }
}

void CheckCollisions() {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (bullets[i].active) {
            for (int y = 0; y < INVADER_ROW; y++) {
                for (int x = 0; x < INVADER_COLUMN; x++) {
                    if (invaders[y][x].x == bullets[i].x && invaders[y][x].y == bullets[i].y) {
                        bullets[i].active = 0; 
                        invaders[y][x].y = -1; 
                        break;
                    }
                }
            }
        }
    }
}

void ShootBullet() {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (!bullets[i].active) {
            bullets[i].x = playerX; 
            bullets[i].y = SCREEN_HEIGHT - 2; 
            bullets[i].active = 1; 
            break;
        }
    }
}

int main() {
    gameRunning = 0; 

    while (1) {
        if (!gameRunning) {
            DrawScreen();
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 's' || ch == 'S') { 
                    gameRunning = 1; 
                    Init();
                } else if (ch == 'q' || ch == 'Q') { 
                    return 0;
                }
            }
            frameCount++; 
            continue;
        }

        DrawScreen();
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'a' || ch == 'A') { 
                if (playerX > 0) playerX--;
            } else if (ch == 'd' || ch == 'D') { 
                if (playerX < SCREEN_WIDTH - 1) playerX++;
            } else if (ch == ' ') { 
                ShootBullet();
            } else if (ch == 'q' || ch == 'Q') { 
                return 0;
            } else if (ch == 'r' || ch == 'R') { 
                Init();
                gameRunning = 1;
                gameOver = 0; 
            }
        }

        MoveInvaders();
        UpdateBullets();
        CheckCollisions();

        DrawScreen();
        Sleep(INTERVAL); 
    }
    return 0;
}