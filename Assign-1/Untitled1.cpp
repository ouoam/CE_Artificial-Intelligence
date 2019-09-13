#include <cstdio>
#include <cctype>
#include <queue>
#include <vector>

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

int numEnemies[3];

struct doMap {
    int tablesCost[8][8];
    char tables[8][8];
    char playAllies;

    int iS;
    int jS;

    struct doMap *baseMap;

    doMap(struct doMap *baseMap, char allies, char oldMap[8][8]) :
        baseMap(baseMap), playAllies(allies)
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                this->tablesCost[i][j] = 5000;
                this->tables[i][j] = oldMap[i][j];
            }
        }
    }

    void printTable() {
        printf("\n\nPlay : '%c'\t\t(%d, %d)", playAllies, iS, jS);
        printf("\n---+---+---+---+---+---+---+---+\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf(" %c |", this->tables[i][j]);
            }
            printf("\n---+---+---+---+---+---+---+---+\n");
        }
    }

    void printTableCost() {
        printf("\n\nPlay : '%c'\t\t(%d, %d)", playAllies, iS, jS);
        printf("\n---+---+---+---+---+---+---+---+\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (this->tables[i][j] != ' ') {
                    printf(" %c |", this->tables[i][j]);
                } else if (this->tablesCost[i][j] != 5000) {
                    printf("%3d|", this->tablesCost[i][j]);
                } else {
                    printf("   |");
                }
            }
            printf("\n---+---+---+---+---+---+---+---+\n");
        }
    }

    void printCost() {
        printf("\n-----+-----+-----+-----+-----+-----+-----+-----+\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf(" %4d|", this->tablesCost[i][j]);
            }
            printf("\n-----+-----+-----+-----+-----+-----+-----+-----+\n");
        }
    }
};

struct bfsQueue {
    int i;
    int j;
    int deep;
    int round;

    struct doMap *playMap;

    bfsQueue() : playMap(nullptr) {
    }
};

std::queue<bfsQueue> bfsQ;

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
    {
        bfsQueue q;
        q.i = i;
        q.j = j;
        q.deep = 0;
        q.round = 1;
        struct doMap *playMap = new doMap(nullptr, allies, table);
        playMap->iS = i;
        playMap->jS = j;
        q.playMap = playMap;
        bfsQ.push(q);
    }

    int valRound = 1;
    int valCost = 5000;

    bfsQueue *lastWin = nullptr;

    while (!bfsQ.empty())
    {
        bfsQueue f = bfsQ.front();
        bfsQ.pop();

        if (f.i < 0 || 7 < f.i || f.j < 0 || 7 < f.j) continue;

        if (f.playMap->tablesCost[f.i][f.j] <= f.deep) continue;

        if (f.round == valRound && f.deep > valCost) continue;

        f.playMap->tablesCost[f.i][f.j] = f.deep;

        // printf("In %d,%d -> %d\t%x\n", f.i, f.j, f.deep, f.playMap);

        bool isFound = false;

        auto checkEnemies = [&] (int iEnemies, int jEnemies)
        {
            if (iEnemies >= 0 && iEnemies <= 8 && jEnemies >= 0 && jEnemies <= 8 && !isFound) {
                if (win(f.playMap->playAllies, f.playMap->tables[iEnemies][jEnemies]) == 1) {
                    printf("Win %d %d -> %d %d\tcost : %d\tround : %d\n", f.i, f.j, iEnemies, jEnemies, f.deep, f.round);
                    isFound = true;

                    struct doMap *nextMap = new doMap(f.playMap, f.playMap->playAllies, f.playMap->tables);

                    nextMap->tables[f.playMap->iS][f.playMap->jS] = ' ';
                    nextMap->tables[f.i][f.j] = f.playMap->playAllies;
                    nextMap->tables[iEnemies][jEnemies] = ' ';
                    nextMap->iS = f.i;
                    nextMap->jS = f.j;

                    // f.playMap->printTableCost();
                    // nextMap->printTable();

                    bfsQueue *temp = new bfsQueue();

                    temp->i = f.i;
                    temp->j = f.j;
                    temp->deep = f.deep + 1;
                    temp->round = f.round + 1;
                    temp->playMap = nextMap;
                    bfsQ.push(*temp);

                    if (lastWin != nullptr) {
                        delete lastWin;
                    }
                    lastWin = temp;

                    valRound = f.round;
                    valCost = f.deep;
                }
            }
        };

        checkEnemies(f.i - 1, f.j);
        checkEnemies(f.i + 1, f.j);
        checkEnemies(f.i, f.j - 1);
        checkEnemies(f.i, f.j + 1);

        if (!isFound) {
            int re = 0;
            bfsQueue temp;

            auto checkAndAddQ = [&] (int i, int j)
            {
                if (i >= 0 && i <= 8 && j >= 0 && j <= 8) {
                    if (checkAround(allies, i, j) == 1) {
                        temp.i = i;
                        temp.j = j;
                        temp.deep = f.deep + 1;
                        temp.round = f.round;
                        temp.playMap = f.playMap;
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

    lastWin->playMap->printTableCost();
    lastWin->playMap->baseMap->printTableCost();
    lastWin->playMap->baseMap->baseMap->printTableCost();
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
        bool isRun = false;
        for (int i = 0; i < 8 && !isRun; i++) {
            for (int j = 0; j < 8 && !isRun; j++) {
                if (table[i][j] == 'A') {
                    bfs(i, j, 'A');
                    isRun = true;
                }
            }
        }
        getchar();
    }
}
