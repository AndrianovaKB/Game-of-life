#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_LENGTH 2000
#define ROWS 25
#define COLS 80
void print(char field[ROWS][COLS]);
int countLiveCel(char field[ROWS][COLS]);
int countNeighbours(char field[ROWS][COLS], int x, int y);
void game(char newField[ROWS][COLS], char field[ROWS][COLS], double temp, int countLiveCels);
void inputMode(char field[ROWS][COLS]);
void inputFromFile(char field[ROWS][COLS]);
int fileRead(char path[MAX_LENGTH], char field[ROWS][COLS]);
void inputFromKeyboard(char field[ROWS][COLS]);
void statPrint(double del, int liveCells);
void updateLive(char newField[ROWS][COLS], char field[ROWS][COLS], int neighbours, int x, int y);

int main(int argc, char *argv[]) {
    char field[ROWS][COLS];
    char newField[ROWS][COLS];
    if (argc == 1) {
        inputMode(field);
    } else if (argc == 2) {
        if (fileRead(argv[1], field)) {
        } else {
            printf("Incorrect file path");
            inputMode(field);
        }
    }
    initscr();
    noecho();
    nodelay(stdscr, true);
    int countLiveCels;
    double temp = 1.0;

    do {
        char speedControler;
        speedControler = getch();
        countLiveCels = countLiveCel(field);
        if (speedControler == '+' && temp < 4) temp += 0.1;
        if (speedControler == '-' && temp > 0.2) temp -= 0.1;
        if (speedControler == 'q') countLiveCels = 0;
        game(newField, field, temp, countLiveCels);
    } while (countLiveCels);
    endwin();
}
void game(char newField[ROWS][COLS], char field[ROWS][COLS], double temp, int countLiveCels) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; ++j) {
            updateLive(newField, field, countNeighbours(field, i, j), i, j);
        }
    }
    print(newField);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; ++j) {
            field[i][j] = newField[i][j];
        }
    }
    usleep(50000 * temp);
    statPrint(temp, countLiveCels);
}
int countLiveCel(char field[ROWS][COLS]) {
    int count = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; ++j) {
            if (field[i][j] == '1') {
                count++;
            }
        }
    }
    return count;
}
void inputMode(char field[ROWS][COLS]) {
    int choosenMode = 0;
    char e = '.';
    do {
        system("clear");
        printf("\nChoose input:\n1. From file\n2. From keyboard\n");
    } while (!(scanf("%d%c", &choosenMode, &e) == 2 && e == '\n' && (choosenMode == 2 || choosenMode == 1)));
    system("clear");
    if (choosenMode == 1) {
        inputFromFile(field);
    } else {
        inputFromKeyboard(field);
    }
}
void inputFromKeyboard(char field[ROWS][COLS]) {
    printf("Put 1, if it's live cell and put 0 if it dead cell\n");
    char c;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; ++j) {
            scanf("%c", &c);
            if (c == '\n') {
                j--;
                continue;
            } else {
                field[i][j] = c;
            }
        }
    }
}

void inputFromFile(char field[ROWS][COLS]) {
    int i = 1, j = 0;
    char e = '.';
    int choosenFile = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir("patterns");
    do {
        system("clear");
        i = 1;
        j = 0;
        printf("Choose file and input it's number:\n");

        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (j > 1) {
                    printf("%d. %s\n", i, dir->d_name);
                    i++;
                } else {
                    j++;
                }
            }
            closedir(d);
        }
    } while (
        !(scanf("%d%c", &choosenFile, &e) == 2 && e == '\n' && (choosenFile > 0 && choosenFile <= (i - 1))));
    j = 1;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (j == choosenFile + 2) {
                char path[MAX_LENGTH];
                snprintf(path, sizeof(path), "patterns/%s", dir->d_name);
                fileRead(path, field);
            }
            j++;
        }
    }
}

int fileRead(char path[MAX_LENGTH], char field[ROWS][COLS]) {
    FILE *ptr = fopen(path, "r");
    if (access(path, R_OK) == 0) {
        char t;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                fscanf(ptr, "%c", &t);
                if (t == '\n') {
                    j--;
                    continue;
                }
                field[i][j] = t;
            }
        }
        fclose(ptr);
        return 1;
    } else {
        fclose(ptr);
        return 0;
    }
}

void print(char field[ROWS][COLS]) {
    clear();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (field[i][j] == '1') {
                printw("@");
            } else {
                printw(" ");
            }
        }
        printw("\n");
    }
}

void statPrint(double del, int liveCells) {
    printw("==== Live cells: %04d ======== Speed [", liveCells);
    for (int i = 0; i < 10 - (int)(del / 0.39); i++) {
        printw("*");
    }
    for (int i = 0; i < (int)(del / 0.39); i++) {
        printw(" ");
    }
    printw("] ==== ");
}

void updateLive(char newField[ROWS][COLS], char field[ROWS][COLS], int neighbours, int x, int y) {
    if (neighbours == 3) {
        newField[x][y] = '1';
    } else if (neighbours == 2 && field[x][y] == '1') {
        newField[x][y] = '1';
    } else {
        newField[x][y] = '0';
    }
}
int countNeighbours(char field[ROWS][COLS], int x, int y) {
    int neighbours = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0)) {
                if (field[(x + i + ROWS) % ROWS][(y + j + COLS) % COLS] == '1') {
                    neighbours++;
                }
            }
        }
    }
    return neighbours;
}
