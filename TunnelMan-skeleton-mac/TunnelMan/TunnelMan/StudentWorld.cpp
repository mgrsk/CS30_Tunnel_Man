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
	player.reset();
}
//---------------------------------------------------------------
int StudentWorld::init()
{
	tickCount = 0;
	makeIceField();
	player = std::unique_ptr<TunnelMan>(new TunnelMan(this));
	distributeBarrelsAndGold();
	return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
	askPlayerAndObjectsToDoSomething();
	destroyDeadObjects();
	++tickCount;

	if (barrelCount == 0)
		return GWSTATUS_FINISHED_LEVEL; //FIXME - does level increase on its own

	if (player->isAlive())
		return GWSTATUS_CONTINUE_GAME;

	decLives();
	return GWSTATUS_PLAYER_DIED;
}
//---------------------------------------------------------------
void StudentWorld::cleanUp()
{
	destroyIceField();  //De-initializes all ice objects
	player.reset();
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
                iceField[newX][newY].reset();     //Frees unique pointer and deletes ice at that point
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
            iceField[i][j].reset();   //Frees unique pointer and deletes ice at that point
        }
    }
}
//---------------------------------------------------------------
double StudentWorld::calculateEuclidianDistance(double x1, double y1, double x2, double y2) 
{
	double xDiffSquared = pow(x2 - x1, 2);
	double yDiffSquared = pow(y2 - y1, 2);
	return sqrt(xDiffSquared + yDiffSquared);
}
//---------------------------------------------------------------
bool StudentWorld::areaIsClear(unsigned int x, unsigned int y) 
{
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		//Checking if the area within 6 Euclidian units if clear of other objects
		if(calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY()) < MIN_EUCLIDIAN_DIST)
			return false;	//Object to be built is too close to another object and cannot be built there
	}
	return true;	//No objects within 6 units were found and object can be built
}
//---------------------------------------------------------------
void StudentWorld::distributeBarrelsAndGold()	//FIXME - needs to be more general and include goldnuggets
{
	//barrelCount = 45; //Use this to test barrel distribution
	this->barrelCount = std::min(static_cast<int>(2 + getLevel()), 21);  //FIXME - change to lambda function?
	for (size_t i = 0; i < barrelCount; ++i)
	{
		int xCoord = rand() % MAX_COORDINATE;	//Random x coordinate from 0-60 non inclusive. FIXME - check this bound
		int yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET); //0-56 non-inclusive
		while (!areaIsClear(xCoord, yCoord) || (xCoord > 26 && xCoord < 30 && yCoord > 0)) //FIXME - check these bounds
		{
			//Generate new coordinates because area was not clear or area overlapped vertical shaft
			xCoord = rand() % MAX_COORDINATE;
			yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
		}

		gameObjects.push_back(std::unique_ptr<Actor>(new BarrelOfOil(xCoord, yCoord, this)));
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
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end();)
	{
		if (!((*it)->isAlive()))	//Checking if it's dead, so we know if we need to delete it
		{
			(*it).reset();
			gameObjects.erase(it);
			it = gameObjects.begin();
		}
		else
			++it;
	}
}
//---------------------------------------------------------------
void StudentWorld::askPlayerAndObjectsToDoSomething() 
{
	player->doSomething();
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		(*it)->doSomething();
}