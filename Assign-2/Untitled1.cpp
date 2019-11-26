// A C++ Program to implement A* Search Algorithm
#include<bits/stdc++.h>
using namespace std;

#define ROW 15
#define COL 15

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int>> pPair;

struct cell;
struct doMap;

int typeChar2Num(char t);
char typeNum2Char(int t);
int win(char allies, char enemies);
float cost(cell grid[][COL], int row, int col);
bool canWalkIn(char tile);


bool isValid(int row, int col);
bool isUnBlocked(cell grid[][COL], int row, int col);
bool isDestination(int row, int col, Pair dest);
double calculateHValue(int row, int col, Pair dest);
void tracePath(cell cellDetails[][COL], Pair dest);
void aStarSearch(char grid[][COL], Pair src, Pair dest);

// A structure to hold the neccesary parameters
struct cell
{
    char in;
	// Row and Column index of its parent
	// Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	int parent_i, parent_j;
	// f = g + h
	double f, g, h;

	cell ()
	{
	    f = FLT_MAX;
        g = FLT_MAX;
        h = FLT_MAX;
        parent_i = -1;
        parent_j = -1;
	}

	cell& operator =(const char a)
    {
        in = a;
        return *this;
    }
};


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

float cost(cell grid[][COL], int row, int col)
{
    switch (grid[row][col].in) {
        case ' ': return 1.0;
        case '*': return 4.0;
        case '%': return 7.0;
        default : return 99;
    }
}

bool canWalkIn(char tile)
{
    return tile == ' ' || tile == '*' || tile == '%';
}



// A Utility Function to check whether given cell (row, col)
// is a valid cell or not.
bool isValid(int row, int col)
{
	// Returns true if row number and column number
	// is in range
	return (row >= 0) && (row < ROW) &&
		(col >= 0) && (col < COL);
}

// A Utility Function to check whether the given cell is
// blocked or not
bool isUnBlocked(cell grid[][COL], int row, int col)
{
	// Returns true if the cell is not blocked else false
	if (canWalkIn(grid[row][col].in) || grid[row][col].in == 'L'|| grid[row][col].in == 's')
		return (true);
	else
		return (false);
}

struct doMap {
    cell tables[ROW][COL];
    char playAllies;

    Pair Start;
    Pair End;
    Pair Kill;

    struct doMap *baseMap;

