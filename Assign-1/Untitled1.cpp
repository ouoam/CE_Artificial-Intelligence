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
struct doStore;

/// Prototype function

int typeChar2Num(char t);
char typeNum2Char(int t);
int win(char allies, char enemies);
doStore *bfs(char tables[8][8]);
unsigned char *findPath(char inTable[]);

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
        // printf("\n\nPlay : '%c'\t\t(%d, %d)", playAllies, iS, jS);
        printf("\n---+---+---+---+---+---+---+---+\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf(" %c |", this->tables[i][j]);
            }
            printf("\n---+---+---+---+---+---+---+---+\n");
        }
    }

    void printTableCost() {
        printf("\n\nS(%2d,%2d) E(%2d,%2d) K(%2d,%2d) C %c\n", iS, jS, iE, jE, iK, jK, playAllies);
        printf("\nPlay : '%c'\t\t(%d, %d)", playAllies, iS, jS);
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

struct doStore {
    unsigned char action    : 1; /// 0 -> walk  1 -> kill
    unsigned char isSet     : 1;
    unsigned char allies    : 2; /// 0 -> L  1 -> S  2 -> A
    unsigned char direction : 2; /// 0 -> up  1 -> down  2 -> left  3 -> right
    unsigned char none      : 2;

    doStore() : action(0), isSet(0), allies(0), direction(0), none(0)  {}
};

doStore *bfs(char tables[8][8]) {
    int numEnemies[3] = {0, 0, 0};
    std::queue<bfsQueue> bfsQ;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tables[i][j] == ' ') continue;
            if (isupper(tables[i][j])) continue;
            if (tables[i][j] == '#') continue;
            int type = typeChar2Num(tables[i][j]);
            numEnemies[type]++;
        }
    }

    int valRound = 0;
    int valCost = 5000;

    bfsQueue *lastWinQ = nullptr;

    int alliesI = 0;
    int lastFinishAllies = -1;

    while (numEnemies[0] > 0 || numEnemies[1] > 0 || numEnemies[2] > 0) {
        if (lastFinishAllies == alliesI) break;
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

                        lastFinishAllies = typeChar2Num(f.playMap->playAllies);
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

    doStore *dstore = nullptr;
    /// parse table array to action array
    if (lastWinQ->playMap != nullptr) {
        struct doMap *pMap = lastWinQ->playMap;
        struct doMap *lMap = nullptr;

        dstore = new doStore[pMap->tablesCost[pMap->iS][pMap->jS] + 1];

        lMap = pMap;
        pMap = pMap->baseMap;
        while(pMap != nullptr) {
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
    } else {
        printf("BFS : can not find path\n");
    }

    return dstore;
}

int dfsr(int i, int j, doMap *Map, int deep) {
    /// filter not use queue
    if (i < 0 || 7 < i || j < 0 || 7 < j) return 0;
    if (Map->tablesCost[i][j] <= deep) return 0;

    Map->tablesCost[i][j] = deep;

    bool isFound = false;
    auto checkEnemies = [&] (int iEnemies, int jEnemies)
    {
        if (iEnemies >= 0 && iEnemies < 8 && jEnemies >= 0 && jEnemies < 8 && !isFound) {
            if (win(Map->playAllies, Map->tables[iEnemies][jEnemies]) == 1) {
                isFound = true;
                ///-------------------  FOUND  --------------------
                Map->iE = i;
                Map->jE = j;
                Map->iK = iEnemies;
                Map->jK = jEnemies;
                return 1;
            }
        }
        return 0;
    };

    if (checkEnemies(i - 1, j) ||
        checkEnemies(i + 1, j) ||
        checkEnemies(i, j - 1) ||
        checkEnemies(i, j + 1))
    {
        return 1;
    }

    bfsQueue temp;

    auto checkAndGo = [&] (int iGo, int jGo)
    {
        if (iGo >= 0 && iGo < 8 && jGo >= 0 && jGo < 8) {
            if (Map->checkAround(Map->playAllies, iGo, jGo) == 1) {
                return dfsr(iGo, jGo, Map, deep + 1);
            }
        }
        return 0;
    };

    if (checkAndGo(i - 1, j) ||
        checkAndGo(i + 1, j) ||
        checkAndGo(i, j - 1) ||
        checkAndGo(i, j + 1))
    {
        return 1;
    }
    return 0;
}

doStore *dfs(char tables[8][8]) {
    int numEnemies[3] = {0, 0, 0};
    int posA[3][2] = {0, 0, 0, 0, 0, 0};

    for (int a = 0; a < 3; a++) {
        char allies = typeNum2Char(a);
        allies = toupper(allies);

        bool isRun = false;
        for (int i = 0; i < 8 && !isRun; i++) {
            for (int j = 0; j < 8 && !isRun; j++) {
                if (tables[i][j] == allies) {
                    posA[a][0] = i;
                    posA[a][1] = j;
                    isRun = true;
                }
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tables[i][j] == ' ') continue;
            if (isupper(tables[i][j])) continue;
            if (tables[i][j] == '#') continue;
            int type = typeChar2Num(tables[i][j]);
            numEnemies[type]++;
        }
    }

    int alliesI = 0;
    int lastFinishAllies = -1;

    doMap *beforeMap = nullptr;

    while (numEnemies[0] > 0 || numEnemies[1] > 0 || numEnemies[2] > 0) {
        if (numEnemies[alliesI] > 0) {

            char allies = typeNum2Char(alliesI);
            allies = toupper(allies);

            doMap *nextMap = nullptr;

            int nextDeep = 0;

            if (beforeMap == nullptr) {
                nextMap = new doMap(beforeMap, allies, tables);
            } else {
                nextMap = new doMap(beforeMap, allies, beforeMap->tables);
                nextMap->tables[beforeMap->iK][beforeMap->jK] = ' ';
                nextMap->tables[beforeMap->iE][beforeMap->jE] = beforeMap->playAllies;
                nextMap->tables[beforeMap->iS][beforeMap->jS] = ' ';
                nextDeep = beforeMap->tablesCost[beforeMap->iE][beforeMap->jE] + 1;
            }

            nextMap->iS = posA[alliesI][0];
            nextMap->jS = posA[alliesI][1];
            nextMap->playAllies = allies;
            int res = dfsr(nextMap->iS, nextMap->jS, nextMap, nextDeep);
            if (res == 1) {
                posA[alliesI][0] = nextMap->iE;
                posA[alliesI][1] = nextMap->jE;
                numEnemies[alliesI]--;
                beforeMap = nextMap;
                // beforeMap->printTableCost();
                lastFinishAllies = alliesI;
            } else {
                delete nextMap;
                if (lastFinishAllies == alliesI) break;
            }
        }

        alliesI++;
        alliesI %= 3;
    }

    doStore *dstore = nullptr;

    if (beforeMap != nullptr) {
        dstore = new doStore[beforeMap->tablesCost[beforeMap->iE][beforeMap->jE] + 2];

        doMap *pMap = beforeMap;
        while (pMap != nullptr) {
            int iE = pMap->iE;
            int jE = pMap->jE;
            int cost = pMap->tablesCost[iE][jE];
            int typeNum = typeChar2Num(pMap->playAllies);

            /// Store kill action
            dstore[cost].action = 1;
            dstore[cost].isSet = 1;
            dstore[cost].allies = typeNum;
            dstore[cost].direction = (pMap->jE - pMap->jK == 0 ? (pMap->iE - pMap->iK == -1 ? 1 : 0) : (1 << 1) | (pMap->jE - pMap->jK == -1 ? 1 : 0));

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

            pMap = pMap->baseMap;
        }
    } else {
        printf("DFS : can not find path\n");
    }
    return dstore;
}


unsigned char *findPath(char inTable[]) {
    char table[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            table[i][j] = inTable[i*8 + j];
        }
    }

    doStore *dstore =  bfs(table);

    int c = 0;
    while (dstore[c++].isSet != 0);

    unsigned char *out = new unsigned char[c + 1];
    memcpy(out, dstore, c);

    return out;
}

