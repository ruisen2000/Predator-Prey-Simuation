#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
using namespace std;



struct Fox{
	int xIndex;	  //column
	int yIndex;  //row
	int fedLevel;
	bool alive;
};

struct Rabbit{
	int xIndex;
	int yIndex;
	int age;
	bool alive;

};

const int MAX_FOXES_PER_GRID = 9;
const int  MAX_RABBITS_PER_GRID = 20;

void createFoxes(int** foxGrid, Fox* foxList, int cols, int rows, int& foxListUsed, int& foxesCreated);			
void createRabbit(int** rabbitGrid, Rabbit* rabbitList, int numCols, int numRows, int& rabbitListUsed, int& rabbitsCreated);  
void PrintGrids(int **FoxBoard, int **RabbitBoard, ostream& outStream, int numRows, int numCols);
bool RabbitStep (int **rabbitBoard, Rabbit *listOfRabbits, int& numberRabbitsTotal, int& numberRabbitsAlive, int numRows, int numCols, int rabbitListLength, int maxNumRabbitsPerGrid, int numChildren);
void FoxStep (int **foxBoard, Fox *listOfFoxes, int **rabbitBoard, Rabbit *listOfRabbits, int numberFoxesTotal, int& numberFoxesAlive, int numberRabbitsTotal, int& numberRabbitsAlive, int numRows, int numCols, int foxListLength, int rabbitListLength);
int findNextLiveFox( Fox *foxList, int Row, int Col, int numberFoxesTotal, int& lastFoxIndex );
int findNextLiveRabbit( Rabbit *rabbitList, int Row, int Col, int numberRabbitsTotal, int& nextRabbitIndex );    
  


//November 23 2014

