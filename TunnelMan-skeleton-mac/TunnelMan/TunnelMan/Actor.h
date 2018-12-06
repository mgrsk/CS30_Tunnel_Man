#ifndef ACTOR_H_
#define ACTOR_H_

#define MAX_COORDINATE				(VIEW_HEIGHT - SPRITE_WIDTH) //Largest X or Y coordinate an object can have
#define MIN_COORDINATE              (0) //Minimum coordinate. ALL objects are within 0 <=x <= 60 and 0 <= y <=60

//Starting positions for various classes

#define START_X_TUNNELMAN           (30)
#define START_Y_TUNNELMAN           (MAX_COORDINATE)
#define START_X_PROTESTOR           (MAX_COORDINATE)
#define START_Y_PROTESTOR           (MAX_COORDINATE)

//Various distances for objects to interact with each other
#define MAX_DISTANCE_INVISIBLE      (4.0)   //The max distance from TunnelMan that a goldnugget/oil barrel can stay invisible
#define PICKUP_DISTANCE				(3.0)   //The distance tunnelman has to be from an object to pick it up
#define SONAR_RANGE					(12.0)  //The range which the sonar can make objects visible

//Standard image size is the size of all classes, except for earth
#define STANDARD_IMAGE_SIZE         (1.0)
#define EARTH_SIZE                  (0.25)

//Various numbers of ticks for objects to perform actions
#define GOLD_LIFETIME				(100)   //How many ticks a gold nugget can survive after being dropped
#define MAX_TICKS_BOULDER_WAITING	(30)    //How long a boulder waits after the earth below it is cleared
#define SQUIRT_LIFETIME             (4)     //How many ticks a squirt can exist
#define TICKS_BETWEEN_SHOUTS        (15)    //How long a protester has to wait after shouting to be able to shout again
#define TICKS_BETWEEN_TURNS         (20)    //How long a protester has to wait before making a turn

//Different damage values
#define DAMAGE_BOULDER              (20)    //How much damage a boulder does. It is high enough to kill anything instantly
#define DAMAGE_SQUIRT_GUN           (2)     //How much damage a squirt does to a protester
#define DAMAGE_PROTESTER_SHOUT      (2)     //How much damage a protester's shout does to TunnelMan

//Values for TunnelMan's starting inventory
#define DEFAULT_GOLD_NUGGETS        (0)     //Number of gold nuggets TunnelMan starts with
#define DEFAULT_SONAR_CHARGES       (1)     //Number of sonar charges TunnelMan starts with
#define DEFAULT_WATER_SQUIRTS       (5)     //Number of squirts TunnelMan starts with

//Max health values for TunnelMan and both types of protesters
#define DEFAULT_HEALTH_PROTESTER    (5)
#define DEFAULT_HEALTH_TUNNELMAN    (10)
#define DEFAULT_HEALTH_HARDCORE     (20)

//Image depth values for various classes
#define DEPTH_EARTH                 (3)
#define DEPTH_GOODIE				(2)
#define DEPTH_BOULDER               (1)
#define DEPTH_SQUIRT                (1)
#define DEPTH_FOREGROUND            (0)

//Score values for different events/actions
#define SCORE_OIL			        (1000)  //Pick up oil
#define SCORE_PICKUP_GOLD           (10)    //TunnelMan picks up gold
#define SCORE_BRIBE_GOLD            (25)    //Protester picks up gold (is bribed)
#define SCORE_SONAR					(75)    //Sonar item is picked up
#define SCORE_WATER_POOL			(100)   //Water pool is picked up (gives player more squirts)
#define SCORE_PROTESTER_BONKED      (500)   //A protester is bonked with a boulder
#define SCORE_PROTESTER_SQUIRTED    (100)   //A protester is annoyed by being squirted too many times

//Values for the StudentWorld's addToTunnelManInventory method
#define ADD_GOLD_NUGGET				(1)
#define ADD_SONAR					(2)
#define ADD_SQUIRTS					(3)

