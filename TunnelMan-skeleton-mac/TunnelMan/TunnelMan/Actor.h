#ifndef ACTOR_H_
#define ACTOR_H_

#define IMAGE_OFFSET                (4)
#define MAX_COORDINATE				(VIEW_HEIGHT - IMAGE_OFFSET)//Largest X or Y coordinate an object can have
#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define MAX_DISTANCE_INVISIBLE      (4.0)
#define PICKUP_DISTANCE				(3.0)
#define SONAR_RANGE					(12)

#define STANDARD_IMAGE_SIZE         (1)
#define ICE_SIZE                    (0.25)

#define GOLD_LIFETIME				(100)
#define MAX_TICKS_BOULDER_WAITING	(30)
#define SQUIRT_LIFETIME             (4)

#define DAMAGE_BOULDER              (10)
#define DAMAGE_SQUIRT_GUN           (2)

#define DEFAULT_GOLD_NUGGETS        (0)
#define DEFAULT_SONAR_CHARGES       (1)
#define DEFAULT_WATER_SQUIRTS       (25)
#define DEFAULT_HEALTH_TUNNELMAN    (10)

#define ICE_DEPTH                   (3)
#define GOODIE_DEPTH				(2)

#define SCORE_OIL			        (1000)
#define SCORE_PICKUP_GOLD           (10)
#define SCORE_BRIBE_GOLD            (25)
#define SCORE_SONAR					(75)
#define SCORE_WATER_POOL			(100)


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
    Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection, StudentWorld * ptr, double size = 1.0, int depth = 0, bool shouldDisplay = true);
    ~Actor();

    StudentWorld * getWorld();
    bool isAlive();
	
    void setDead();
    bool moveInDirection(Direction d);

    virtual void doSomething() = 0;
    virtual bool canBeAnnoyed();
    virtual bool isBoulder();
	virtual void annoy(size_t damage);
	virtual void bribe();
    
};

//------------------------------------------

class Ice : public Actor
{
public:
    Ice(unsigned int startX, unsigned int startY);
    void doSomething();
};

//------------------------------------------

class TunnelMan: public Actor
{
private:
	size_t health;
    size_t gold_nuggets;
    size_t sonar_charges;
    size_t num_squirts;
    
public:
    TunnelMan(StudentWorld * world);
    ~TunnelMan();
    bool canBeAnnoyed() override;
    void doSomething();
	void annoy(size_t damage) override;
    
    void incGoldNugs();
    void decGoldNugs();
	size_t getGoldNugs();

    void incSonarCharges();
	size_t getSonarCharges();

    void increaseNumSquirts(); 
    void decNumSquirts();
	size_t getNumSquirts();

	size_t getHealth();


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
	void doSomething();
	void fall();
    bool isBoulder();
    
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
* PROTESTER CLASSES BELOW
*
*/

class Protester : public Actor 
{
private:

public:
	void doSomething();
	void annoy(size_t damage) override;
    bool canBeAnnoyed() override;
	void bribe() override;
	void leaveOilField();
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
    virtual void doSomething() = 0;
	void annoy(size_t damage);
	void bribe();

    bool checkIfTunnelManPickedUp(const double & distanceFromTunnelMan);

	int getMaxTickLife();
	int getTicksPassed();
	void incTicksPassed();
};


//------------------------------------------


class BarrelOfOil : public Goodie
{
public:
	BarrelOfOil(unsigned int startX, unsigned int startY, StudentWorld * world);
	void doSomething();
};

//------------------------------------------

class GoldNugget : public Goodie
{
private:
    bool canBePickedUpByTunnelMan;
public:
    GoldNugget(unsigned int startX, unsigned int startY, StudentWorld * world, bool shouldDisplay, bool canPickup);
    void doSomething();
    void checkIfProtestorPickedUp();
};

//------------------------------------------

class WaterPool : public Goodie 
{
public:
	WaterPool(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks);
	void doSomething();
};

//------------------------------------------

class Sonar : public Goodie 
{
public:
	Sonar(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks);
	void doSomething();
};




#endif // ACTOR_H_