int main() {
    char table[8][8] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', 's', ' ', 'a', ' ', ' ', 'l', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'S', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'L'},
        {' ', ' ', ' ', 'A', ' ', ' ', ' ', ' '}
    };

    doStore *dstore =  dfs(table);

    int posA[3][2];
    doMap Maps = doMap(nullptr, ' ', table);

    for (int a = 0; a < 3; a++) {
        char allies = typeNum2Char(a);
        allies = toupper(allies);

        bool isRun = false;
        for (int i = 0; i < 8 && !isRun; i++) {
            for (int j = 0; j < 8 && !isRun; j++) {
                if (Maps.tables[i][j] == allies) {
                    posA[a][0] = i;
                    posA[a][1] = j;
                    isRun = true;
                }
            }
        }
    }

    for (int i = 0; ; i++) {
        if (dstore[i].isSet) {
            unsigned char temp;
            memcpy(&temp, &dstore[i], 1);
            printf("\n\n%d -> %u %u %u  %02X\n", i, dstore[i].action, dstore[i].allies, dstore[i].direction, temp);
            char allies = typeNum2Char(dstore[i].allies);
            allies = toupper(allies);
            printf("Play : '%c'\t   ", allies);
            if (dstore[i].action == 0) {
                Maps.tables[posA[dstore[i].allies][0]][posA[dstore[i].allies][1]] = ' ';
                switch (dstore[i].direction) {
                    case 0: posA[dstore[i].allies][0]--; break;
                    case 1: posA[dstore[i].allies][0]++; break;
                    case 2: posA[dstore[i].allies][1]--; break;
                    case 3: posA[dstore[i].allies][1]++; break;
                    default: break;
                }
                Maps.tables[posA[dstore[i].allies][0]][posA[dstore[i].allies][1]] = allies;
                printf("walk to (%d,%d)", posA[dstore[i].allies][0], posA[dstore[i].allies][1]);
            } else {
                int iK = posA[dstore[i].allies][0];
                int jK = posA[dstore[i].allies][1];
                switch (dstore[i].direction) {
                    case 0: iK--; break;
                    case 1: iK++; break;
                    case 2: jK--; break;
                    case 3: jK++; break;
                    default: break;
                }
                Maps.tables[iK][jK] = ' ';
                printf("   kill (%d,%d)", iK, jK);
            }
            Maps.printTable();
        } else {
            printf("%d -> Not Set\n", i);
            break;
        }
    }
}