    doMap(struct doMap *baseMapIn, char allies, cell oldMap[ROW][COL]) :
        playAllies(allies), baseMap(baseMapIn)
    {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                this->tables[i][j].in = oldMap[i][j].in;
            }
        }
    }

    doMap(struct doMap *baseMapIn, char allies, char oldMap[ROW][COL]) :
        playAllies(allies), baseMap(baseMapIn)
    {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                this->tables[i][j].in = oldMap[i][j];
            }
        }
    }

    doMap(struct doMap *baseMapIn, char allies, cell *oldMap[ROW]) :
        playAllies(allies), baseMap(baseMapIn)
    {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                this->tables[i][j].in = oldMap[i][j].in;
            }
        }
    }

    void printTable() {
        // printf("\n\nPlay : '%c'\t\t(%d, %d)", playAllies, iS, jS);
        printf("\n");
        for (int j = 0; j < COL; j++) {
            printf(" %-2d|", j);
        }
        printf("\n");
        for (int j = 0; j < COL; j++) {
            printf("---+");
        }
        printf("---\n");
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                printf(" %c |", this->tables[i][j].in);
            }
            printf(" %-2d\n", i);
            for (int j = 0; j < COL; j++) {
                printf("---+");
            }
            printf("---\n");
        }
    }

    void printTableCost() {
        printf("\n\nS(%2d,%2d) E(%2d,%2d) K(%2d,%2d) C %c\n", Start.first, Start.second, End.first, End.second, Kill.first, Kill.second, playAllies);
        printf("Play : '%c'\t\t(%d, %d)\n", playAllies, Start.first, Start.second);
        printf("\n");
        for (int j = 0; j < COL; j++) {
            printf(" %-2d|", j);
        }
        printf("\n");
        for (int j = 0; j < COL; j++) {
            printf("---+");
        }
        printf("---\n");
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (this->tables[i][j].in != ' ' && this->tables[i][j].in != '*' && this->tables[i][j].in != '%') {
                    printf(" %c |", this->tables[i][j].in);
                } else if (this->tables[i][j].g != FLT_MAX) {
                    printf("%3.0f|", this->tables[i][j].g);
                } else {
                    printf("   |");
                }
            }
            printf(" %-2d\n", i);
            for (int j = 0; j < COL; j++) {
                printf("---+");
            }
            printf("---\n");
        }
    }

    int checkAround(int i, int j) {
        if (!canWalkIn(this->tables[i][j].in)) return 0;

        auto checkA = [&] (int iC, int jC)
        {
            if (isValid(iC, jC)) {
                if (win(playAllies, this->tables[iC][jC].in) < 0) return -1;
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

struct doStore {
    unsigned char action    : 1; /// 0 -> walk  1 -> kill
    unsigned char isSet     : 1;
    unsigned char allies    : 2; /// 0 -> L  1 -> S  2 -> A
    unsigned char direction : 2; /// 0 -> up  1 -> down  2 -> left  3 -> right
    unsigned char none      : 2;

    doStore() : action(0), isSet(0), allies(0), direction(0), none(0)  {}
};

// A Utility Function to check whether destination cell has
// been reached or not
bool isDestination(int row, int col, Pair dest)
{
	if (row == dest.first && col == dest.second)
		return (true);
	else
		return (false);
}

// A Utility Function to calculate the 'h' heuristics.
double calculateHValue(int row, int col, Pair dest[], int numDest)
{
	// Return using the distance formula
//	return ((double)sqrt ((row-dest.first)*(row-dest.first)
//						+ (col-dest.second)*(col-dest.second)));
    double minVal = FLT_MAX;
    for (int i = 0; i < numDest; i++) {
        double calc = abs(row - dest[i].first) + abs(col - dest[i].second);
        if (minVal > calc || minVal == FLT_MAX) {
            minVal = calc;
        }
    }
    return minVal;
}

// A Utility Function to trace the path from the source
// to destination
void tracePath(cell cellDetails[][COL], Pair dest)
{
	printf ("\nThe Path is ");
	int row = dest.first;
	int col = dest.second;

	stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
			&& cellDetails[row][col].parent_j == col ))
	{
		Path.push (make_pair (row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	Path.push (make_pair (row, col));
	while (!Path.empty())
	{
		pair<int,int> p = Path.top();
		Path.pop();
		printf("-> (%d,%d) ",p.first,p.second);
	}

	return;
}

// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
int aStarSearch(doMap *Map, Pair dest[], int numDest)
{
	// If the source is out of range
//	if (isValid (src.first, src.second) == false)
//	{
//		printf ("Source is invalid\n");
//		return;
//	}

	// If the destination is out of range
//	if (isValid (dest.first, dest.second) == false)
//	{
//		printf ("Destination is invalid\n");
//		return;
//	}

	// Either the source or the destination is blocked
//	if (isUnBlocked(grid, src.first, src.second) == false ||
//			isUnBlocked(grid, dest.first, dest.second) == false)
//	{
//		printf ("Source or the destination is blocked\n");
//		return;
//	}

	// If the destination cell is the same as source cell
//	if (isDestination(src.first, src.second, dest) == true)
//	{
//		printf ("We are already at the destination\n");
//		return;
//	}

	// Create a closed list and initialise it to false which means
	// that no cell has been included yet
	// This closed list is implemented as a boolean 2D array
	bool closedList[ROW][COL];
	memset(closedList, false, sizeof (closedList));

	// Declare a 2D array of structure to hold the details
	//of that cell
	//cell cellDetails[ROW][COL];

	int i, j;

	// Initialising the parameters of the starting node
	i = Map->Start.first, j = Map->Start.second;
	Map->tables[i][j].f = 0.0;
	Map->tables[i][j].g = 0.0;
	Map->tables[i][j].h = 0.0;
	Map->tables[i][j].parent_i = i;
	Map->tables[i][j].parent_j = j;

	/*
	Create an open list having information as-
	<f, <i, j>>
	where f = g + h,
	and i, j are the row and column index of that cell
	Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	This open list is implenented as a set of pair of pair.*/
	set<pPair> openList;

	// Put the starting cell on the open list and set its
	// 'f' as 0
	openList.insert(make_pair (0.0, make_pair (i, j)));

	// We set this boolean value as false as initially
	// the destination is not reached.
	bool foundDest = false;

	auto successor = [&] (int iGo, int jGo)
    {
        // To store the 'g', 'h' and 'f' of the 8 successors
		double gNew, hNew, fNew;

        // Only process this cell if this is a valid one
        if (isValid(iGo, jGo) == true)
        {
            // If the destination cell is the same as the
            // current successor
            for (int k = 0; k < numDest; k++) {
                if (isDestination(iGo, jGo, dest[k]) == true)
                {
                    // Set the Parent of the destination cell
                    Map->tables[iGo][jGo].parent_i = i;
                    Map->tables[iGo][jGo].parent_j = j;

                    // printf ("The destination cell is found\n");
                    Map->Kill = dest[k];
                    Map->End = make_pair(i, j);
                    Map->printTable();
                    // tracePath (Map->tables, dest[k]);
                    foundDest = true;
                    return true;
                }
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            if (closedList[iGo][jGo] == false &&
                    Map->checkAround(iGo, jGo) == 1)
            {
                gNew = Map->tables[i][j].g + cost(Map->tables, iGo, jGo);
                hNew = calculateHValue (iGo, jGo, dest, numDest);
                fNew = gNew + hNew;

                // If it isn�t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //			 OR
                // If it is on the open list already, check
                // to see if this path to that square is better,
                // using 'f' cost as the measure.
                if (Map->tables[iGo][jGo].f == FLT_MAX ||
                        Map->tables[iGo][jGo].f > fNew)
                {
                    openList.insert( make_pair(fNew, make_pair(iGo, jGo)));

                    // Update the details of this cell
                    Map->tables[iGo][jGo].f = fNew;
                    Map->tables[iGo][jGo].g = gNew;
                    Map->tables[iGo][jGo].h = hNew;
                    Map->tables[iGo][jGo].parent_i = i;
                    Map->tables[iGo][jGo].parent_j = j;
                }
            }
        }
        return false;
    };

	while (!openList.empty())
	{
		pPair p = *openList.begin();

		// Remove this vertex from the open list
		openList.erase(openList.begin());

		// Add this vertex to the closed list
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

	/*
		Generating all the 8 successor of this cell

			 N.W  N  N.E
			   \  |  /
                \ | /
			W----Cell----E
				/ | \
               /  |  \
			 S.W  S  S.E

		Cell-->Popped Cell (i, j)
		N --> North	 (i-1, j)
		S --> South	 (i+1, j)
		E --> East	 (i, j+1)
		W --> West		 (i, j-1)
		N.E--> North-East (i-1, j+1)
		N.W--> North-West (i-1, j-1)
		S.E--> South-East (i+1, j+1)
		S.W--> South-West (i+1, j-1)*/



		//----------- 1st Successor (North) ------------
        if (successor(i - 1, j)) return 1;

		//----------- 2nd Successor (South) ------------
		if (successor(i + 1, j)) return 1;

		//----------- 3rd Successor (East) ------------
        if (successor(i, j + 1)) return 1;

		//----------- 4th Successor (West) ------------
        if (successor(i, j - 1)) return 1;

//		//----------- 5th Successor (North-East) ------------
//        if (successor(i - 1, j + 1)) return;
//
//		//----------- 6th Successor (North-West) ------------
//        if (successor(i - 1, j - 1)) return;
//
//		//----------- 7th Successor (South-East) ------------
//        if (successor(i + 1, j + 1)) return;
//
//		//----------- 8th Successor (South-West) ------------
//        if (successor(i + 1, j - 1)) return;

        // Map->printTableCost();
	}


	// When the destination cell is not found and the open
	// list is empty, then we conclude that we failed to
	// reach the destiantion cell. This may happen when the
	// there is no way to destination cell (due to blockages)
	if (foundDest == false)
		printf("Failed to find the Destination Cell\n");

	return 0;
}

doStore *Astar(char tables[ROW][COL]) {
    int numEnemies[3] = {0, 0, 0};
    Pair posA[3];

    for (int a = 0; a < 3; a++) {
        char allies = typeNum2Char(a);
        allies = toupper(allies);

        bool isRun = false;
        for (int i = 0; i < ROW && !isRun; i++) {
            for (int j = 0; j < COL && !isRun; j++) {
                if (tables[i][j] == allies) {
                    posA[a] = make_pair(i, j);
                    isRun = true;
                }
            }
        }
    }

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            int type = typeChar2Num(tables[i][j]);
            if (isupper(tables[i][j])) continue;
            if (type == 3) continue;
            numEnemies[type]++;
        }
    }

    int alliesI = 0;
    int lastFinishAllies = -1;

    doMap *beforeMap = nullptr;

    while (numEnemies[0] > 0 || numEnemies[1] > 0 || numEnemies[2] > 0) {
        char allies = typeNum2Char(alliesI);
        allies = toupper(allies);
        char muchWin = win(allies, 'l') == 1 ? 'l' : (win(allies, 's') == 1 ? 's' : win(allies, 'a') == 1 ? 'a' : ' ');
        int muchWinType = typeChar2Num(muchWin);

        if (numEnemies[muchWinType] > 0) {
            doMap *nextMap = nullptr;

            if (beforeMap == nullptr) {
                nextMap = new doMap(beforeMap, allies, tables);
            } else {
                nextMap = new doMap(beforeMap, allies, beforeMap->tables);
                nextMap->tables[beforeMap->Kill.first][beforeMap->Kill.second] = ' ';
                nextMap->tables[beforeMap->End.first][beforeMap->End.second] = beforeMap->playAllies;

                char mapAtFirst = tables[beforeMap->Start.first][beforeMap->Start.second];
                if (mapAtFirst == ' ' || mapAtFirst == '#' || mapAtFirst == '*' || mapAtFirst == '%') {
                    nextMap->tables[beforeMap->Start.first][beforeMap->Start.second] = mapAtFirst;
                } else {
                    nextMap->tables[beforeMap->Start.first][beforeMap->Start.second] = ' ';
                }

            }
            nextMap->Start = posA[alliesI];
            nextMap->playAllies = allies;

            Pair posE[3];
            int posEi = 0;

            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++) {
                    if (nextMap->tables[i][j].in == muchWin) {
                        posE[posEi] = make_pair(i, j);
                        posEi++;
                    }
                }
            }

            if (aStarSearch(nextMap, posE, posEi)) {
                posA[alliesI] = nextMap->End;
                numEnemies[muchWinType]--;
                beforeMap = nextMap;
                lastFinishAllies = alliesI;
            } else {
                delete nextMap;
                if (lastFinishAllies == alliesI) {
                    printf("!!Can not find!!\n");
                    break;
                }
            }
        }

        alliesI++;
        alliesI %= 3;
    }
}

unsigned char *runAndcopyAstart(char table[ROW][COL]) {
	doStore *dstore =  Astar(table);

    int c = 0;
    while (dstore[c++].isSet != 0);

    unsigned char *out = new unsigned char[c + 1];
    memcpy(out, dstore, c);
	return out;
}


// Driver program to test above function
int main()
{
	/* Description of the Grid-
	1--> The cell is not blocked
	0--> The cell is blocked */
	char grid[ROW][COL] =
	{ //  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
        {'*',' ',' ',' ','%','*','*',' ','a','*','*',' ','%',' ',' '}, // 0
        {'*','%',' ',' ',' ','*','%',' ',' ',' ','*',' ','*',' ',' '}, // 1
        {' ','s','%',' ','*',' ',' ',' ','*','%','%','*',' ',' ',' '}, // 2
        {'%','%','*',' ','%','l',' ','%','s','*','%',' ','%','a',' '}, // 3
        {' ',' ','*',' ','%','a','l','*','%','%',' ',' ',' ','%',' '}, // 4
        {'%',' ',' ',' ','*','%','%','*','%',' ',' ','s',' ','%',' '}, // 5
        {' ','l',' ',' ','*',' ','*','*',' ',' ',' ',' ','%','*',' '}, // 6
        {'*','#','#',' ','%','#','#','*','#','#','#','#','*',' ',' '}, // 7
        {' ','*','*',' ','%','*',' ',' ',' ',' ',' ',' ',' ','*',' '}, // 8
        {' ',' ',' ','%',' ','*','%',' ',' ','%',' ',' ','%',' ',' '}, // 9
        {' ',' ',' ',' ','%',' ','%','%','*',' ','*','*','*','%',' '}, // 10
        {'%',' ',' ','*',' ','%',' ','%',' ',' ','%','S',' ','A',' '}, // 11
        {'*',' ',' ',' ','*',' ','%','*','%','%','%','%','*','*',' '}, // 12
        {'*',' ',' ',' ','%','%','*','%','L',' ','*','*',' ','*',' '}, // 13
        {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}  // 14
    };

//	// Source is the left-most bottom-most corner
//	Pair src = make_pair(13, 8);
//
//	// Destination is the left-most top-most corner
//	Pair dest = make_pair(2, 1);
//
//	aStarSearch(grid, src, dest);

	runAndcopyAstart(grid);

	return(0);
}
