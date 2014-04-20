//A* implementation(demo) in pure C
//Written by me in 9th grade.
#include <stdio.h>
#include <math.h>



typedef struct _COORD
{
	int x, y;
}COORD;

typedef struct _BOX
{
int info, list;
float g, h;
COORD parent;
}BOX;

COORD bestNode();

void CheckNeighbours(COORD cp);
float magnitute(COORD a, COORD b);
//global vars
BOX grid[25][25]; // main maze
COORD ep; // end postion. ( where u must reach )

void printmaze(int clear)
{
	//clear flag -> if 1 then it will clear the grid and set it to 0
	//else -> jsut print its current state
	int i, z;
	for(i = 0; i < 25; i++)
	{
		for(z = 0; z < 25; z++)
		{	
			if(clear == 1) {
			grid[i][z].info = 0; 
			}
			printf("%i", grid[i][z].info);
		}
		printf("\n");
	}
	printf("\n");
}

int main()
{
	COORD cp, sp; int e; COORD ctemp;
	e  = 0;
	printf("Start point. X: / 0-24 / ");
	scanf("%i", &sp.y);
	printf("Start point. Y: / 0-24 / ");
	scanf("%i", &sp.x);
	printf("End point. X: / 0-24 / ");
	scanf("%i", &ep.y);
	printf("End point. Y: / 0-24 / ");
	scanf("%i", &ep.x);
	printmaze(1);
	//add closed
	printf("Add a closed block? Yes = 1, No = everything else: ");
	scanf("%i", &e);
	while(e == 1)
	{
		printf("Closed Block. X: / 0-24 / ");
		scanf("%i", &cp.y);
		printf("Closed Block. Y: / 0-24 / ");
		scanf("%i", &cp.x);
		grid[cp.x][cp.y].list = 2;
		grid[cp.x][cp.y].info = 2; // with 2 I've marked the closed postions where u can't go
		printf("Add a closed block? Yes = 1, No = everything else: ");
		scanf("%i", &e);
	}
	grid[sp.x][sp.y].info = 5; // start point on the grid
	grid[ep.x][ep.y].info = 6; // end point on the grid
	printf("\n");
	// find best way
	cp.x = sp.x;
	cp.y = sp.y;
	//add to open list;
	grid[cp.x][cp.y].list = 1;
	grid[cp.x][cp.y].g = 0;
	grid[cp.x][cp.y].h = magnitute(cp, ep);
	while(!(cp.x == ep.x && cp.y == ep.y)) // while we have not reached the target
	{//  current postion.x == end postion.x ...

		//check best node from open list;
		cp = bestNode();
		printf("checking %i-%i \n", cp.x, cp.y);
		//check out neighbours;
		if(cp.x == -25000 || cp.y == -25000) // my grid is fix sized from 0-24 so I use -25000 as a flag
		{
			printf("There is no way.");
			cp.x = ep.x;
			cp.y = ep.y;
		}else{
		CheckNeighbours(cp);
		grid[cp.x][cp.y].list = 2; // closed list 
		}
		
	}	
	
	while(!(cp.x == sp.x && cp.y == sp.y))
	{
		cp = grid[cp.x][cp.y].parent;
		grid[cp.x][cp.y].info = 1;
		if(cp.x == sp.x && cp.y == sp.y)
		grid[cp.x][cp.y].info = 5;
	}
	
	printf("\n");
	printmaze(2);
	printf("End. \n");
	scanf("%i", &e);
	

	return 0;
}



void CheckNeighbours(COORD cp)
{
	int i, z;
						COORD ctemp;
	for(i = -1; i < 2; i++)
	{

		for(z = -1; z < 2; z++)
		{
			//check if alive;
			if(cp.x + i >= 0 && cp.x + i < 25 && cp.y + z >= 0 && cp.y + z < 25)
			{
				if(!(i == 0 && z == 0))
				{
					if(grid[cp.x + i][cp.y + z].list != 1 && grid[cp.x + i][cp.y + z].list != 2)			
					{ //if open list or closed
						grid[cp.x + i][cp.y + z].list = 1;
						grid[cp.x + i][cp.y + z].parent.x = cp.x;
						grid[cp.x + i][cp.y + z].parent.y = cp.y;


						//record h value
						ctemp.x = cp.x + i;
						ctemp.y = cp.y + z;
						grid[cp.x  + i][cp.y  + z].h  =  magnitute(ctemp, ep);
						grid[cp.x  + i][cp.y  + z].g  =  magnitute(ctemp, cp) + grid[cp.x][cp.y].g;
						printf("new neighbour: %i - %i \n", cp.x  + i, cp.y + z);
					}else if(grid[cp.x + i][cp.y + z].list == 1)
					{
						ctemp.x = cp.x  + i;
						ctemp.y = cp.y  + z;
						if( grid[cp.x  + i][cp.y  + z].g > grid[cp.x][cp.y].g + magnitute(cp, ctemp))
						{
							grid[cp.x  + i][cp.y  + z].parent = cp;
							grid[cp.x  + i][cp.y  + z].g  =  magnitute(ctemp, cp)  + grid[cp.x][cp.y].g;
						}
					}
				} // end if mid
			} // end if alive
		} // end for z
	}
	return;
}

COORD bestNode()
{
	int i, z;
	int bestf = -25000;
	int a = 0;
	COORD bestNode;
	for(i = 0; i < 25; i++)
	{
		for(z = 0; z < 25; z++)
		{	
			if(grid[i][z].list == 1)
			{
				if(a == 0)
				{
					a = 1;
					bestf = grid[i][z].g + grid[i][z].h;
					bestNode.x = i;
					bestNode.y = z;
				}
				else 
				{
					if(bestf > grid[i][z].g + grid[i][z].h)
					{
						bestf = grid[i][z].g + grid[i][z].h;
						bestNode.x = i;
						bestNode.y = z;
					}
				}
			}

		}
	}
	if(a == 0)
	{
		bestNode.x = -25000;
		bestNode.y = -25000;
	}
	return bestNode;
}





float magnitute(COORD a, COORD b)
{
	COORD toret; float ab;
	if(a.x > b.x)
	{
		toret.x = a.x - b.x;
	}else
	{
		toret.x = b.x - a.x;
	}
	if(a.y > b.y)
	{
		toret.y = a.y - b.y;
	}else
	{
		toret.y = b.y - a.y;
	}
	ab = toret.x * toret.x + toret.y * toret.y;
	ab = sqrt(ab);
	return ab;
}










