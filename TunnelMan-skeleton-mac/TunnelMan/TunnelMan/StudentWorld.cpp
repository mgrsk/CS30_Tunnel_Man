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
	makeIceField();
	player = std::unique_ptr<TunnelMan>(new TunnelMan(this));
	distributeBarrelsGoldAndBoulders();
	//distributeBoulders();
	return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
    while(player->isAlive())
    {
        setDisplayText();
        generateGoodies();
        askPlayerAndObjectsToDoSomething();
        destroyDeadObjects();

        if (barrelCount == 0)
            return GWSTATUS_FINISHED_LEVEL;
        
        return GWSTATUS_CONTINUE_GAME;
    }
    
	decLives();
    playSound(SOUND_PLAYER_GIVE_UP);
	return GWSTATUS_PLAYER_DIED;
}
//---------------------------------------------------------------
void StudentWorld::cleanUp()
{
	destroyIceField();  //De-initializes all ice objects
	player.reset();
    gameObjects.clear();
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
            if(!(i >= SHAFT_LEFT_COORD && i <= SHAFT_RIGHT_COORD && j >= SHAFT_BOTTOM_COORD)) //Making sure to skip the vertical shaft in the middle
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
void StudentWorld::deleteIceAroundObject(unsigned int xCord, unsigned int yCord)
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
bool StudentWorld::areaIsClearOfObjects(unsigned int x, unsigned int y) 
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
bool StudentWorld::areaIsClearOfIce(unsigned int x, unsigned int y) 
{
	for (size_t i = x; i < x + 4; ++i)
	{
		for (size_t j = y; j < y + 4; ++j)
		{
			if (iceField[i][j] != nullptr) //Checking if there is ice at that point
				return false;
		}
	}
	return true;
}
//---------------------------------------------------------------
bool StudentWorld::noBouldersBlocking(unsigned int x, unsigned int y)
{
    double distance;
    for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
    {
        if((*it)->isBoulder())
        {
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
            if(distance <= 3.0)
                return false;   //Player would be touching boulder if it moved here
        }
    }
    return true;    //Area is clear of boulders
}
//---------------------------------------------------------------
void StudentWorld::generateObjects(int numObjects, int typeOfObject)
{
    int xCoord;
    int yCoord;
    
    for(size_t i = 0; i < numObjects; ++i)
    {
        xCoord = rand() % (MAX_COORDINATE + 1);    //Random x coordinate from 0-60 inclusive.
        
        if(typeOfObject == GENERATE_BOULDER)
            yCoord = rand() % (37) + MIN_BOULDER_HEIGHT; //y coordinate 20-56 inclusive
        else
            yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
        
        
        while (!areaIsClearOfObjects(xCoord, yCoord) || ((xCoord > SHAFT_LEFT_COORD - IMAGE_OFFSET) &&
                (xCoord <= SHAFT_RIGHT_COORD) && (yCoord > 0)))
        {
            //Generate new coordinates because area was not clear or area overlapped vertical shaft
            xCoord = rand() % (MAX_COORDINATE + 1);
            
            if(typeOfObject == GENERATE_BOULDER)
                yCoord = rand() % (37) + MIN_BOULDER_HEIGHT;
            else
                yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
        }
        
        switch(typeOfObject)
        {
            case GENERATE_OIL:
            {
                gameObjects.push_back(std::unique_ptr<Actor>(new BarrelOfOil(xCoord, yCoord, this)));
                break;
            }
            case GENERATE_GOLD:
            {
                gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(xCoord, yCoord, this, false,true)));
                break;
            }
            case GENERATE_BOULDER:
            {
                gameObjects.push_back(std::unique_ptr<Actor>(new Boulder(xCoord, yCoord, this)));
                deleteIceAroundObject(xCoord, yCoord);
                break;
            }
        }
    }
}

void StudentWorld::distributeBarrelsGoldAndBoulders()
{
	//barrelCount = std::min(static_cast<int>(2 + getLevel()), 21);  //FIXME - change to lambda function?
    //int goldCount = std::max(5 - getLevel() / 2, 2); FIXME - implement this
    //int Boulders = min(current_level_number / 2 + 2, 9)
	this->barrelCount = 35;
    int goldCount = 3; //FIXME - for testing only
    int boulderCount = 9;
    
    generateObjects(barrelCount, GENERATE_OIL);
    generateObjects(goldCount, GENERATE_GOLD);
    generateObjects(boulderCount, GENERATE_BOULDER);
    
}
//---------------------------------------------------------------
void StudentWorld::generateGoodies() 
{
	int tickLife = 100; //For testing - FIXME later
	//int tickLife = max(100, 300 – 10 * getLevel()) 
	int G = (getLevel() * 25) + 300;
	if (rand() % G == 0) //1 in G chance new goodie is made
	{
		int P = rand() % 5; //1 in 5 chance sonar is made
		if (P == 0)	//Generate a sonar
		{
			gameObjects.push_back(std::unique_ptr<Actor>(new Sonar(0, MAX_COORDINATE, this, tickLife)));
		}
		else 
		{
			int xCoord = rand() % (MAX_COORDINATE + 1);	//Random x coordinate from 0-60 inclusive.
			int yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET); //y coordinate 0-56 non-inclusive

			//Generate new random coordinates until the area is clear of ice and objects
			while (!areaIsClearOfIce(xCoord, yCoord) || !areaIsClearOfObjects(xCoord, yCoord)) 
			{
				xCoord = rand() % (MAX_COORDINATE + 1);
				yCoord = rand() % (MAX_COORDINATE - IMAGE_OFFSET);
			}
			gameObjects.push_back(std::unique_ptr<Actor>(new WaterPool(xCoord, yCoord, this, tickLife)));
		}
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
bool StudentWorld::checkForBribes(unsigned int x, unsigned int y) 
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
		if (distance <= PICKUP_DISTANCE && (*it)->getCanBeAnnoyed()) 
		{
			(*it)->bribe();
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------
bool StudentWorld::checkForIceBelowBoulder(unsigned int x, unsigned int y) 
{
	for (int i = x; i < x + IMAGE_OFFSET; ++i) 
	{
		//Checking if there is ice immediately below the boulder
		if (iceField[i][y - 1] != nullptr)
			return true;
	}
	return false;
}
//---------------------------------------------------------------
void StudentWorld::checkForBoulderHits(unsigned int x, unsigned int y) 
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

		if (distance <= 3.0)
			(*it)->annoy(BOULDER_DAMAGE);
	}
    if(getTunnelManDistance(x, y) <= 3.0)
        player->annoy(BOULDER_DAMAGE);
}
//---------------------------------------------------------------
void StudentWorld::useSonar() 
{
	//iterating through the gameObjects list and making the ones close to tunnelman visible
	int x = player->getX();
	int y = player->getY();
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

		if (distance < SONAR_RANGE)
			(*it)->setVisible(true);
	}
}
//---------------------------------------------------------------
void StudentWorld::dropGoldNug(unsigned int x, unsigned int y) 
{
	gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(x, y, this, true, false)));
}
