#ifndef ACTOR_H_
#define ACTOR_H_

#define IMAGE_OFFSET                (4)
#define MAX_COORDINATE				(VIEW_HEIGHT - IMAGE_OFFSET)	//Largest X or Y coordinate an object can have
#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define MAX_DISTANCE_INVISIBLE      (4)
#define SONAR_RANGE					(12)

#define STANDARD_IMAGE_SIZE         (1)
#define ICE_SIZE                    (0.25)

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
    bool stillAlive;
    StudentWorld * world;    //Allows classes to see the gameworld
    
public:
    Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection = right, StudentWorld * ptr = nullptr, double size = 1.0, int depth = 0, bool shouldDisplay = true);
    ~Actor();
    StudentWorld * getWorld();
    bool isAlive();
    void setDead();
    virtual void doSomething() = 0;
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
    size_t health = 10; //FIXME -should I make a base class for protestors/tunnelman w/ health/annoy member?
    size_t gold_nuggets;
    size_t sonar_charges;
    size_t num_squirts;
    
public:
    TunnelMan(StudentWorld * world);
    ~TunnelMan();
    void doSomething();
    
    void incGoldNugs();
    void decGoldNugs();
	int getGoldNugs();

    void incSonarCharges();
	int getSonarCharges();

    void increaseNumSquirts(); 
    void decNumSquirts();
	int getNumSquirts();

	int getHealth() { return health; } //FIXME - separate into cpp. determine need for base class

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
    bool atRest;    //FIXME - implement this
    int ticksSinceLastAction; //FIXME - implement this
public:
    Goodie(int imageID, unsigned int startX, unsigned int startY, StudentWorld * worldPtr, int score, int sound, bool shouldDisplay);
    virtual void doSomething() = 0;
    bool checkIfTunnelManPickedUp(const double & distanceFromTunnelMan);
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
	WaterPool(unsigned int startX, unsigned int startY, StudentWorld * world);
	void doSomething();
};

//------------------------------------------

class Sonar : public Goodie 
{
public:
	Sonar(unsigned int startX, unsigned int startY, StudentWorld * world);
	void doSomething();
};




#endif // ACTOR_H_
