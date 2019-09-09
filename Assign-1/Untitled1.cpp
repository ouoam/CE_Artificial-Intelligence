#include <cstdio>
#include <cctype>
#include <queue>

/// 0 -> L
/// 1 -> S
/// 2 -> A

int typeChar2Num(char t) {
    t = tolower(t);
    switch (t) {
        case 'l': return 0;
        case 's': return 1;
        case 'a': return 2;
        default : return 3;
    }
}

char typeNum2Char(int t) {
    char a[] = {'l', 's', 'a', ' '};
    return a[t];
}


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

int numEnemies[3];

struct bfsQueue {
    int i;
    int j;
    int deep;
    char allies;
};

std::queue<bfsQueue> bfsQ;

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

void resetCost() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tableL[i][j] = 5000;
        }
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

    auto checkA = [&] (int iC, int jC)
    {
        if (iC >= 0 && iC <= 8 && jC >= 0 && jC <= 8) {
            if (win(allies, table[iC][jC]) < 0) return -1;
        }
        return 1;
    };

    if ( checkA(i - 1, j) == -1 ||
         checkA(i + 1, j) == -1 ||
         checkA(i, j - 1) == -1 ||
         checkA(i, j + 1) == -1 )
        return -1;

    return 1;
}

int bfs(int i, int j, char allies) {
    bfsQueue q;
    q.i = i;
    q.j = j;
    q.deep = 0;
    q.allies = allies;

    bfsQ.push(q);
    while (!bfsQ.empty())
    {
        bfsQueue f = bfsQ.front();
        bfsQ.pop();

        if (f.i < 0 || 7 < f.i || f.j < 0 || 7 < f.j) continue;

        if (tableL[f.i][f.j] <= f.deep) continue;
        tableL[f.i][f.j] = f.deep;

        printf("In %d,%d -> %d\n", f.i, f.j, f.deep);

        auto checkEnemies = [&] (int iEnemies, int jEnemies)
        {
            if (iEnemies >= 0 && iEnemies <= 8 && jEnemies >= 0 && jEnemies <= 8) {
                if (win(f.allies, table[iEnemies][jEnemies]) == 1) {
                    printf("Win %d %d -> %d %d\n", f.i, f.j, iEnemies, jEnemies);
                }
            }
        };

        checkEnemies(f.i - 1, f.j);
        checkEnemies(f.i + 1, f.j);
        checkEnemies(f.i, f.j - 1);
        checkEnemies(f.i, f.j + 1);

        int re = 0;
        bfsQueue temp;

        auto checkAndAddQ = [&] (int i, int j)
        {
            if (i >= 0 && i <= 8 && j >= 0 && j <= 8) {
                if (checkAround(f.allies, i, j) == 1) {
                    temp.i = i;
                    temp.j = j;
                    temp.deep = f.deep + 1;
                    temp.allies = f.allies;
                    bfsQ.push(temp);
                }
            }
        };

        checkAndAddQ(f.i - 1, f.j);
        checkAndAddQ(f.i + 1, f.j);
        checkAndAddQ(f.i, f.j - 1);
        checkAndAddQ(f.i, f.j + 1);
    }
}

int main() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (table[i][j] == ' ') continue;
            if (isupper(table[i][j])) continue;
            int type = typeChar2Num(table[i][j]);
            numEnemies[type]++;
        }
    }

    while (numEnemies[0] != 0 && numEnemies[1] != 0 && numEnemies[2] != 0) {
        resetCost();
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (table[i][j] == 'A') {
                    bfs(i, j, 'A');
                    printCost();
                    printTable();
                    resetCost();
                }
            }
        }
        getch();
    }
}
