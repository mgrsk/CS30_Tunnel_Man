#ifndef ACTOR_H_
#define ACTOR_H_

#define MAX_COORDINATE				(VIEW_HEIGHT - SPRITE_WIDTH)//Largest X or Y coordinate an object can have

#define START_X_TUNNELMAN           (30)
#define START_Y_TUNNELMAN           (MAX_COORDINATE)
#define START_X_PROTESTOR           (MAX_COORDINATE)
#define START_Y_PROTESTOR           (MAX_COORDINATE)

#define MAX_DISTANCE_INVISIBLE      (4.0)
#define PICKUP_DISTANCE				(3.0)
#define SONAR_RANGE					(12)

#define STANDARD_IMAGE_SIZE         (1.0)
#define EARTH_SIZE                  (0.25)

#define GOLD_LIFETIME				(100)
#define MAX_TICKS_BOULDER_WAITING	(30)
#define SQUIRT_LIFETIME             (4)
#define TICKS_BETWEEN_SHOUTS        (15)

#define DAMAGE_BOULDER              (10)
#define DAMAGE_SQUIRT_GUN           (2)
#define DAMAGE_PROTESTER_SHOUT      (2)

#define DEFAULT_GOLD_NUGGETS        (0)
#define DEFAULT_SONAR_CHARGES       (1)
#define DEFAULT_WATER_SQUIRTS       (25)

#define DEFAULT_HEALTH_PROTESTER    (5)
#define DEFAULT_HEALTH_TUNNELMAN    (10)
#define DEFAULT_HEALTH_HARDCORE     (20)


#define DEPTH_EARTH                 (3)
#define DEPTH_GOODIE				(2)
#define DEPTH_BOULDER               (1)
#define DEPTH_SQUIRT                (1)
#define DEPTH_FOREGROUND            (0)

#define SCORE_OIL			        (1000)
#define SCORE_PICKUP_GOLD           (10)
#define SCORE_BRIBE_GOLD            (25)
#define SCORE_SONAR					(75)
#define SCORE_WATER_POOL			(100)
#define SCORE_PROTESTER_BONKED      (500)
#define SCORE_PROTESTER_SQUIRTED    (100)


#define ADD_GOLD_NUGGET				(1)
#define ADD_SONAR					(2)
#define ADD_SQUIRTS					(3)

#include "GraphObject.h"
#include "StudentWorld.h"
#include <memory>

class StudentWorld; //Forwarding declaration


//------------------------------------------
class Actor : public GraphObject
{
private:
    bool stillAlive;	//Tells gameworld if the object is still in play
    StudentWorld * world;    //Allows classes to see the gameworld
    
public:
    Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection, StudentWorld * ptr, double size = STANDARD_IMAGE_SIZE, int depth = DEPTH_FOREGROUND, bool shouldDisplay = true);
    ~Actor();

    StudentWorld * getWorld() const;
    bool isAlive() const;
	
    void setDead();
    bool moveInDirection(Direction d);

    virtual void doSomething() = 0;
	virtual void annoy(int damage);
	virtual bool bribe();
    
    virtual bool canBeAnnoyed() const;
    virtual bool isBoulder() const;
    
};

//------------------------------------------

class Earth : public Actor
{
public:
    Earth(unsigned int startX, unsigned int startY);
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
    bool atRest;
    size_t ticksWaiting;
public:
    Boulder(unsigned int x, unsigned int y, StudentWorld * world);
    void doSomething() override;
    void fall();
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
    int ticksAlive;
public:
    Squirt(unsigned int x, unsigned int y, Direction startDirection, StudentWorld * world);
    void doSomething();
};

/*
 *
 * PEOPLE CLASSES BELOW. REPRESENT TUNNELMAN AND PROTESTORS
 *
 */

class People : public Actor
{
private:
    int health;
    
public:
    People(int imageID, unsigned int x, unsigned int y, Direction startDirection, StudentWorld * world, int maxHealth);
    
    bool canBeAnnoyed() const final;
    
    int getHealth() const;
    void takeDamage(int damage);
};

//------------------------------------

class TunnelMan: public People
{
private:
    size_t gold_nuggets;
    size_t sonar_charges;
    size_t num_squirts;
    
public:
    TunnelMan(StudentWorld * world);
    ~TunnelMan(); //FIXME - is this needed?
    
    void doSomething() override;
	void annoy(int damage) override;
    
    void incGoldNugs();
	size_t getGoldNugs() const;

    void incSonarCharges();
	size_t getSonarCharges() const;

    void increaseNumSquirts();
	size_t getNumSquirts() const;
};



/*
*
* PROTESTER CLASSES BELOW
*
*/

class RegularProtester : public People
{
private:
    bool leavingOilField;
    bool atRest;
    int ticksBetweenMoves;
    int currentRestingTicks;   //Number of ticks since last movement
    int nonRestingTickShoutedAt;
    int totalNonRestingTicks;  //Stores total number of non-resting ticks
    int levelNumber;
    int numSquaresToMoveInCurrentDirection;
    bool stunned;
    
    void pickRandomDirection();
    void tryToMove();
    
    
    
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
    Goodie(int imageID, unsigned int startX, unsigned int startY, StudentWorld * worldPtr, int score, int sound, int maxTicks, bool shouldDisplay);

    bool checkIfTunnelManPickedUp(const double & distanceFromTunnelMan);

	int getMaxTickLife() const;
	int getTicksPassed() const;
	void incTicksPassed();
};

//------------------------------------------

class BarrelOfOil : public Goodie
{
public:
	BarrelOfOil(unsigned int startX, unsigned int startY, StudentWorld * world);
	void doSomething() override;
};

//------------------------------------------

class GoldNugget : public Goodie
{
private:
    bool canBePickedUpByTunnelMan;
public:
    GoldNugget(unsigned int startX, unsigned int startY, StudentWorld * world, bool shouldDisplay, bool canPickup);
    void doSomething() override;
    void checkIfProtestorPickedUp();
};

//------------------------------------------

class WaterPool : public Goodie 
{
public:
	WaterPool(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks);
	void doSomething() override;
};

//------------------------------------------

class Sonar : public Goodie 
{
public:
	Sonar(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks);
	void doSomething() override;
};




#endif // ACTOR_H_
