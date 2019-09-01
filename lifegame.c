#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void usage(void)
{
    printf("ライフゲーム操作方法\n");
    printf("\n");
    printf("    左移動      :a\n");
    printf("    右移動      :d\n");
    printf("    上移動      :w\n");
    printf("    下移動      :s\n");
    printf("\n");
    printf("    生死切替    :Space\n");
    printf("\n");
    printf("    世代交代    :Enter\n");
    printf("\n");
    printf("    盤面リセット:r\n");
    printf("    カーソル削除:p\n");
    printf("    ゲーム終了  :q\n");
    printf("\n");
    printf("Enterキーを押してゲームに進む\n");

    getchar();
}

#define FIELD_H 20
#define FIELD_W 20
typedef struct {
    int r;
    int c;
} position_t;
int st_field[FIELD_H][FIELD_W];
int st_work[FIELD_H][FIELD_W];
position_t st_cursor;
int st_quit = 0;
int cursor_hidden = 0;

void display(void)
{
    system("clear");
    for (int r = 0; r < FIELD_H; r++) {
        for (int c = 0; c < FIELD_W; c++) {
            if (r == st_cursor.r && c == st_cursor.c
                    && !cursor_hidden) {
                printf("%s", "II");
            } else {
                printf("%s", st_field[r][c]? "HH": "[]");
            }
        }
        printf("\n");
    }
}
int count_lives(int row, int col)
{
    int r = 0, c = 0;
    int cnt = 0;

    for (r = row+FIELD_H-1; r <= row+FIELD_H+1; r++) {
        for (c = col+FIELD_W-1; c <= col+FIELD_W+1; c++) {
            if (r == row+FIELD_H && c == col+FIELD_W) {
                continue;
            }
            if (st_work[r%FIELD_H][c%FIELD_W]) {
                cnt++;
            }
        }
    }
    return cnt;
}

void next_generation(void)
{
    int cnt = 0;

    memcpy(st_work, st_field, sizeof(st_field));
    for (int r = 0; r < FIELD_H; r++) {
        for (int c = 0; c < FIELD_W; c++) {
            cnt = count_lives(r, c);
            if (st_field[r][c]) {
                if (cnt >= 4 || cnt <= 1) {
                    st_field[r][c] = 0;
                }
            } else {
                if (cnt == 3) {
                    st_field[r][c] = 1;
                }
            }
        }
    }
}

void reset(void)
{
    for (int r = 0; r < FIELD_H; r++) {
        for (int c = 0; c < FIELD_W; c++) {
            st_field[r][c] = 0;
        }
    }
}

int main(void)
{
    char in = 0;
    usage();

    display();

    while (1) {
        if (kbhit()) {
            in = getchar();
            printf("'%c' (0x%0X)が押されました\n", in, in);
            switch (in) {
            case 'a':
                if (st_cursor.c != 0) {
                    printf("'%c' entered.\n", in);
                    st_cursor.c -= 1;
                }
                break;
            case 'd':
                if (st_cursor.c != FIELD_W-1) {
                    printf("'%c' entered.\n", in);
                    st_cursor.c += 1;
                }
                break;
            case 'w':
                if (st_cursor.r != 0) {
                    printf("'%c' entered.\n", in);
                    st_cursor.r -= 1;
                }
                break;
            case 's':
                if (st_cursor.r != FIELD_H-1) {
                    printf("'%c' entered.\n", in);
                    st_cursor.r += 1;
                }
                break;
            case ' ':
                printf("SPACE entered.\n");
                st_field[st_cursor.r][st_cursor.c] ^= 1;
                break;
            case '\n':
                printf("ENTER entered.\n");
                next_generation();
                break;
            case 'r':
                reset();
                break;
            case 'p':
                cursor_hidden ^= 1;
                break;
            case 'q':
                st_quit = 1;
                break;
            }
            printf("cursor [%d, %d]\n", st_cursor.r, st_cursor.c);
            if (st_quit) {
                break;
            }
            display();
        }
    }

    return 0;
}
