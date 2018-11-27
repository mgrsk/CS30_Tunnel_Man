#include "StudentWorld.h"
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
//---------------------------------------------------------------
StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir)
{
}
//---------------------------------------------------------------
StudentWorld::~StudentWorld()
{
    destroyIceField();  //De-initializes all ice objects
    player = nullptr;   
}
//---------------------------------------------------------------
void StudentWorld::makeIceField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - IMAGE_OFFSET; ++j)
        {
            if(!(i >= 30 && i <= 33 && j >= 4)) //Making sure to skip the vertical shaft in the middle
            {
                iceField[i][j] = std::unique_ptr<Ice>(new Ice(i,j));
            }
        }
    }
}
//---------------------------------------------------------------
int StudentWorld::init()
{
    tickCount = 0;
    makeIceField();
    player = std::unique_ptr<TunnelMan>(new TunnelMan);
    player->setWorld(this);
	distributeBarrelsAndGold();
    return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
	askPlayerAndObjectsToDoSomething();
    ++tickCount; 

	if (barrelCount == 0)
		return GWSTATUS_FINISHED_LEVEL; //FIXME - does level increase on its own

    if(player->isAlive())
        return GWSTATUS_CONTINUE_GAME;

    decLives();
    return GWSTATUS_PLAYER_DIED;
}
//---------------------------------------------------------------
void StudentWorld::cleanUp()
{
    destroyIceField();  //De-initializes all ice objects
    player = nullptr;
}
//---------------------------------------------------------------
void StudentWorld::decBarrels() 
{
	--barrelCount;
}
//---------------------------------------------------------------
void StudentWorld::deleteIce(unsigned int xCord, unsigned int yCord)
{
    int newX;   //Will store the x-Coordinates in TunnelMan's 4x4 image
    int newY;   //Will store the y-Coordinates in TunnelMan's 4x4 image
    
    for(int i = 0; i < IMAGE_OFFSET; ++i)   //Iterating through x coordinates of image
    {
        for(int j = 0; j < IMAGE_OFFSET; ++j)   //Iterating through y coordinates of image
        {
            newX = xCord + i;
            newY = yCord + j;
            
            //Validating that coordinate is in bounds
            if(newX < VIEW_WIDTH && newY < VIEW_HEIGHT - IMAGE_OFFSET)
                iceField[newX][newY] = nullptr;     //Frees unique pointer and deletes ice at that point
        }
    }
}
//---------------------------------------------------------------
void StudentWorld::destroyIceField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - IMAGE_OFFSET; ++j)
        {
            iceField[i][j] = nullptr;   //Frees unique pointer and deletes ice at that point
        }
    }
}
//---------------------------------------------------------------
double StudentWorld::calculateEuclidianDistance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) 
{
	double xDiffSquared = pow(x2 - x1, 2);
	double yDiffSquared = pow(y2 - y1, 2);
	return sqrt(xDiffSquared + yDiffSquared);
}
//---------------------------------------------------------------
bool StudentWorld::areaIsClear(unsigned int x, unsigned int y) 
{
	for (std::vector<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		//Checking if the area within 6 Euclidian units if clear of other objects
		if (calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY()) < MIN_EUCLIDIAN_DIST)
			return false;	//Object to be built is too close to another object and cannot be built there
	}
	return true;	//No objects within 6 units were found and object can be built
}
//---------------------------------------------------------------
void StudentWorld::distributeBarrelsAndGold()	//FIXME - needs to be more general and include goldnuggets
{
	barrelCount = std::min(static_cast<int>(2 + getLevel()), 21);  //FIXME - change to lambda function?

	int xCoord = rand() % MAX_COORDINATE + 1;	//Random x coordinate from 0-60 inclusive
	int yCoord = rand() % MAX_COORDINATE + 1;
	for (size_t i = 0; i < barrelCount; ++i)
	{
		while (!areaIsClear(xCoord, yCoord))
		{
			//Generate new coordinates because area was not clear
			xCoord = rand() % MAX_COORDINATE + 1;
			yCoord = rand() % MAX_COORDINATE + 1;
		}
		//gameObjects.push_back(std::unique_ptr<Actor>(new BarrelOfOil(xCoord, yCoord))); FIXME - causes exception
	}
}
//---------------------------------------------------------------
double StudentWorld::getTunnelManDistance(unsigned int x, unsigned int y) 
{
	return calculateEuclidianDistance(x, y, player->getX(), player->getY());
}
//---------------------------------------------------------------
void StudentWorld::destroyDeadObjects() 
{
	std::vector<unique_ptr<Actor>>::iterator it = gameObjects.begin();
	while (it != gameObjects.end()) 
	{
		if (!(*it)->isAlive())
			gameObjects.erase(it);
		else
			++it;
	}
}
//---------------------------------------------------------------
void StudentWorld::askPlayerAndObjectsToDoSomething() 
{
	player->doSomething();
	for (std::vector<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		(*it)->doSomething();
}