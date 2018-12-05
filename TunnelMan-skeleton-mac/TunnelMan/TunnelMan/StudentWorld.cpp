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
StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir),
    currentTick(0), numberOfProtesters(0)
{
}
//---------------------------------------------------------------
StudentWorld::~StudentWorld()
{
    destroyEarthField();  //De-initializes all earth objects
	player.reset();
}
//---------------------------------------------------------------
int StudentWorld::init()
{
	makeEarthField();
	player = std::unique_ptr<TunnelMan>(new TunnelMan(this));
	distributeBarrelsGoldAndBoulders();
    ticksBetweenProtesters = MAX(25, 200 - getLevel());
    targetNumberOfProtesters = MIN(15, 2 + getLevel() * 1.5);
    currentTick = 0;
    
	return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
    while(player->isAlive())
    {
        setDisplayText();
        askPlayerAndObjectsToDoSomething();
        destroyDeadObjects();
        generateGoodies();
        generateProtesters();
        ++currentTick;

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
	destroyEarthField();  //De-initializes all earth objects
	player.reset();
    gameObjects.clear();
    numberOfProtesters = 0;
}
//---------------------------------------------------------------
void StudentWorld::setDisplayText() 
{
	stringstream gameText;
	gameText << "Lvl: " << setw(2) << getLevel() << " "
		<< "Lives: " << getLives() << " "
		<< "Hlth: " << setw(3) << player->getHealth() * 10 << "% "
		<< "Wtr: " << setw(2) << player->getNumSquirts() << " "
		<< "Gld: " << setw(2) << player->getGoldNugs() << " "
		<< "Oil Left: " << setw(2) << this->barrelCount << " "
		<< "Sonar: " << setw(2) << player->getSonarCharges() << " "
		<< "Scr: " << setw(6) << setfill('0') << getScore();
	setGameStatText(gameText.str());
}
//---------------------------------------------------------------
void StudentWorld::makeEarthField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - SPRITE_WIDTH; ++j)
        {
            if(!(i >= SHAFT_LEFT_COORD && i <= SHAFT_RIGHT_COORD && j >= SHAFT_BOTTOM_COORD)) //Making sure to skip the vertical shaft in the middle
            {
                earthField[i][j] = std::unique_ptr<Earth>(new Earth(i,j));
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
void StudentWorld::decNumberOfProtesters()
{
    --numberOfProtesters;
}
//---------------------------------------------------------------
void StudentWorld::deleteEarthAroundObject(int xCord, int yCord)
{
    int newX;   //Will store the x-Coordinates in TunnelMan's 4x4 image
    int newY;   //Will store the y-Coordinates in TunnelMan's 4x4 image
    
    for(int i = 0; i < SPRITE_WIDTH; ++i)   //Iterating through x coordinates of image
    {
        for(int j = 0; j < SPRITE_WIDTH; ++j)   //Iterating through y coordinates of image
        {
            newX = xCord + i;
            newY = yCord + j;
            
            //Validating that coordinate is in bounds
            if(newX < VIEW_WIDTH && newY < VIEW_HEIGHT - SPRITE_WIDTH)
                earthField[newX][newY].reset();     //Frees unique pointer and deletes earth at that point
        }
    }
}
//---------------------------------------------------------------
void StudentWorld::destroyEarthField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT; ++j)
        {
            earthField[i][j].reset();   //Frees unique pointer and deletes earth at that point
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
bool StudentWorld::areaIsClearOfObjects(int x, int y)
{
    double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		//Checking if the area within 6 Euclidian units is clear of other objects
        distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
		if(distance < MIN_SPAWN_DIST)
			return false;	//Object to be built is too close to another object and cannot be built there
	}
	return true;	//No objects within 6 units were found and object can be built
}
//---------------------------------------------------------------
bool StudentWorld::areaIsClearOfEarth(int x, int y)
{
	for (int i = x; i < x + SPRITE_WIDTH; ++i)
	{
		for (int j = y; j < y + SPRITE_WIDTH; ++j)
		{
			if (earthField[i][j]) //Checking if there is earth at that point
				return false;   //Earth was found, blocking the object from moving
		}
	}
	return true;    //No earth was found. Object is free to move
}
//---------------------------------------------------------------
bool StudentWorld::noBouldersBlocking(int x, int y, GraphObject::Direction d, Actor *object)
{
    if(!shiftCoordinates(x, y, 1, d))
        return false;   //Coordinate shifting failed because of bounds
    
    double distance;
    for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
    {
        if((*it)->isBoulder() && object != &(*it->get()))
        {
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
            if(distance <= MIN_INTERACT_DIST)
                return false;   //Object would be touching boulder if it moved here
        }
    }
    return true;    //Area is clear of boulders
}
//---------------------------------------------------------------
void StudentWorld::generateObjects(int numObjects, int typeOfObject)
{
    int xCoord;
    int yCoord;
    bool isBoulder;
    
    if(typeOfObject == GENERATE_BOULDER)
        isBoulder = true;
    else
        isBoulder = false;
    
    for(int i = 0; i < numObjects; ++i)
    {
        generateRandomCoordinates(xCoord, yCoord, isBoulder);
        
        while (!areaIsClearOfObjects(xCoord, yCoord) || ((xCoord > SHAFT_LEFT_COORD - SPRITE_WIDTH) &&
                (xCoord <= SHAFT_RIGHT_COORD) && (yCoord > 0)))
        {
            //Generate new coordinates because area was not clear or area overlapped vertical shaft
            generateRandomCoordinates(xCoord, yCoord, isBoulder);
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
                gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(xCoord, yCoord, this, false, true)));
                break;
            }
            case GENERATE_BOULDER:
            {
                gameObjects.push_back(std::unique_ptr<Actor>(new Boulder(xCoord, yCoord, this)));
                deleteEarthAroundObject(xCoord, yCoord);
                break;
            }
        }
    }
}
//---------------------------------------------------------------
void StudentWorld::generateProtesters()
{
    if(numberOfProtesters == targetNumberOfProtesters)
        return; //There are already enough protesters on the field
    
    //Checking if this is the initial tick or if enough ticks have passed for a new protester
	if (currentTick == 0 || currentTick % ticksBetweenProtesters == 0)
	{
		int probabilityOfHardcore = MIN(90, getLevel() * 10 + 30);
		int chanceOfHardcore = rand() % probabilityOfHardcore;

		//1 in probabilityOfHardcore chance that protester will be hardcore
		if (chanceOfHardcore == 0)
			gameObjects.push_back(std::unique_ptr<Actor>(new HardcoreProtestor(this, getLevel())));
		else
			gameObjects.push_back(std::unique_ptr<Actor>(new RegularProtester(this, getLevel())));

		++numberOfProtesters;
	}
}
//---------------------------------------------------------------
void StudentWorld::distributeBarrelsGoldAndBoulders()
{
	barrelCount = MIN((2 + getLevel()), 21); 
    int goldCount = MAX(5 - getLevel() / 2, 2);
	int boulderCount = MIN(getLevel() / 2 + 2, 9);	

    generateObjects(barrelCount, GENERATE_OIL);
    generateObjects(goldCount, GENERATE_GOLD);
    generateObjects(boulderCount, GENERATE_BOULDER);
    
}
//---------------------------------------------------------------
void StudentWorld::generateRandomCoordinates(int & xCoord, int & yCoord, bool isBoulder)
{
    xCoord = rand() % (MAX_COORDINATE + 1);    //Random x coordinate from 0-60 inclusive.
    
    if(isBoulder)
        yCoord = rand() % (37) + MIN_BOULDER_HEIGHT; //y coordinate 20-56 inclusive
    else
        yCoord = rand() % (MAX_COORDINATE - SPRITE_WIDTH + 1); //y coordinate 0-56 inclusive
}
//---------------------------------------------------------------
void StudentWorld::generateGoodies() 
{
    int tickLife = MAX(100, 300 - 10 * getLevel());
	int goodieChance = (getLevel() * 25) + 300;
    
	if (rand() % goodieChance == 0) //1 in G chance new goodie is made
	{
		int sonarChance = rand() % 5; //1 in 5 chance sonar is made
        
		if (sonarChance == 0)	//Generate a sonar
		{
			gameObjects.push_back(std::unique_ptr<Actor>(new Sonar(0, MAX_COORDINATE, this, tickLife)));
		}
		else 
		{
            int xCoord;
            int yCoord;
            generateRandomCoordinates(xCoord, yCoord, false);

			//Generate new random coordinates until the area is clear of earth and objects
			while (!areaIsClearOfEarth(xCoord, yCoord) || !areaIsClearOfObjects(xCoord, yCoord))
			{
                generateRandomCoordinates(xCoord, yCoord, false);
			}
			gameObjects.push_back(std::unique_ptr<Actor>(new WaterPool(xCoord, yCoord, this, tickLife)));
		}
	}
}
//---------------------------------------------------------------
double StudentWorld::getTunnelManDistance(int x, int y)
{
	return calculateEuclidianDistance(x, y, player->getX(), player->getY());
}
//---------------------------------------------------------------
bool StudentWorld::canProteserShoutAtTunnelMan(int x, int y, GraphObject::Direction d)
{
    int newX = x;
    int newY = y;
    
    if(calculateEuclidianDistance(x, y, player->getX(), player->getY()) > 4.0)
        return false;   //Protester is too far
    
    //Shifting x or y coordinate over 1 depending on direction protester is facing
    if(!shiftCoordinates(newX, newY, 1, d))
        return false;   //Protester is facing out of bounds
    
    /*
     Iterating through shifted coordinates. If TunnelMan is within
     the 16x16 grid that an object could occupy with these coordinates,
     then the protester is facing him.
     */
    for(int i = newX; i < newX + SPRITE_WIDTH; ++i)
    {
        for(int j = newY; j < newY + SPRITE_HEIGHT; ++j)
        {
            if(i == player->getX() && j == player->getY())
                return true;
        }
    }
    
    return false;
}
//---------------------------------------------------------------
bool StudentWorld::tunnelManIsInStraightLineOfSight(int x, int y)
{
    return false; //FIXME - implement function
}
//---------------------------------------------------------------
void StudentWorld::shoutAtTunnelMan()
{
    player->annoy(DAMAGE_PROTESTER_SHOUT);
    playSound(SOUND_PROTESTER_YELL);
}
//---------------------------------------------------------------
bool StudentWorld::shiftCoordinates(int &x, int &y, int amountToShift, GraphObject::Direction directionToShift)
{
    switch(directionToShift)
    {
        case GraphObject::down:
        {
            y -= amountToShift;
            
            if(y < 0)
                return false;
            
            break;
        }
        case GraphObject::up:
        {
            y += amountToShift;
            
            if(y > MAX_COORDINATE)
                return false;
            
            break;
        }
        case GraphObject::left:
        {
            x -= amountToShift;
            
            if(x < 0)
                return false;
            
            break;
        }
        case GraphObject::right:
        {
            x += amountToShift;
            
            if(x > MAX_COORDINATE)
                return false;
            
            break;
        }
        case GraphObject::none:
            return false;
    }
    return true;
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
bool StudentWorld::checkForBribes(int x, int y)
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
        
        //Only protestors and tunnelman can be annoyed, but since tunnelman is not in
        //the gameobjects list, only protestors can pass this if statement and be bribed
		if (distance <= PICKUP_DISTANCE && (*it)->canBeAnnoyed())
		{
			if((*it)->bribe())
                return true;
		}
	}
	return false;
}
//---------------------------------------------------------------
bool StudentWorld::noEarthBlocking(int x, int y, GraphObject::Direction directionToCheck)
{
	switch (directionToCheck) 
	{
		case GraphObject::right:
		case GraphObject::up:
		{
			if (!shiftCoordinates(x, y, 4, directionToCheck))
				return false;
			break;
		}
		case GraphObject::left: 
		case GraphObject::down:
		{
			if (!shiftCoordinates(x, y, 1, directionToCheck))
				return false;
			break;
		}
	}

	if (directionToCheck == GraphObject::right || directionToCheck == GraphObject::left) 
	{
		for (int i = y; i < y + 4; ++i) 
		{
			if (earthField[x][i])
				return false;
		}
	}
	else 
	{
		for (int i = x; i < x + 4; ++i) 
		{
			if (earthField[i][y])
				return false;
		}
	}
    
	return true;
}
//---------------------------------------------------------------
void StudentWorld::checkForBoulderHits(int x, int y)
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

		if (distance <= MIN_INTERACT_DIST)
			(*it)->annoy(DAMAGE_BOULDER);
	}
    if(getTunnelManDistance(x, y) <= 3.0)
        player->annoy(DAMAGE_BOULDER);
}
//---------------------------------------------------------------
bool StudentWorld::checkForSquirtGunHits(int x, int y)
{
    bool annoyedProtester = false;
    double distance;
    
    for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
    {
        if((*it)->canBeAnnoyed())
        {
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
            if(distance <= MIN_INTERACT_DIST)
            {
                (*it)->annoy(DAMAGE_SQUIRT_GUN);
                annoyedProtester = true;
            }
        }
    }
    
    return annoyedProtester;
}
//---------------------------------------------------------------
void StudentWorld::useSonar() 
{
	int x = player->getX();
	int y = player->getY();
	double distance;
    
    //iterating through the gameObjects list and making the ones close enough to tunnelman visible
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
		distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

		if (distance < SONAR_RANGE)
			(*it)->setVisible(true);
	}
    
    playSound(SOUND_SONAR);
}
//---------------------------------------------------------------
void StudentWorld::useSquirtGun(GraphObject::Direction d)
{
    int x = player->getX();
    int y = player->getY();
    gameObjects.push_back(std::unique_ptr<Actor>(new Squirt(x, y, d, this)));
    playSound(SOUND_PLAYER_SQUIRT);
}
//---------------------------------------------------------------
void StudentWorld::dropGoldNug()
{
	gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(player->getX(), player->getY(), this, true, false)));
}
