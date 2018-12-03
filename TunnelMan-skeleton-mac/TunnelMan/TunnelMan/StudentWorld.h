#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>

//Min and max functions, which take two numbers throughout different functions
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

//Coordinates for the shaft in the middle
#define SHAFT_LEFT_COORD    (30)
#define SHAFT_RIGHT_COORD    (33)
#define SHAFT_BOTTOM_COORD    (4)

#define MIN_SPAWN_DIST	    (6)	    //Minimum distance objects must be spawned from each other
#define MIN_INTERACT_DIST   (3.0)   //Minimum distance for objects to interact with each other
#define MIN_BOULDER_HEIGHT  (20)    //Minimum y coordinate boulders can spawn at

//Different values used for the generateObjects function. Determines which object to spawn
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
    
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    
    //GAME CONTROL FUNCTIONS
    virtual int init();         //Creates a new level
    virtual int move();         //Runs the game
    virtual void cleanUp();     //Destroys all objects after the level is finished
    
    void setDisplayText();      //Sets the stats at the top of the screen
    
    void decBarrels();          //Reduces barrelCount by 1. Is called by the BarrelOfOil class
    void decNumberOfProtesters();   //Reduces numberOfProtesters by 1. Called by the protester class
    
    //FIXME - make more efficient
    void deleteEarthAroundObject(int xCord, int yCord);    //Deletes earth in the 4x4 area overlapping an object's image.
    
    /*
     Determines if an object at x, y can be spawned by checking area surrounding it for objects in the gameObjects list.
     If there are NO objects within MIN_SPAWN_DIST, returns true. Otherwise returns false.
    */
    bool areaIsClearOfObjects(int x, int y);
    
    /*
     Determines if the 4x4 area of an object at x, y would be clear of any Earth. Returns
     true if there is NO Earth, otherwise returns false.
    */
    bool areaIsClearOfEarth(int x, int y);
    
    /*
     Determines if moving an object at x, y in the Direction specified by d would cause it
     to hit a boulder. Returns true if NO boulders would hit it, otherwise returns false.
    */
    bool noBouldersBlocking(int x, int y, GraphObject::Direction d);
    
    /*
     Determines if moving an object at x, y in the Direction specified by d would cause it
     to hit Earth. Returns true if NO Earth would hit it, otherwise returns false.
     */
    bool noEarthBlocking(int x, int y, GraphObject::Direction d); //FIXME - make more efficient
    
    double getTunnelManDistance(int x, int y);    //Returns the distance between TunnelMan and an object at x, y
    
    /*
     Determines if a protester at x, y and facing Direction d can shout at the TunnelMan.
     Checks if he is closing enough and if the protester is facing him. Returns true
     if so, otherwise returns false.
    */
    bool canProteserShoutAtTunnelMan(int x, int y, GraphObject::Direction d);
    void shoutAtTunnelMan();    //Called by the protester class, causes TunnelMan to be annoyed.
    
    bool tunnelManIsInStraightLineOfSight(int x, int y);    //Checks if TunnelMan is within line of sight of a protester at x, y
    
    /*
     Checks if there are any protesters within range of a gold nugget at x, y
     that can be bribed. If yes, it bribes ONLY the first protester it finds and
     the function returns true. Otherwise, returns false.
    */
    bool checkForBribes(int x, int y);
    
    /*
     Checks if there are any protesters within range of a squirt at x, y that can be
     annoyed. If yes, ALL protesters in range will be squirted and the function returns true.
     Otherwise, it returns false.
    */
    bool checkForSquirtGunHits(int x, int y);
    
    void checkForBoulderHits(int x, int y); //Checks if a boulder at x, y will annoy the player or any protesters.
    
    void addToTunnelManInventory(int change); //Adds sonar, gold, or squirts to TunnelMan's Inventory based on the int change 
    
    /*
     Attempts to shift the x or y coordinate in the specified Direction d. If d == up, shifts y to y+1, if
     d == left, shifts x to x-1, etc. Returns true if move was successful and updates x or y
     by reference. Returns false and makes no changes if x or y would go out of bounds.
    */
    bool shiftCoordinates(int &x, int &y, GraphObject::Direction d);
    
    //FUNCTIONS CALLED BY TUNNELMAN TO USE HIS ITEMS
    void useSonar();    //Activates the sonar object and makes gold/oil nearby visible
    void useSquirtGun(GraphObject::Direction d); //Creates a squirt object at TunnelMan's x,y position that moves in the Direction d.
    void dropGoldNug(); //Creates a gold object at TunnelMan's x,y position. Gives it temporary lifespan
    
    
    
private:
    int currentTick;                //Keeps track of which tick the game is on
	int barrelCount;		        //Keeps track of number of active barrels
    int numberOfProtesters;         //Keeps track of number of active protesters
    int targetNumberOfProtesters;   //Maximum number of protesters that can exist at once
    int ticksBetweenProtesters;     //How many ticks the game must wait before adding a new protester
    
    
    std::unique_ptr<Earth> earthField[VIEW_WIDTH][VIEW_HEIGHT];	//Holds the earth field
    std::unique_ptr<TunnelMan> player;	                        //The player object
    std::list<std::unique_ptr<Actor>> gameObjects;	            //All other game objects, eg. sonar, enemies, etc
    
    void makeEarthField();    //Generates the earth field during init
    void destroyEarthField(); //Destroys the earth field during cleanup
    
	void askPlayerAndObjectsToDoSomething();	//Asks player and all objects in gameObjects list to do something
	void destroyDeadObjects();	                //Removes dead objects from the gameObjects list
    
    void distributeBarrelsGoldAndBoulders();	//Determines number of barrels/gold/boulders to creat during init. Calls generateObjects
    
    /*
    generateObjects is called from distributeBarrelsGoldAndBoulders. Int numObjects tells it how many objects to
    insert into the gameObjects vector, and typeOfObject tells generateObjects which type of object it is inserting.
    */
	void generateObjects(int numObjects, int typeOfObject);
    
    void generateGoodies(); //Tries to generate sonar/water pools depending on random values.
    
    /*
     Generates random x, y coordinates and assigns xCoord and yCoord to those coordinates. The random coordinates will be
     within the range of MIN_COORDINATE to MAX_COORDINATE for both x and y, UNLESS isBoulder is true. In that case,
     the coordinates will be from MIN_BOULDER_HEIGHT to MAX_COORDINATE for the yCoord value.
     */
    void generateRandomCoordinates(int & xCoord, int & yCoord, bool isBoulder);
    
    void generateProtesters();  //Attempts to generate a new protester
	double calculateEuclidianDistance(double x1, double y1, double x2, double y2); //Calculates the distance between x1,y1 and x2,y2
    

    



};

#endif // STUDENTWORLD_H_
