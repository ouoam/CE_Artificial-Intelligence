#include <cstdio>
#include <cctype>
#include <cstring>
#include <queue>
#include <vector>

/// 0 -> L
/// 1 -> S
/// 2 -> A

struct doMap;
struct bfsQueue;

/// Prototype function

int typeChar2Num(char t);
char typeNum2Char(int t);
int win(char allies, char enemies);
bfsQueue *bfs(char tables[8][8]);

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

struct doMap {
    int tablesCost[8][8];
    char tables[8][8];
    char playAllies;

    int iS = -1;
    int jS = -1;

    int iE = -1;
    int jE = -1;

    int iK = -1;
    int jK = -1;

    struct doMap *baseMap;

    doMap(struct doMap *baseMapIn, char allies, char oldMap[8][8]) :
        playAllies(allies), baseMap(baseMapIn)
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                this->tablesCost[i][j] = 5000;
                this->tables[i][j] = oldMap[i][j];
            }
        }
    }

    doMap(struct doMap *baseMapIn, char allies, char *oldMap[8]) :
        playAllies(allies), baseMap(baseMapIn)
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

    int checkAround(char allies, int i, int j) {
        if (this->tables[i][j] != ' ') return 0;

        auto checkA = [&] (int iC, int jC)
        {
            if (iC >= 0 && iC < 8 && jC >= 0 && jC < 8) {
                if (win(allies, this->tables[iC][jC]) < 0) return -1;
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
};

struct bfsQueue {
    int i = -1;
    int j = -1;
    int deep = -1;
    int round = -1;

    struct doMap *playMap;

    bfsQueue() : playMap(nullptr) {
    }
};

bfsQueue *bfs(char tables[8][8]) {
    int numEnemies[3] = {0, 0, 0};
    std::queue<bfsQueue> bfsQ;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tables[i][j] == ' ') continue;
            if (isupper(tables[i][j])) continue;
            int type = typeChar2Num(tables[i][j]);
            numEnemies[type]++;
        }
    }

    int valRound = 0;
    int valCost = 5000;

    bfsQueue *lastWinQ = nullptr;

    int alliesI = 0;

    while (numEnemies[0] > 0 || numEnemies[1] > 0 || numEnemies[2] > 0) {
        { /// find character to find in map
            /// Get char of character to play
            char allies = typeNum2Char(alliesI);
            allies = toupper(allies);
            bool isRun = false;
            char *findTable[8];
            for (int i = 0; i < 8; i++) {
                if (lastWinQ != nullptr) {
                    findTable[i] = lastWinQ->playMap->tables[i];
                } else {
                    findTable[i] = tables[i];
                }
            }

            /// find character in map
            for (int i = 0; i < 8 && !isRun; i++) {
                for (int j = 0; j < 8 && !isRun; j++) {
                    if (findTable[i][j] == allies) {
                        /// copy map
                        struct doMap *nextMap;

                        if (lastWinQ != nullptr) {
                            nextMap = new doMap(lastWinQ->playMap->baseMap, allies, findTable);
                        } else {
                            nextMap = new doMap(nullptr, allies, findTable);
                        }
                        nextMap->iS = i;
                        nextMap->jS = j;
                        if (lastWinQ != nullptr) {
                            nextMap->iE = lastWinQ->playMap->iE;
                            nextMap->jE = lastWinQ->playMap->jE;
                            nextMap->iK = lastWinQ->playMap->iK;
                            nextMap->jK = lastWinQ->playMap->jK;
                        }

                        /// add queue data
                        bfsQueue temp;

                        temp.i = i;
                        temp.j = j;
                        if (lastWinQ != nullptr) {
                            temp.deep = lastWinQ->deep;
                            temp.round = lastWinQ->round;
                        } else {
                            temp.deep = 0;
                            temp.round = 1;
                        }
                        temp.playMap = nextMap;
                        bfsQ.push(temp);

                        isRun = true;
                    }
                }
            }
            alliesI++;
            alliesI %= 3;
        }

        while (!bfsQ.empty())
        {
            /// get front queue
            bfsQueue f = bfsQ.front();
            bfsQ.pop();

            /// filter not use queue
            if (f.i < 0 || 7 < f.i || f.j < 0 || 7 < f.j) continue;
            if (f.playMap->tablesCost[f.i][f.j] <= f.deep) continue;
            if (f.round == valRound && f.deep > valCost) continue;

            f.playMap->tablesCost[f.i][f.j] = f.deep;

            bool isFound = false;

            auto checkEnemies = [&] (int iEnemies, int jEnemies)
            {
                if (iEnemies >= 0 && iEnemies < 8 && jEnemies >= 0 && jEnemies < 8 && !isFound) {
                    if (win(f.playMap->playAllies, f.playMap->tables[iEnemies][jEnemies]) == 1) {
                        isFound = true;

                        if (valRound != f.round) {
                            valRound = f.round;
                            valCost = f.deep;
                            numEnemies[typeChar2Num(f.playMap->tables[iEnemies][jEnemies])]--;
                        }
                        struct doMap *nextMap = new doMap(f.playMap, f.playMap->playAllies, f.playMap->tables);

                        nextMap->tables[f.playMap->iS][f.playMap->jS] = ' ';
                        nextMap->tables[f.i][f.j] = f.playMap->playAllies;
                        nextMap->tables[iEnemies][jEnemies] = ' ';
                        nextMap->iS = f.i;
                        nextMap->jS = f.j;
                        nextMap->iE = f.i;
                        nextMap->jE = f.j;
                        nextMap->iK = iEnemies;
                        nextMap->jK = jEnemies;
                        bfsQueue *temp = new bfsQueue();

                        temp->i = f.i;
                        temp->j = f.j;
                        temp->deep = f.deep + 1;
                        temp->round = f.round + 1;
                        temp->playMap = nextMap;
                        bfsQ.push(*temp);

                        if (lastWinQ != nullptr) {
                            delete lastWinQ;
                        }
                        lastWinQ = temp;
                    }
                }
            };

            checkEnemies(f.i - 1, f.j);
            checkEnemies(f.i + 1, f.j);
            checkEnemies(f.i, f.j - 1);
            checkEnemies(f.i, f.j + 1);

            if (!isFound) {
                bfsQueue temp;

                auto checkAndAddQ = [&] (int i, int j)
                {
                    if (i >= 0 && i < 8 && j >= 0 && j < 8) {
                        if (f.playMap->checkAround(f.playMap->playAllies, i, j) == 1) {
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
    }

    return lastWinQ;
}

struct doStore {
    unsigned char action    : 1; /// 0 -> walk  1 -> kill
    unsigned char isSet     : 1;
    unsigned char allies    : 2; /// 0 -> L  1 -> S  2 -> A
    unsigned char direction : 2; /// 0 -> up  1 -> down  2 -> left  3 -> right
    unsigned char none      : 2;

    doStore() : action(0), isSet(0), allies(0), direction(0), none(0)  {}
};

int main() {
    char table[8][8] = {
        {'a', 'l', 's', ' ', ' ', ' ', ' ', ' '},
        {'a', 'l', 'a', ' ', ' ', ' ', ' ', ' '},
        {'s', 's', 'l', ' ', ' ', ' ', ' ', ' '},
        {'a', 'l', 's', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'S'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'A'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'L'}
    };

    bfsQueue *lastWinQ =  bfs(table);

    doStore *dstore = nullptr;
    int outSize = 0;
    if (lastWinQ->playMap != nullptr) {
        struct doMap *pMap = lastWinQ->playMap;
        struct doMap *lMap = nullptr;

        outSize = pMap->tablesCost[pMap->iS][pMap->jS];
        dstore = new doStore[outSize + 1];

        lMap = pMap;
        pMap = pMap->baseMap;
        while(pMap != nullptr) {
            // printf("S(%2d,%2d) E(%2d,%2d) K(%2d,%2d) C %c\n", pMap->iS, pMap->jS, lMap->iE, lMap->jE, lMap->iK, lMap->jK, pMap->playAllies);
            // pMap->printTableCost();

            int iE = lMap->iE;
            int jE = lMap->jE;
            int cost = pMap->tablesCost[iE][jE];
            int typeNum = typeChar2Num(pMap->playAllies);

            /// Store kill action
            dstore[cost].action = 1;
            dstore[cost].isSet = 1;
            dstore[cost].allies = typeNum;
            dstore[cost].direction = (lMap->jE - lMap->jK == 0 ? (lMap->iE - lMap->iK == -1 ? 1 : 0) : (1 << 1) | (lMap->jE - lMap->jK == -1 ? 1 : 0));

            while (iE != pMap->iS || jE != pMap->jS) {
                auto check = [&] (int i, int j, int direction)
                {
                    if (i >= 0 && i < 8 && j >= 0 && j < 8) {
                        if (pMap->tablesCost[i][j] == cost - 1) {
                            cost--;
                            iE = i;
                            jE = j;

                            /// Store walk action
                            dstore[cost].action = 0;
                            dstore[cost].isSet = 1;
                            dstore[cost].allies = typeNum;
                            dstore[cost].direction = direction;
                            return true;
                        }
                    }
                    return false;
                };
                if      (check(iE - 1, jE, 1)) continue;
                else if (check(iE + 1, jE, 0)) continue;
                else if (check(iE, jE - 1, 3)) continue;
                else if (check(iE, jE + 1, 2)) continue;
            }

            lMap = pMap;
            pMap = pMap->baseMap;
        }

        for (int i = 0; i < outSize; i++) {
            if (dstore[i].isSet) {
                unsigned char temp;
                memcpy(&temp, &dstore[i], 1);
                printf("%d -> %u %u %u  %02X\n", i, dstore[i].action, dstore[i].allies, dstore[i].direction, temp);
            } else {
                printf("%d -> Not Set\n", i);
            }
        }
    } else {
        printf("Some thing bug");
    }
}