#define MAX_ITEM_COUNT              (99)

#include "GraphObject.h"
#include "StudentWorld.h"
#include <memory>   //For std::unique_ptr

class StudentWorld; //Forwarding declaration


//------------------------------------------
class Actor : public GraphObject
{
private:
    bool stillAlive;    //Tells us if object is still alive or not
    StudentWorld * world;    //Allows classes to see the gameworld and use it's public functions
    
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld * ptr, double size = STANDARD_IMAGE_SIZE, int depth = DEPTH_FOREGROUND, bool shouldDisplay = true);
    ~Actor();

    //ACCESSOR FUNCTIONS
    StudentWorld * getWorld() const;    //Returns a pointer to the gameworld
    bool isAlive() const;   //Tells gameworld if the object is still in play
    
    //MUTATOR FUNCTIONS
	void setDead(); //Sets object's stillAlive variable to false
    
    /*
    This function attempts to move the object by 1 coordinate in the specified direction. Ex. if direction
     is up, y is increased by 1. If direction is left, x is decreased by 1. If the movement would cause the
     object to go out of bounds, it returns false. Otherwise it moves and returns true.
    */
    bool moveInDirection(Direction d);

    //FUNCTIONS WHICH WILL BE OVERRIDED BY APPROPRIATE CLASSES
    virtual void doSomething() = 0; //The main control function for all classes. Each one will override it differently
	virtual void annoy(int damage); //Allows TunnelMan/Protesters to be annoyed/take damage. Does nothing for other classes.
	virtual bool bribe();   //Does nothing for all classes except the protester classes
    virtual bool canBeAnnoyed() const;  //Returns false for all classes except for TunnelMan and protester classes
    virtual bool isBoulder() const; //Returns false for all classes except the boulder class
    
};

//------------------------------------------

class Earth : public Actor
{
public:
    Earth(int startX, int startY);
    void doSomething();
};

//------------------------------------------

/*
 *
 * BOULDER CLASS BELOW
 *
 */

class Boulder : public Actor
{
private:
    bool atRest; //Variable which is true until the Earth below the boulder is destroyed
    size_t ticksWaiting; //Keeps track of how many ticks have passed since the boulder stopped being at rest
public:
    Boulder(int x, int y, StudentWorld * world);
    
    void fall();    //Decrements the boulder's y-coordinate until it hits Earth or y < 0, then calls setDead()
    void doSomething() override;
    bool isBoulder() const override;
    
    
};

//------------------------------------------

/*
 *
 * SQUIRT CLASS BELOW
 *
 */

class Squirt : public Actor
{
private:
    int ticksAlive; //Keeps track of how long the squirt object has been alive for.
public:
    Squirt(int x, int y, Direction startDirection, StudentWorld * world);
    void doSomething();
};

/*
 *
 * PEOPLE CLASSES BELOW. REPRESENTS TUNNELMAN AND PROTESTORS
 *
 */

class People : public Actor
{
private:
    int health; //Keeps track of how much health TunnelMan and protester objects have
    
public:
    //Max health is the starting value for the health member variable
    People(int imageID, int x, int y, Direction startDirection, StudentWorld * world, int maxHealth);
    
    bool canBeAnnoyed() const final;    //Overrides canBeAnnoyed() to return true
    int getHealth() const;              //Returns the amount of health the object has
    void takeDamage(int damage);        //Lowers health by specified damage. Calls setDead() if health <= 0
};

//------------------------------------

class TunnelMan: public People
{
private:
    //ITEM COUNT VARIABLES
    size_t gold_nuggets;    //Keeps track of the number of gold nuggets TunnelMan has
    size_t sonar_charges;   //Number of sonar charges
    size_t num_squirts;     //Number of squirts
    
public:
    TunnelMan(StudentWorld * world);
    ~TunnelMan(); //FIXME - is this needed?
    
