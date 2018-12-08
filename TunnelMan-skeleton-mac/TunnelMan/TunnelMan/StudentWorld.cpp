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
	--this->barrelCount;
}
//---------------------------------------------------------------
void StudentWorld::decNumberOfProtesters()
{
    --this->numberOfProtesters;
}
//---------------------------------------------------------------
void StudentWorld::deleteEarthAroundObject(const int xCord, const int yCord)
{
    for(int i = xCord; i < xCord + SPRITE_WIDTH; ++i)   //Iterating through x coordinates of image
    {
        for(int j = yCord; j < yCord + SPRITE_HEIGHT; ++j)   //Iterating through y coordinates of image
        {
                earthField[i][j].reset();     //Frees unique pointer and deletes earth at that point
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
double StudentWorld::calculateEuclidianDistance(double x1, double y1, double x2, double y2) const
{
	double xDiffSquared = pow(x2 - x1, 2);
	double yDiffSquared = pow(y2 - y1, 2);
	return sqrt(xDiffSquared + yDiffSquared);
}
//---------------------------------------------------------------
bool StudentWorld::areaIsClearOfObjects(const int x, const int y) const
{
    double distance;
	for (std::list<unique_ptr<Actor>>::const_iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		//Checking if the area within 6 Euclidian units is clear of other objects
        distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
        
		if(distance < MIN_SPAWN_DIST)
			return false;	//Object to be built is too close to another object and cannot be built there
	}
	return true;	//No objects within MIN_SPAWN_DIST units were found and object can be built
}
//---------------------------------------------------------------
bool StudentWorld::areaIsClearOfEarth(const int x, const int y) const
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
bool StudentWorld::noBouldersBlocking(int x, int y, GraphObject::Direction d, Actor *object) const
{
    if(!shiftCoordinates(x, y, 1, d))
        return false;   //Coordinate shifting failed because of bounds
    
    double distance;
    for (std::list<unique_ptr<Actor>>::const_iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
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
void StudentWorld::generateObjects(const int numObjects, const int typeOfObject)
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
    if(this->numberOfProtesters >= this->targetNumberOfProtesters)
        return; //There are already enough protesters on the field
    
    //Checking if enough ticks have passed for a new protester or if this is the initial tick
	if (this->currentTick % this->ticksBetweenProtesters == 0 || this->currentTick == 0)
	{
		int probabilityOfHardcore = MIN(90, getLevel() * 10 + 30);  //Determines %chance of a hardcore protester
		int chanceOfHardcore = (rand() % 100) + 1;  //Generating random number 1-100 inclusive

		//If chanceOfHardcore is in range 1-probabilityOfHardcore, then make a hardcore protester.
		if (chanceOfHardcore <= probabilityOfHardcore)
			gameObjects.push_back(std::unique_ptr<Actor>(new HardcoreProtestor(this, getLevel())));
		else
			gameObjects.push_back(std::unique_ptr<Actor>(new RegularProtester(this, getLevel())));

		++this->numberOfProtesters;
	}
}
//---------------------------------------------------------------
void StudentWorld::distributeBarrelsGoldAndBoulders()
{
	this->barrelCount = MIN((2 + getLevel()), 21);
    int goldCount = MAX(5 - getLevel() / 2, 2);
    int boulderCount = MIN(getLevel() / 2 + 2, 9);

    generateObjects(barrelCount, GENERATE_OIL);
    generateObjects(goldCount, GENERATE_GOLD);
    generateObjects(boulderCount, GENERATE_BOULDER);
    
}
//---------------------------------------------------------------
void StudentWorld::generateRandomCoordinates(int & xCoord, int & yCoord, bool isBoulder) const
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
	int goodieChance = (getLevel() * 25) + 300;
    
	if (rand() % goodieChance == 0) //1 in G chance new goodie is made
	{
        int tickLife = MAX(100, 300 - 10 * getLevel()); //The maximum tick life of the goodie to be generated
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
double StudentWorld::getTunnelManDistance(const int x, const int y) const
{
	return calculateEuclidianDistance(x, y, player->getX(), player->getY());
}
//---------------------------------------------------------------
bool StudentWorld::canProteserShoutAtTunnelMan(int x, int y, GraphObject::Direction d)
{
    if(calculateEuclidianDistance(x, y, player->getX(), player->getY()) > 4.0)
        return false;   //Protester is too far
    
    //Shifting x or y coordinate over 1 depending on direction protester is facing
    if(!shiftCoordinates(x, y, 1, d))
        return false;   //Protester is facing out of bounds
    
    /*
     Iterating through shifted coordinates. If TunnelMan is within
     the 16x16 grid that an object could occupy with these coordinates,
     then the protester is facing him.
     */
    for(int i = x; i < x + SPRITE_WIDTH; ++i)
    {
        for(int j = y; j < y + SPRITE_HEIGHT; ++j)
        {
            if(i == player->getX() && j == player->getY())
                return true;
        }
    }
    
    return false;
}
//---------------------------------------------------------------
//FIXME - protesters are able to see through walls
bool StudentWorld::tunnelManIsInStraightLineOfSight(int x, int y, GraphObject::Direction &directionToTunnelMan)
{
    int tunnelManX = player->getX();    //Stores tunnelMan's x-coordinate. Avoids calling getX() too many times
    int tunnelManY = player->getY();    //Stores tunnelMan's y-coordinate. Avoids calling getY() too many times
    
    if(x == tunnelManX)
    {
        if(y > tunnelManY)
            directionToTunnelMan = GraphObject::down;
        else
            directionToTunnelMan = GraphObject::up;
    }
    else if(y == tunnelManY)
    {
        if(x > tunnelManX)
            directionToTunnelMan = GraphObject::left;
        else
            directionToTunnelMan = GraphObject::right;
    }
    else
        return false; //TunnelMan must have one x or y coordinate in common with protester to be straight line away
    
    //Continues to shift coordinates in TunnelMan's direction, to see if there is no Earth/Boulder's blocking the path to him
    while(noEarthBlocking(x, y, directionToTunnelMan) && noBouldersBlocking(x, y, directionToTunnelMan, nullptr))
    {
        shiftCoordinates(x, y, 1, directionToTunnelMan);
        if(x == tunnelManX || y == tunnelManY)
            return true;    //The protester is able to reach TunnelMan without any earth/boulders blocking
    }
    
    return false;   //There was either Earth or a boulder blocking the path to TunnelMan
}
//---------------------------------------------------------------
void StudentWorld::shoutAtTunnelMan()
{
    player->annoy(DAMAGE_PROTESTER_SHOUT);
    playSound(SOUND_PROTESTER_YELL);
}
//---------------------------------------------------------------
bool StudentWorld::shiftCoordinates(int &x, int &y, const int &amountToShift, GraphObject::Direction directionToShift) const
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
void StudentWorld::addToTunnelManInventory(const int change)
{
	switch (change) 
	{
		case ADD_GOLD_NUGGET:
        {
			player->incGoldNugs();
			break;
        }
		case ADD_SONAR:
        {
			player->incSonarCharges();
			break;
        }
		case ADD_SQUIRTS:
        {
			player->increaseNumSquirts();
			break;
        }
	}
}
//---------------------------------------------------------------
bool StudentWorld::checkForBribes(const int x, const int y)
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
        if((*it)->canBeAnnoyed())   //For efficiency. Skips all objects except protesters.
        {
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());
        
            if (distance <= PICKUP_DISTANCE)
            {
                if((*it)->bribe())  //Checking if the protester is in a state where he can be bribed, and bribing him if so
                    return true;    //Protester was able to be bribed. Only one can be bribed by each gold, so we must exit the function.
            }
        }
	}
	return false;
}
//---------------------------------------------------------------
/*
 If we need to check the right side of an object at (x, y) for Earth, then we must check
 (x+4, y) to (x+4, y+3) for Earth, because its image occupies x+1, x+2, x+3, and the first bit
 of Earth to its right could only exist at x+4, and we need to check the y coordinates its image
 would occupy if it were moved to the right. The same principle applies for checking up,
 but its (x, y+4) to (x+3, y+4). For checking left and down, since the objects point is at the bottom
 right, we only need to shift the constant coordinate by 1. So for left, it would be (x-1, y) to
 (x-1, y+3), and for down it would be (x, y-1) to (x+3, y-1).
 */
bool StudentWorld::noEarthBlocking(int x, int y, GraphObject::Direction directionToCheck) const
{
	switch (directionToCheck) //Determines whether to shift the x or y coordinate, and by how much
	{
		case GraphObject::right:
		case GraphObject::up:
		{
            shiftCoordinates(x, y, 4, directionToCheck);
			break;
		}
		case GraphObject::left: 
		case GraphObject::down:
		{
            shiftCoordinates(x, y, 1, directionToCheck);
			break;
		}
        case GraphObject::none:
            return false;
	}
    
    //Validating that shifted coordinates are in bounds
    if(x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)
        return false;

    //Determines which coordinate to shift, and which one to iterate through in the loop
	if (directionToCheck == GraphObject::right || directionToCheck == GraphObject::left) 
	{
		for (int i = y; i < y + 4; ++i) 
		{
			if (earthField[x][i])
				return false;   //Earth was found to the object's left or right
		}
	}
	else 
	{
		for (int i = x; i < x + 4; ++i) 
		{
			if (earthField[i][y])
				return false;   //Earth was found above or below the object
		}
	}
    
	return true;
}
//---------------------------------------------------------------
void StudentWorld::checkForBoulderHits(const int x, const int y)
{
	double distance;
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
        if((*it)->canBeAnnoyed())   //For efficiency. Skips all objects in list except regular and hardcore protesters
        {
            //Calculating distance from boulder to protester
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

            if (distance <= MIN_INTERACT_DIST)  //Checking if the boulder is close enough to bonk the object
                (*it)->annoy(DAMAGE_BOULDER);
        }
	}
    
    if(getTunnelManDistance(x, y) <= MIN_INTERACT_DIST)   //Checking player separately because he is not in gameobjects list
        player->annoy(DAMAGE_BOULDER);
}
//---------------------------------------------------------------
bool StudentWorld::checkForSquirtGunHits(const int x, const int y)
{
    bool annoyedProtester = false;  //Tells us whether at least one or more protesters were annoyed
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
    //Following two variables are simply to prevent calling getX() and getY() too many times in loop
	int x = player->getX();
	int y = player->getY();
	double distance;
    
    //iterating through the gameObjects list and making the ones close enough to tunnelman visible
	for (std::list<unique_ptr<Actor>>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) 
	{
        if(!(*it)->isVisible()) //For efficiency. Skips objects that are already visible
        {
            distance = calculateEuclidianDistance(x, y, (*it)->getX(), (*it)->getY());

            if (distance < SONAR_RANGE)
                (*it)->setVisible(true);
        }
	}
    
    playSound(SOUND_SONAR);
}
//---------------------------------------------------------------
void StudentWorld::useSquirtGun()
{
    //Creating a new squirt object a TunnelMan's position, facing his direction
    gameObjects.push_back(std::unique_ptr<Actor>(new Squirt(player->getX(), player->getY(), player->getDirection(), this)));
    playSound(SOUND_PLAYER_SQUIRT);
}
//---------------------------------------------------------------
void StudentWorld::dropGoldNug()
{
    //Creating a new GoldNugget object at TunnelMan's position that is visible and cannot be picked up by him
	gameObjects.push_back(std::unique_ptr<Actor>(new GoldNugget(player->getX(), player->getY(), this, true, false)));
}
