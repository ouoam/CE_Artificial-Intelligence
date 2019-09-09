#include <stdio.h>

char table[8][8] = {
    {' ', 'l', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', 'l', 's', ' ', ' ', ' ', ' '},
    {' ', 'a', ' ', 'a', ' ', ' ', ' ', ' '},
    {' ', ' ', 's', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', 'S', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'A', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'L', ' ', ' ', ' ', ' '}
};

int tableL[8][8];

int num_s = 2;
int num_l = 2;
int num_a = 2;

void printTable() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf(" %c |", table[i][j]);
        }
        printf("\n---+---+---+---+---+---+---+---+\n");
    }
}

void printCost() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf(" %4d|", tableL[i][j]);
        }
        printf("\n-----+-----+-----+-----+-----+-----+-----+-----+\n");
    }
}

int win(char allies, char enemies) {
    if (allies == 'S') {
        if (enemies == 'l') return -1;
        if (enemies == 's') return 0;
        if (enemies == 'a') return 1;
    }

    if (allies == 'L') {
        if (enemies == 'a') return -1;
        if (enemies == 'l') return 0;
        if (enemies == 's') return 1;
    }

    if (allies == 'A') {
        if (enemies == 's') return -1;
        if (enemies == 'a') return 0;
        if (enemies == 'l') return 1;
    }

    return 2;
}

int checkAround(char allies, int i, int j) {
    if (table[i][j] != ' ') return 0;

    if (i - 1 >= 0) {
        if (win(allies, table[i - 1][j]) < 0) return -1;
    }

    if (i + 1 <= 8) {
        if (win(allies, table[i + 1][j]) < 0) return -1;
    }

    if (j - 1 >= 0) {
        if (win(allies, table[i][j - 1]) < 0) return -1;
    }

    if (j + 1 <= 8) {
        if (win(allies, table[i][j + 1]) < 0) return -1;
    }

    return 1;
}

int dfs(int i, int j, char charactor, int deep) {
    if (i < 0 || 7 < i || j < 0 || 7 < j) return -1;

    if (tableL[i][j] < deep) return -2;
    tableL[i][j] = deep;

    printf("In %d,%d -> %d\n", i, j, deep);

    if (i - 1 >= 0) {
        if (win(charactor, table[i - 1][j]) == 1) {
            printf("Win %d %d -> %d %d\n", i, j, i - 1, j);
            table[i - 1][j] = ' ';
            table[i][j] = charactor;
            //return 1;
        }
    }

    if (i + 1 <= 8) {
        if (win(charactor, table[i + 1][j]) == 1) {
            printf("Win %d %d -> %d %d\n", i, j, i + 1, j);
            table[i + 1][j] = ' ';
            table[i][j] = charactor;
            //return 1;
        }
    }

    if (j - 1 >= 0) {
        if (win(charactor, table[i][j - 1]) == 1) {
            printf("Win %d %d -> %d %d\n", i, j, i, j - 1);
            table[i][j - 1] = ' ';
            table[i][j] = charactor;
            //return 1;
        }
    }

    if (j + 1 <= 8) {
        if (win(charactor, table[i][j + 1]) == 1) {
            printf("Win %d %d -> %d %d\n", i, j, i, j + 1);
            table[i][j + 1] = ' ';
            table[i][j] = charactor;
            //return 1;
        }
    }

    int re = 0;

    if (i - 1 >= 0) {
        if (checkAround(charactor, i - 1, j) == 1) {
            re = dfs(i - 1, j, charactor, deep + 1);
            if (re == 1) return 1;
        }
    }

    if (i + 1 <= 8) {
        if (checkAround(charactor, i + 1, j) == 1) {
            re = dfs(i + 1, j, charactor, deep + 1);
            if (re == 1) return 1;
        }
    }

    if (j - 1 >= 0) {
        if (checkAround(charactor, i, j - 1) == 1) {
            re = dfs(i, j - 1, charactor, deep + 1);
            if (re == 1) return 1;
        }
    }

    if (j + 1 <= 8) {
        if (checkAround(charactor, i, j + 1) == 1) {
            re = dfs(i, j + 1, charactor, deep + 1);
            if (re == 1) return 1;
        }
    }

    return 0;
}

void resetCost() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tableL[i][j] = 5000;
        }
    }
}

int main() {
    //while (num_s != 0 && num_l != 0 && num_a != 0) {
    resetCost();
    while (num_a != 0) {
        int re = 0;
        for (int i = 0; i < 8 && !re; i++) {
            for (int j = 0; j < 8 && !re; j++) {
                if (table[i][j] == 'A') {
                    re = dfs(i, j, 'A', 0);
                    if (re == 1) {
                        table[i][j] = ' ';
                        num_a--;

                        resetCost();
                    }
                    printCost();
                    printTable();
                }
            }
        }
        getch();
    }
}