    void doSomething() override;
	void annoy(int damage) override;
    
    //ACCESSORS
    size_t getGoldNugs() const;     //Returns gold_nuggets
    size_t getSonarCharges() const; //Returns sonar_charges
    size_t getNumSquirts() const;   //Returns num_squirts
    
    //MUTATORS
    void incGoldNugs();             //Increases gold_nuggets by 1
    void incSonarCharges();         //Increases sonar_charges by 1
    void increaseNumSquirts();      //Increases num_squirts by 5
    
    
};



/*
*
* PROTESTER CLASSES BELOW
*
*/

class RegularProtester : public People
{
private:
    bool leavingOilField;  //Keeps track of whether the protester is leaving the oil field
    bool stunned;          //Is true if the protester is hit by a squirt. After a certain number of ticks, will go back to false.
    
    int ticksBetweenMoves;  //The minimum number of ticks a protester must wait between actions
    int currentRestingTicks;   //Number of ticks since last movement
    int nonRestingTickShoutedAt;    //Keeps track of when a protester last shouted
    int totalNonRestingTicks;  //Stores total number of non-resting ticks
    int lastTickTurnMade; //Stores the last tick that a protester turned at
    
    int levelNumber;    //The current level that the gameWorld is at. Used for certain calculations
    int numSquaresToMoveInCurrentDirection; //The number of squares the protester wants to continue moving

    void checkIfProtesterIsAtIntersection(); //If
    void pickRandomDirection(); //Changes protester's direction randomly until one is picked that allows it to move at least one square
    void tryToMove();   //Attempts to move.
    
public:
    RegularProtester(StudentWorld * world, int level, int imageID = TID_PROTESTER, int maxHealth = DEFAULT_HEALTH_PROTESTER);
	virtual void doSomething() override;
	void annoy(int damage) override;
	bool bribe() override;
	void leaveOilField();
    void setNumSquaresToMoveInCurrentDirection();
};

//------------------------------------------

class HardcoreProtestor : public RegularProtester
{
public:
    HardcoreProtestor(StudentWorld * world, int levelNumber);
    void doSomething() override;
};

//------------------------------------------

/*
*
* GOODIE CLASSES BELOW
*
*/

class Goodie : public Actor
{
private:
    int scoreValue;
    int soundToPlay;
	int ticksPassed;
	int maxTickLife;
public:
    Goodie(int imageID, int startX, int startY, StudentWorld * worldPtr, int score, int sound, int maxTicks, bool shouldDisplay);

    //If tunnelMan is close enough, plays soundToPlay, increases the player's score, and returns true. Otherwise returns false
    bool checkIfTunnelManPickedUp(const double & distanceFromTunnelMan);

	int getMaxTickLife() const;
	int getTicksPassed() const;
	void incTicksPassed();
};

//------------------------------------------

class BarrelOfOil : public Goodie
{
public:
	BarrelOfOil(int startX, int startY, StudentWorld * world);
	void doSomething() override;
};

//------------------------------------------

class GoldNugget : public Goodie
{
private:
    bool canBePickedUpByTunnelMan;  //Tells us if this was a gold object spawned by the game or dropped by TunnelMan
    
public:
    //canPickup sets canBePickedUpByTunnelMan
    GoldNugget(int startX, int startY, StudentWorld * world, bool shouldDisplay, bool canPickup);
    
    void doSomething() override;
    void checkIfProtestorPickedUp();    //Checks if a protester picked it up. Only for gold dropped by TunnelMan
};

//------------------------------------------

class WaterPool : public Goodie 
{
public:
	WaterPool(int startX, int startY, StudentWorld * world, int maxTicks);
	void doSomething() override;
};

//------------------------------------------

class Sonar : public Goodie 
{
public:
	Sonar(int startX, int startY, StudentWorld * world, int maxTicks);
	void doSomething() override;
};




#endif // ACTOR_H_
