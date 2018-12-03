#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

//Coordinates for the shaft in the middle
#define SHAFT_LEFT_COORD    (30)
#define SHAFT_RIGHT_COORD    (33)
#define SHAFT_BOTTOM_COORD    (4)

#define MIN_SPAWN_DIST	    (6)	//Minimum distance objects must be spawned from each other
#define MIN_INTERACT_DIST   (3.0) //Minimum distance for objects to interact with each other
#define MIN_BOULDER_HEIGHT  (20) //Minimum y coordinate boulders can spawn at

#define GENERATE_OIL        (1)
#define GENERATE_GOLD       (2)
#define GENERATE_BOULDER    (3)

//Forwarding declarations
class Actor;
class TunnelMan;
class Earth;
class Squirt;
class BarrelOfOil;
class Sonar;
class WaterPool;
class Boulder;
class RegularProtester;
class HardcoreProtestor;



class StudentWorld : public GameWorld
{
private:
    int currentTick;
	int barrelCount;		//Keeps track of number of active barrels
    int numberOfProtesters;
    int targetNumberOfProtesters;
    int ticksBetweenProtesters;
    
    
    std::unique_ptr<Earth> earthField[VIEW_WIDTH][VIEW_HEIGHT];	//Holds the earth field
    std::unique_ptr<TunnelMan> player;	//The player object
    std::list<std::unique_ptr<Actor>> gameObjects;	//All other game objects, eg. sonar, enemies, etc
    
    void makeEarthField();    //Generates the earth field in the game
    void destroyEarthField(); //Destroys the earth field during cleanup
    
	void askPlayerAndObjectsToDoSomething();	//Asks player and all objects in gameObjects list to do something
	void destroyDeadObjects();	//Removes dead objects from the gameObjects list
    
    
    void distributeBarrelsGoldAndBoulders();	//Randomly distributes these barrels/gold in the field during initialization
	void generateObjects(int numObjects, int typeOfObject);
    void generateGoodies();
    void generateRandomCoordinates(int & xCoord, int & yCoord, bool isBoulder);
    void generateProtesters();
	double calculateEuclidianDistance(double x1, double y1, double x2, double y2);
    
    
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	void setDisplayText();
    
	void decBarrels();
    void decNumberOfProtesters();
    void deleteEarthAroundObject(unsigned int xCord, unsigned int yCord);	//Deletes earth in area overlapping an object's image
	bool areaIsClearOfObjects(unsigned int x, unsigned int y);	//Determines if an object can be spawned by checking area surrounding it for objects in the gameObjects list
	bool areaIsClearOfEarth(unsigned int x, unsigned int y);
    bool noBouldersBlocking(unsigned int x, unsigned int y, GraphObject::Direction d);
	
    
	double getTunnelManDistance(unsigned int x, unsigned int y);	//Tells an object if TunnelMan is close enough to interact with it
    bool canProteserShoutAtTunnelMan(unsigned int x, unsigned int y, GraphObject::Direction d);
    bool tunnelManIsInStraightLineOfSight(unsigned int x, unsigned int y);
    void shoutAtTunnelMan();
    bool shiftCoordinates(int &x, int &y, GraphObject::Direction d);
	bool checkForBribes(unsigned int x, unsigned int y);
    bool noEarthBlocking(unsigned int x, unsigned int y, GraphObject::Direction d);
    bool checkForSquirtGunHits(unsigned int x, unsigned int y);
	void checkForBoulderHits(unsigned int x, unsigned int y);
    

	void addToTunnelManInventory(int change); //Adds items to TunnelMan's Inventory (sonar, water, gold, etc)
	void useSonar();
    void useSquirtGun(GraphObject::Direction d);
	void dropGoldNug(unsigned int x, unsigned int y);
    



};

#endif // STUDENTWORLD_H_
