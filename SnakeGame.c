#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include<string.h>

int i, j, height = 20, width = 20;
int gameover, score;
int x, y, fruitx, fruity, flag;

// Function prototypes
int kbhit(void);
void reset_terminal_mode();
void set_conio_terminal_mode();

// Terminal modes to handle non-blocking key input
struct termios orig_termios;

// Initialize the game and place the fruit
void setup() {
    gameover = 0;
    x = height / 2;
    y = width / 2;

    do {
        fruitx = rand() % (height - 2) + 1;
        fruity = rand() % (width - 2) + 1;
    } while (fruitx <= 0 || fruity <= 0);

    score = 0;
}

// Draw the game boundaries and the snake
void draw() {
    system("clear");  // Clear the screen (use "cls" for Windows)

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                printf("#");
            }
            else {
                if (i == x && j == y)
                    printf("O");
                else if (i == fruitx && j == fruity)
                    printf("*");
                else
                    printf(" ");
            }
        }
        printf("\n");
    }

    printf("score = %d\n", score);
    printf("press X to quit the game\n");
}

// Handle user input without blocking
void input() {
    if (kbhit()) {
        char ch = getchar();
        switch (ch) {
        case 'a':
            flag = 1;
            break;
        case 's':
            flag = 2;
            break;
        case 'd':
            flag = 3;
            break;
        case 'w':
            flag = 4;
            break;
        case 'x':
            gameover = 1;
            break;
        }
    }
}

// Update game logic for movement and fruit collection
void logic() {
    usleep(100000);

    switch (flag) {
    case 1:
        y--;
        break;
    case 2:
        x++;
        break;
    case 3:
        y++;
        break;
    case 4:
        x--;
        break;
    }

    if (x < 1 || x >= height - 1 || y < 1 || y >= width - 1)
        gameover = 1;

    if (x == fruitx && y == fruity) {
        do {
            fruitx = rand() % (height - 2) + 1;
            fruity = rand() % (width - 2) + 1;
        } while (fruitx <= 0 || fruity <= 0);

        score += 10;
    }
}

// Check if keyboard has been hit
int kbhit(void) {
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

int main() {
    set_conio_terminal_mode();

    setup();

    while (!gameover) {
        draw();
        input();
        logic();
    }

    reset_terminal_mode();
    return 0;
}

// Set terminal to single character input mode
void set_conio_terminal_mode() {
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

// Reset terminal to original mode
void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
}