int main()
{
	int tries = 0;   //Reused for all input
	char outfile[80] = {'\0'};
	ofstream output;

	int numRows = 0;
	int numCols = 0;
	int generations = 0;
	int maxFoxes = 0;		//maximum initial number of foxes per square
	int maxRabbits = 0;		//maximum initial number of rabbits per square
	int maxChildren = 0;	//max number of children allowed when rabbits breed
	unsigned int seed = 0;	
	double fracFox = 0;		// initial fraction of squares containing foxes
	double fracRabbit = 0;  // initial fraction of squares containing rabbits
	
	

	int** foxGrid = NULL;
	int** rabbitGrid = NULL;

	Fox* foxList = NULL;	
	Rabbit* rabbitList = NULL;

	int randomNum = 0;
	double increment = 0;
	double prob = 0;
	double firstFractFox = 0;
	double firstFracRabbit = 0;
		
	int foxListSize = 0;	//size of the array foxList
	int foxListUsed = 0;  //Number of rabbits stored in foxList (includes dead foxes). Since array elements start from 0, if 10 foxes are stored, array will be filled up to element 9

	int rabbitListSize = 0;	//size of the array rabbitList
	int rabbitListUsed = 0; //Number of rabbits stored in rabbitList (includes dead rabbits). Since array elements start from 0, if 10 rabbitss are stored, array will be filled up to element 9

	int liveFoxes = 0;  //number of foxes alive
	int liveRabbits = 0; //number of rabbits alive
	
	int test = 0;

#if 0
	while (tries < 6)
	{
		cout << "Enter the name of the output file: ";
		cin >> outfile;
		output.open(outfile);

		if(output.fail() )
		{
			cerr << "ERROR: output file not opened correctly" << endl;
			tries++;
			continue;
		}
		break;
	}

	if (tries == 6)
	{
		cerr << "ERROR: could not open output file after 6 tries" << endl;
		return 1;
	}
	tries = 0;

	while (numRows <= 0 || numRows >= 15)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read number of rows after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the number of rows in the simulation grid: ";
		if ( !(cin >> numRows) )
		{
			 cerr << "ERROR: Cannot read number of rows" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (numRows <= 0 || numRows >= 15)
		{
			cerr << "ERROR: Read an illegal number of rows for the board" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (numCols <= 0 || numCols >= 15)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read number of columns after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the number of columns in the simulation grid: ";

		if ( !(cin >> numCols) )
		{
			 cerr << "ERROR: Cannot read number of columns" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (numCols <= 0 || numCols >= 15)
		{
			cerr << "ERROR: Read an illegal number of columns for the grid" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (generations <= 0 || generations >= 40)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read number of generations after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the number of generations: ";

		if ( !(cin >> generations) )
		{
			 cerr << "ERROR: Cannot read the number of generations" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (generations <= 0 || generations >= 40)
		{
			cerr << "ERROR: Read an illegal number of generations" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (seed <= 0 || seed >= RAND_MAX)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the seed for the random number generator after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the seed for the random number generator: ";

		if ( !(cin >> seed) )
		{
			 cerr << "ERROR: Cannot read the seed for the random number generator" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (seed <= 0 || seed >= RAND_MAX)
		{
			cerr << "ERROR: Read an illegal seed" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (fracFox <= 0.05 || fracFox >= 0.95)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the initial fraction of squares in the grid containing no foxes after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the initial fraction of squares in the grid containing foxes: ";

		if ( !(cin >> fracFox) )
		{
			 cerr << "ERROR: Cannot read the initial fraction of squares in the grid containing foxes" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (fracFox <= 0.05 || fracFox >= 0.95)
		{
			cerr << "ERROR: Read an illegal initial fraction of squares in the grid with foxes" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (fracRabbit <= 0.05 || fracRabbit >= 0.95)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the initial fraction of squares in the grid containing no rabbits after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the initial fraction of squares in the grid containing rabbits: ";

		if ( !(cin >> fracRabbit) )
		{
			 cerr << "ERROR: Cannot read the initial fraction of squares in the grid containing rabbits" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (fracRabbit <= 0.05 || fracRabbit >= 0.95)
		{
			cerr << "ERROR: Read an illegal initial fraction of squares in the grid containing foxes" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (maxFoxes <= 0 || maxFoxes > MAX_FOXES_PER_GRID)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the initial maximum number of foxes per square after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the initial maximum number of foxes per grid: ";

		if ( !(cin >> maxFoxes) )
		{
			 cerr << "ERROR: Cannot read the initial maximum number of foxes per grid" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (maxFoxes <= 0 || maxFoxes > MAX_FOXES_PER_GRID)
		{
			cerr << "ERROR: Read an illegal initial maximum number of foxes per square" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (maxRabbits <= 0 || maxRabbits > MAX_RABBITS_PER_GRID)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the initial maximum number of rabbits per grid after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the initial maximum number of rabbits per grid ";

		if ( !(cin >> maxRabbits) )
		{
			 cerr << "ERROR: Cannot read the initial maximum number of rabbits per grid" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (maxRabbits <= 0 || maxRabbits > MAX_RABBITS_PER_GRID)
		{
			cerr << "ERROR: Read an illegal initial maximum number of rabbits per grid" << endl;
			tries++;
			continue;
		}
	}

	tries = 0;

	while (maxChildren <= 0 || maxChildren > MAX_RABBITS_PER_GRID)
	{
		if (tries >=6)
		{
			cerr << "ERROR: could not read the maximum number of children produced when rabbits breed after 6 tries" << endl;
			output.close();
			return 2;
		}

		cout <<  "Enter the maximum number of children produced when rabbits breed ";

		if ( !(cin >> maxChildren) )
		{
			 cerr << "ERROR: Cannot read the maximum number of children produced when rabbits breed" << endl;
			 tries++;
			 cin.clear();
			 cin.sync();
			 continue;
		}
		if (maxChildren <= 0 || maxChildren > MAX_RABBITS_PER_GRID)
		{
			cerr << "ERROR: Read an illegal maximum number of children produced when rabbits breed" << endl;
			tries++;
			continue;
		}
	}
#endif

	numRows = 6;
	numCols = 5;
	generations = 4;
	seed = 432;
	fracFox = 0.2;
	fracRabbit = 0.5;
	maxRabbits = 8;
	maxFoxes = 8;
	maxChildren = 2;
	output.open("out.txt");


	srand(seed); 

	//Create a grid of foxes 
	foxGrid = new int*[numRows];


	for (int i = 0; i < numRows; i++)
	{
		foxGrid[i] = new int [numCols];
	}

	
	//Code provided for assignment
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numCols; j++ )
		{ 
			foxGrid[i][j] = maxFoxes;
			randomNum = rand();
			firstFractFox = 1-fracFox;
			for( int nFox = 0; nFox <= maxFoxes; nFox++)
			{			
				if( (double)randomNum < firstFractFox * RAND_MAX  )
				{
					foxGrid[i][j] = nFox;
					break;
				}
				firstFractFox = (1-fracFox)+ (fracFox * (nFox+1))/maxFoxes;
			}

		}
	}
			
	
	//Create each individual fox and store them in an array of structures
	foxListSize = numRows*numCols*maxFoxes*2;
	foxList = new Fox[foxListSize];
	createFoxes(foxGrid, foxList, numCols, numRows, foxListUsed, liveFoxes); //create each fox

	//Create a grid of rabbits

	rabbitGrid = new int*[numRows];


	for (int i = 0; i < numRows; i++)
	{
		rabbitGrid[i] = new int [numCols];
	}

	#if 0
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numCols; j++ )
		{ 
			randomNum = rand();
			if (randomNum < (1.0-fracRabbit)*RAND_MAX)
			{
				rabbitGrid[i][j] = 0;
			}
			else
			{
				 increment = fracRabbit / maxRabbits;        
				 prob = (double)randomNum / (double)RAND_MAX;

				 for (int a = 1; a <= maxRabbits; a++)
				 {
					 if ( prob <  (1-fracRabbit) + a * increment )
					 {
						 rabbitGrid[i][j] = a;
						 break;
					 }
					
				 }
			}
		}
	}
#endif


	//Code provided for assignment
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numCols; j++ )
		{ 
			rabbitGrid[i][j] = maxRabbits;
			randomNum = rand();
			firstFracRabbit = 1-fracRabbit;
			for( int nRab = 0; nRab <= maxRabbits; nRab++)
			{			
				if( (double)randomNum < firstFracRabbit * RAND_MAX  )
				{
					rabbitGrid[i][j] = nRab;
					break;
				}
				firstFracRabbit = (1-fracRabbit)+ (fracRabbit * (nRab+1))/maxRabbits;
			}

		}
	}

	
	//Create each individual rabbit and store them in an array of structures
	rabbitListSize = numRows*numCols*maxRabbits*20;
	rabbitList = new Rabbit[rabbitListSize];
	createRabbit(rabbitGrid, rabbitList, numCols, numRows, rabbitListUsed, liveRabbits);
	
	cout << endl << "after initialization" << endl;
	output << "after initialization" << endl;
	PrintGrids(foxGrid, rabbitGrid, output, numRows, numCols);
	PrintGrids(foxGrid, rabbitGrid, cout, numRows, numCols);

	for (int i = 0; i < generations; i++)
	{
		cout << "after rabbits die of old age and breed" << endl;
		output << "after rabbits die of old age and breed" << endl;
		if (! (RabbitStep(rabbitGrid, rabbitList, rabbitListUsed, liveRabbits, numRows, numCols, rabbitListSize, MAX_RABBITS_PER_GRID, maxChildren)) )
		{
			cerr << "Error, number of rabbits exceeded maximum capacity of the array holding rabbits";
			return 1;
		}
		PrintGrids(foxGrid, rabbitGrid, cout, numRows, numCols);

		FoxStep(foxGrid, foxList, rabbitGrid, rabbitList, foxListUsed, liveFoxes, rabbitListUsed, liveRabbits, numRows, numCols, foxListSize, rabbitListSize);
	}	

	for (int i = 0; i < numRows; i++)
	{
		delete [] foxGrid[i];
		foxGrid[i] = NULL;
		delete[] rabbitGrid[i];
		rabbitGrid[i] = NULL;
	}

	delete [] foxGrid;
	foxGrid = NULL;
	delete [] rabbitGrid;
	rabbitGrid = NULL;
	delete [] foxList;
	foxList = NULL;
	delete [] rabbitList;
	rabbitList = NULL;

	output.close();
	return 0;
}

void createFoxes(int** foxGrid, Fox* foxList, int cols, int rows, int& foxListUsed, int& foxesCreated)
{
	//foxListSize is size of the array foxList, foxesCreated is the initial amount of live foxes
	int m = 0;

	
	for (int r = 0; r < rows; r++)  //for each row in fox Grid
	{
		for (int c = 0; c < cols; c++) //for each element in the row
		{
			m = 0;
			while (m < foxGrid[r][c])
			{

				foxList[foxListUsed].xIndex = c;				
				foxList[foxListUsed].yIndex = r;
				foxList[foxListUsed].fedLevel = rand() % 10;
				foxList[foxListUsed].alive = true;
				m++;
				foxesCreated++;
				foxListUsed++;		//Since array elements start from 0, if 10 foxes are stored, array will be filled up to element 9
			}
		}
	}
	

}

void createRabbit(int** rabbitGrid, Rabbit* rabbitList, int numCols, int numRows, int& rabbitListUsed, int& rabbitsCreated)
{
	//rabbitListSize is the size of the array rabbitList, rabbitsCreated is the initial amount of live rabbits
	int m = 0;

	for (int r = 0; r < numRows; r++)  //for each row in rabbit Grid
	{
		for (int c = 0; c < numCols; c++) //for each element in the row
		{
			m = 0;
			while (m < rabbitGrid[r][c])
			{
				 rabbitList[rabbitListUsed].xIndex = c;
				 rabbitList[rabbitListUsed].yIndex = r;
				 rabbitList[rabbitListUsed].age = rand() % 10;
				 rabbitList[rabbitListUsed].alive = true;
				 rabbitsCreated++;
				 rabbitListUsed++;		//Since array elements start from 0, if 10 rabbits are stored, array will be filled up to element 9
				m++;
			}
		}
	}
	
}

void PrintGrids(int **FoxBoard, int **RabbitBoard, ostream& outStream, int numRows, int numCols)
{
	for (int r = 0; r < numRows; r++)  //for each row in Grid
	{
		for (int c = 0; c < numCols; c++) //for each element in the row
		{
			outStream << setw(3) << FoxBoard[r][c] << "," << setw(2) << RabbitBoard[r][c];
		}
		outStream << endl;
	}
	outStream << endl << endl << endl << endl << endl;

}

bool RabbitStep (int **rabbitBoard, Rabbit *listOfRabbits, int& numberRabbitsTotal, int& numberRabbitsAlive, int numRows, int numCols, int rabbitListLength, int maxNumRabbitsPerGrid, int numChildren) 
{
	//numChildren is the max amount of children rabbits can have
	//rabbitListLength records the size of the array. This value should never be changed in the function
	//numberRabbitsTotal records the number of slots of the array that is used. Since array elements start from 0, if 10 rabbits are stored, array will be filled up to element 9

	int numSpacesforChildren = 0;  //The amount of space remaining in the grid for more rabbits
	int breeding_pairs = 0;  //number of pairs of rabbits that will breed
	int currentPair = 0;	 //Current pair that is breeding
	int babies = 0;  //Number of rabbit babies each pair produces

	//Step 1: Rabbits that are too old die
	//Go through each rabbit in the array listOfRabbits
	for (int m = 0; m < numberRabbitsTotal; m++)
	{
		if(listOfRabbits[m].alive == true)
		{
			listOfRabbits[m].age++;
	
			if (listOfRabbits[m].age==11)
			{
				listOfRabbits[m].alive=false;
				numberRabbitsAlive--;
				rabbitBoard[listOfRabbits[m].yIndex][listOfRabbits[m].xIndex]-=1; 
			}
		}
	}

	//Step 2: Rabbits breed
	
	

	//In each space of the grid
	for (int r = 0; r < numRows; r++)  //for each row in rabbit Grid
	{
		for (int c = 0; c < numCols; c++) //for each element in the grid
		{
			breeding_pairs = rabbitBoard[r][c] / 2;
			numSpacesforChildren = MAX_RABBITS_PER_GRID - rabbitBoard[r][c];		//Find out how many more rabbits can fit in this grid
			currentPair = 0;

			while (currentPair < breeding_pairs && numSpacesforChildren > 0)  //for each breeding pair
			{
				
					babies= rand();
					babies = babies % (numChildren);
					babies++;   //Random number of children from 1 to max number of children (numChildren)
					
					
					if (babies > numSpacesforChildren)
					{
						babies = numSpacesforChildren;						
					}

					rabbitBoard[r][c] += babies;
					numSpacesforChildren = numSpacesforChildren - babies;
					currentPair++;

					numberRabbitsAlive += babies;
					
					
					//create rabbits that are born

					for (int i = 0; i < babies; i++)
					{
						 listOfRabbits[numberRabbitsTotal].xIndex = c;		//if numberRabbitsTotal = 10, 10 rabbits are stored and array should be filled up to element 9
						 listOfRabbits[numberRabbitsTotal].yIndex = r;
						 listOfRabbits[numberRabbitsTotal].age = 1;
						 listOfRabbits[numberRabbitsTotal].alive = true;						
						 numberRabbitsTotal++;
					}

				
			}
			
		}
	}

	if (numberRabbitsTotal > rabbitListLength)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void FoxStep (int **foxBoard, Fox *listOfFoxes, int **rabbitBoard, Rabbit *listOfRabbits, int numberFoxesTotal, int& numberFoxesAlive, int numberRabbitsTotal, int& numberRabbitsAlive, int numRows, int numCols, int foxListLength, int rabbitListLength)
{
	//numberFoxesTotal keeps track of the number of elements used in foxList
	//numberRabbitsTotal keeps track of the number of elements used in rabbitList
	//foxListLength and rabbitListLength is the size of the array foxList and rabbitList
		

	int currentFox = 0;  //CurrentFox counts the number of foxes that have had their turn at feeding. 
	int nextFox = 0;  //The location of currentFox in the array listOfFoxes
	int nextRabbit = 0;  //The location of rabbit eaten by currentFox in listOfRabbits
	int direction = 0;  //Direction that the foxes move. If fox is on the leftmost grid and it moves left, it goes to the rightmost grid. If it is on the top right grid and it moves in direction 3 it goes to bottom left grid

	//Step 1: Foxes eat rabbits

	
	for (int r = 0; r < numRows; r++)  //for each row in rabbit Grid
	{
		for (int c = 0; c < numCols; c++) //for each square in the row
		{
			currentFox = 0;
			nextFox = 0;
			nextRabbit = 0;
			while (currentFox < foxBoard[r][c])			 
			{
					
				nextFox = findNextLiveFox(listOfFoxes, r, c, numberFoxesTotal, nextFox);				//Find next fox in correct grid from foxList
				nextRabbit = findNextLiveRabbit(listOfRabbits, r, c, numberRabbitsTotal, nextRabbit);	//Find next rabbit in correct grid from rabbitList
				
				
				if (rabbitBoard[r][c] > 0)    //If there are rabbits
				{

					if (nextRabbit == -1)
					{
					cerr << "Error, cannot find the rabbit in that location in rabbitList";
					return;
					}
					if (nextFox == -1)
					{
						cerr << "Error, cannot find the fox in that location in foxList";
						return;
					}

					numberRabbitsAlive--;  //fox eats rabbit	
					rabbitBoard[r][c]--;

					listOfRabbits[nextRabbit].alive = false; //rabbit is eaten
					

					listOfFoxes[nextFox].fedLevel++;
				}
				else
				{
					//No rabbits, fox fedlevel decreases, and fox dies if fedLevel < 0

					listOfFoxes[nextFox].fedLevel--;
					if (listOfFoxes[nextFox].fedLevel<=0)
					{
						listOfFoxes[nextFox].alive = false;
						numberFoxesAlive--;
						foxBoard[r][c]--;
					}
				}

					
				nextFox++;
				nextRabbit++;
				currentFox++;
			}
		}
	}
	cout << "after foxes eat rabbits" << endl;
	PrintGrids(foxBoard, rabbitBoard, cout, numRows, numCols);

	//Step 2: Foxes move after all foxes have eaten.  Start from first element in foxList and go through each fox 
					
		//For each live fox in listOfFoxes
	for (int i = 0; i < numberFoxesTotal; i++)
	{
		if (listOfFoxes[i].alive = true)
		{
			direction=rand();
			direction %= 8;

			if (direction == 0)
			{
				//If at bottom square

				if (listOfFoxes[i].yIndex == (numRows - 1) )
				{
					//If top square is full
					if (foxBoard[0][listOfFoxes[i].xIndex] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else //move to top square
					{
						foxBoard[0][listOfFoxes[i].xIndex]++;						//top square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;	//original square has 1 less fox
						listOfFoxes[i].yIndex = 0;									//update fox location
					}

				}
				//If square below is full
				else if (foxBoard[listOfFoxes[i].yIndex+1][listOfFoxes[i].xIndex] == MAX_FOXES_PER_GRID)
				{
					continue;	//does not move
				}
				else //move to square below
				{
					foxBoard[listOfFoxes[i].yIndex+1][listOfFoxes[i].xIndex]++;		//square below gets 1 more fox
					foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
					listOfFoxes[i].yIndex++;										//Update fox location
				}
			}
			else if (direction == 1)
			{
				//if fox is at corner square
				if (listOfFoxes[i].yIndex == (numRows - 1)  && listOfFoxes[i].xIndex == (numCols - 1) )
				{
					//check is diagonally opposite corner is full
					if (foxBoard[0][0] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else	//move to diagonally opposite corner
					{
						foxBoard[0][0]++;												//diagonally opposite square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = 0;										//Update fox location
						listOfFoxes[i].xIndex = 0;
					}
				}

				//if fox is at right hand edge of square, move to a left hand edge square one row below
				else if (listOfFoxes[i].xIndex == (numCols - 1) )
				{
					//check if left hand edge square is full
					if (foxBoard[listOfFoxes[i].yIndex + 1][0] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to left hand edge square the row below
					{
						foxBoard[listOfFoxes[i].yIndex + 1][0]++;						//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = 0;										//Update fox location
						listOfFoxes[i].yIndex++;	
					}
				}

				//if fox is at bottom row of the grid
				else if (listOfFoxes[i].yIndex == (numRows - 1) )
				{
					//check if top square 1 column to the right is full
					if (foxBoard[0][listOfFoxes[i].xIndex + 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to top square
					{
						foxBoard[0][listOfFoxes[i].xIndex + 1]++;						//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = 0;										//Update fox location
						listOfFoxes[i].xIndex++;	
					}
				}

				//if fox is not at corners, move diagonally down right 

				else
				{
					//check new square is full
					
					if (foxBoard[listOfFoxes[i].yIndex+1][listOfFoxes[i].xIndex+1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex + 1][listOfFoxes[i].xIndex + 1]++;	//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;			//original square loses 1 fox
						listOfFoxes[i].xIndex += 1;										//Update fox location
						listOfFoxes[i].yIndex += 1;	
					}
				}

			}
			else if (direction == 2)
			{
								
				//if fox is at right hand edge of square, move to left hand edge of same row
				if (listOfFoxes[i].xIndex == (numCols - 1) )		//checks if fox at right had edge
				{
					//check if left hand edge square is full
					if (foxBoard[listOfFoxes[i].yIndex][0] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to left hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex][0]++;							//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = 0;										//Update fox location
						
					}
				}

				//if fox is not at edge, move right 

				else
				{
					//check new square is full
					if (foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex + 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to square on the right
					{
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex + 1]++;	//new square on the right gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;			//original square loses 1 fox
						listOfFoxes[i].xIndex += 1;										//Update fox location
						
					}
				}


			}
			else if (direction==3)
			{
				
				//if fox is at top right corner square
				if (listOfFoxes[i].yIndex == 0  && listOfFoxes[i].xIndex == (numCols - 1) )
				{
					//check is diagonally opposite corner is full
					if (foxBoard[numRows - 1][0] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else	//move to diagonally opposite corner
					{
						foxBoard[numRows - 1][0]++;										//diagonally opposite square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = numRows - 1;							//Update fox location
						listOfFoxes[i].xIndex = 0;
					}
				}

				//if fox is at right hand edge of square
				else if (listOfFoxes[i].xIndex == (numCols - 1) )
				{
					//check if left hand edge square one row above is full
					if (foxBoard[listOfFoxes[i].yIndex-1][0] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to left hand edge square one row above
					{
						foxBoard[listOfFoxes[i].yIndex-1][0]++;							//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = 0;										//Update fox location
						listOfFoxes[i].yIndex--;	
					}
				}

				//if fox is at top row

				else if (listOfFoxes[i].yIndex == 0 )
				{
					//If bottom square one column to the right is full
					if (foxBoard[numRows - 1][listOfFoxes[i].xIndex + 1] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else //move to bottom square one column to the right
					{
						foxBoard[numRows - 1][listOfFoxes[i].xIndex + 1]++;			//bottom square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;	//original square has 1 less fox
						listOfFoxes[i].yIndex = (numRows - 1);									//update fox location
						listOfFoxes[i].xIndex++;	
					}
				}

				//if fox is not at corners, move diagonally up right 

				else
				{
					//check new square is full
					if (foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex + 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex + 1]++;	//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;			//original square loses 1 fox
						listOfFoxes[i].xIndex += 1;											//Update fox location
						listOfFoxes[i].yIndex -= 1;	
					}
				}


			}
			else if (direction==4)
			{
				
				//If at top square

				if (listOfFoxes[i].yIndex == 0 )
				{
					//If bottom square is full
					if (foxBoard[numRows - 1][listOfFoxes[i].xIndex] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else //move to bottom square
					{
						foxBoard[numRows - 1][listOfFoxes[i].xIndex]++;				//bottom square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;	//original square has 1 less fox
						listOfFoxes[i].yIndex = (numRows - 1);									//update fox location
					}

				}
				//Otherwise move to square above
				//If square above is full
				else if (foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex] == MAX_FOXES_PER_GRID)
				{
					continue;	//does not move
				}
				else //move to square above
				{
					foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex]++;		//square above gets 1 more fox
					foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
					listOfFoxes[i].yIndex--;										//Update fox location
				}


			}
			else if (direction == 5)
			{
				//if fox is at top left corner square
				if (listOfFoxes[i].yIndex == 0  && listOfFoxes[i].xIndex == 0 )
				{
					//check i diagonally opposite corner is full
					if (foxBoard[numRows - 1][numCols - 1] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else	//move to diagonally opposite corner
					{
						foxBoard[numRows - 1][numCols - 1]++;							//diagonally opposite square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = numRows - 1;							//Update fox location
						listOfFoxes[i].xIndex = numCols - 1;
					}
				}

				//if fox is at left hand edge of square
				else if (listOfFoxes[i].xIndex == 0 )
				{
					//check if right hand edge square one row up is full
					if (foxBoard[listOfFoxes[i].yIndex - 1][numCols - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex - 1][numCols - 1]++;				//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = numCols - 1;							//Update fox location
						listOfFoxes[i].yIndex--;	
					}
				}

				//if fox is at top row
				else if (listOfFoxes[i].yIndex == 0 )
				{
					//check if bottom square one col to the left is full
					if (foxBoard[numRows - 1][listOfFoxes[i].xIndex - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to bottom square one column to the left
					{
						foxBoard[numRows - 1][listOfFoxes[i].xIndex - 1]++;					//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = numRows - 1;							//Update fox location
						listOfFoxes[i].xIndex--;	
					}
				}

				//if fox is not at corners, move diagonally up left  

				else
				{
					//check if new square is full
					if (foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex - 1][listOfFoxes[i].xIndex - 1]++;	//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;			//original square loses 1 fox
						listOfFoxes[i].xIndex -= 1;											//Update fox location
						listOfFoxes[i].yIndex -= 1;	
					}
				}



			}
			else if (direction==6)
			{
				
				//if fox is at left hand edge of square
				if (listOfFoxes[i].xIndex == 0 )
				{
					//check if right hand edge square is full
					if (foxBoard[listOfFoxes[i].yIndex][numCols - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square
					{
						foxBoard[listOfFoxes[i].yIndex][numCols - 1]++;					//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = numCols - 1;							//Update fox location
						
					}
				}

				//if not at corner square

				else
				{
					//check if square to the left is full
					if (foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to square on the left
					{
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex - 1]++;	//new square on the right gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex -= 1;										//Update fox location
						
					}
				}

				
			}
			else if (direction == 7)
			{
				
				//if fox is at bottom left corner square
				if (listOfFoxes[i].yIndex == (numRows - 1)  && listOfFoxes[i].xIndex == 0 )
				{
					//check i diagonally opposite corner is full
					if (foxBoard[0][numCols - 1] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else	//move to diagonally opposite corner
					{
						foxBoard[0][numCols - 1]++;										//diagonally opposite square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].yIndex = 0;										//Update fox location
						listOfFoxes[i].xIndex = numCols - 1;
					}
				}

				//if fox is at left hand edge of square
				else if (listOfFoxes[i].xIndex == 0 )
				{
					//check if right hand edge square one row below is full
					if (foxBoard[listOfFoxes[i].yIndex + 1][numCols - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to right hand edge square one row below
					{
						foxBoard[listOfFoxes[i].yIndex + 1][numCols - 1]++;					//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;		//original square loses 1 fox
						listOfFoxes[i].xIndex = numCols - 1;							//Update fox location
						listOfFoxes[i].yIndex++;	
					}
				}

				//if fox is at bottom row
				if (listOfFoxes[i].yIndex == (numRows - 1) )
				{
					//If top square one col to the left is full
					if (foxBoard[0][listOfFoxes[i].xIndex - 1] == MAX_FOXES_PER_GRID)
					{
						continue;	//does not move
					}
					else //move to top square one col to the left
					{
						foxBoard[0][listOfFoxes[i].xIndex - 1]++;						//top square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;	//original square has 1 less fox
						listOfFoxes[i].yIndex = 0;									//update fox location
						listOfFoxes[i].xIndex--;	
					}
				}


				//if fox is not at corners, move diagonally down left  

				else
				{
					//check if new square is full
					if (foxBoard[listOfFoxes[i].yIndex + 1][listOfFoxes[i].xIndex - 1] == MAX_FOXES_PER_GRID)
					{
						continue;		//does not move
					}
					else		//move to new square
					{
						foxBoard[listOfFoxes[i].yIndex + 1][listOfFoxes[i].xIndex - 1]++;	//new square gets 1 more fox
						foxBoard[listOfFoxes[i].yIndex][listOfFoxes[i].xIndex]--;			//original square loses 1 fox
						listOfFoxes[i].xIndex -= 1;											//Update fox location
						listOfFoxes[i].yIndex += 1;	
					}
				}




			}
			else
			{
				cout << "Invalid direction generated";
			}
		}
	}

	cout << "after foxes move" << endl;
	PrintGrids(foxBoard, rabbitBoard, cout, numRows, numCols);
			
}


//Everytime we go to a new grid squarewe start searching from element 0 of FoxList. 
//In 1 grid square, if we find the first fox belonging to this grid square in element 10 of foxList, we store 10 in lastFoxIndex so that we start searching from element 10 for the next fox belonging to this grid  

int findNextLiveFox( Fox *foxList, int Row, int Col, int numberFoxesTotal,int& lastFoxIndex )
{
	for (int i = lastFoxIndex; i < numberFoxesTotal; i++)
	{
		if (foxList[i].xIndex == Col && foxList[i].yIndex == Row && foxList[i].alive == true)
		{
			return i;
		}
	}
	return -1;
}

int findNextLiveRabbit( Rabbit *rabbitList, int Row, int Col, int numberRabbitsTotal, int& nextRabbitIndex )
{
	for (int i = nextRabbitIndex; i < numberRabbitsTotal; i++)
	{
		if (rabbitList[i].xIndex == Col && rabbitList[i].yIndex == Row && rabbitList[i].alive == true)
		{
			return i;
		}
	}
	return -1;

}