#ifndef ACTOR_H_
#define ACTOR_H_

#define IMAGE_OFFSET                (4)
#define MAX_COORDINATE				(VIEW_HEIGHT - IMAGE_OFFSET)	//Largest X or Y coordinate an object can have
#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define MAX_DISTANCE_INVISIBLE      (4.0)
#define PICKUP_DISTANCE				(3.0)
#define SONAR_RANGE					(12)

#define STANDARD_IMAGE_SIZE         (1)
#define ICE_SIZE                    (0.25)

#define GOLD_LIFETIME				(100)
#define MAX_TICKS_BOULDER_WAITING	(30)

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
	bool canBeAnnoyed;	//Allows gameworld to determine if it is a protestor/TunnelMan, or another type of object
    StudentWorld * world;    //Allows classes to see the gameworld
    
public:
    Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection, StudentWorld * ptr, 
		bool annoyable = false, double size = 1.0, int depth = 0, bool shouldDisplay = true);
    ~Actor();

    StudentWorld * getWorld();
    bool isAlive();
	bool getCanBeAnnoyed();
    void setDead();

    virtual void doSomething() = 0;
	virtual void annoy(size_t damage) = 0;
	virtual void bribe() = 0;
};

//------------------------------------------

class Ice : public Actor
{
public:
    Ice(unsigned int startX, unsigned int startY);
    void doSomething(); 
	void annoy(size_t damage);
	void bribe();
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
    void doSomething();
	void annoy(size_t damage);
	void bribe();
    
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
	void annoy(size_t damage);
	void bribe();
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
	void annoy(size_t damage);
	void bribe();
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
