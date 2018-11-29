#include "StudentWorld.h"
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>

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
	setDisplayText();
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
void StudentWorld::setDisplayText() 
{
	stringstream gameText;
	gameText << "Lvl: " << setw(2) << getLevel() << " "
		<< "Lives: " << getLives() << " "
		<< "Hlth: " << setw(2) << player->getHealth() << "% "
		<< "Wtr: " << setw(2) << player->getNumSquirts() << " "
		<< "Gld: " << setw(2) << player->getGoldNugs() << " "
		<< "Oil Left: " << setw(2) << this->barrelCount << " "
		<< "Sonar: " << setw(2) << player->getSonarCharges() << " "
		<< "Scr: " << setw(6) << setfill('0') << getScore();
	setGameStatText(gameText.str());
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
    double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		//Checking if the area within 6 Euclidian units is clear of other objects
        distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
		if(distance < MIN_EUCLIDIAN_DIST)
			return false;	//Object to be built is too close to another object and cannot be built there
	}
	return true;	//No objects within 6 units were found and object can be built
}
//---------------------------------------------------------------
void StudentWorld::distributeBarrelsAndGold()	//FIXME - needs to be more general and include goldnuggets
{
	//barrelCount = std::min(static_cast<int>(2 + getLevel()), 21);  //FIXME - change to lambda function?
    //int goldCount = std::max(5 - getLevel() / 2, 2); FIXME - implement this
	barrelCount = 10;
    int goldCount = 10; //FIXME - for testing only
    
    int xCoord;     //Will store a randomly generated x-coordinate
    int yCoord;     //Will store a randomly generated y-coordinate
    for (size_t i = 0; i < barrelCount; ++i)
	{
        
		xCoord = rand() % (MAX_COORDINATE + 1);	//Random x coordinate from 0-60 inclusive.
		yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET); //y coordinate 0-56 non-inclusive
            
		while (!areaIsClear(xCoord, yCoord) || (xCoord > 26 && xCoord < 34 && yCoord > 0))
		{
			//Generate new coordinates because area was not clear or area overlapped vertical shaft
			xCoord = rand() % (MAX_COORDINATE + 1);
			yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
		}

		gameObjects.push_back(std::unique_ptr<Actor>(new BarrelOfOil(xCoord, yCoord, this)));
	}
    
    for(int i = 0; i < goldCount; ++i)
    {
        xCoord = rand() % (MAX_COORDINATE + 1);
        yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
        
        while (!areaIsClear(xCoord, yCoord) || (xCoord > 26 && xCoord < 34 && yCoord > 0))
        {
            xCoord = rand() % (MAX_COORDINATE + 1);
            yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
        }
        //Creating gold that is invisible but can be picked up by the player
        gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(xCoord, yCoord, this, false, true)));
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
//---------------------------------------------------------------
void StudentWorld::addToTunnelManInventory(int change)
{
	switch (change) 
	{
		case ADD_GOLD_NUGGET:
			player->incGoldNugs();
			break;
		case ADD_SONAR:
			player->incSonarCharges();
			break;
		case ADD_SQUIRTS:
			player->increaseNumSquirts();
			break;
	}
}
//---------------------------------------------------------------
void StudentWorld::useSonar() 
{
	//iterating through the gameObjects list and making the ones close to tunnelman visible
	int x = player->getX();
	int y = player->getY();
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		double distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

		if (distance < SONAR_RANGE)
			(*it)->setVisible(true);
	}
}
