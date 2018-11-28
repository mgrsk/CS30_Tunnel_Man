#ifndef ACTOR_H_
#define ACTOR_H_

#define IMAGE_OFFSET                (4)
#define MAX_COORDINATE				(VIEW_HEIGHT - IMAGE_OFFSET)//Largest X or Y coordinate an object can have
#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define STANDARD_IMAGE_SIZE         (1)
#define ICE_DEPTH                   (3)
#define ICE_SIZE                    (0.25)
#define GOODIE_DEPTH				(2)
#define OIL_SCORE                   (1000)
#define GOLD_SCORE_PICKUP           (10)
#define GOLD_SCORE_BRIBE            (25)
#define MAX_DISTANCE_INVISIBLE      (4)
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
	void setWorld(StudentWorld * worldPtr);
    
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
    
    //Variables are defined, but objects have not yet been implemented. To be done in part 2.
    size_t health; //FIXME -should I make a base class for protestors/tunnelman w/ health/annoy member?
    size_t gold_nuggets;
    size_t sonar_charges;
    size_t num_squirts;
    
public:
    TunnelMan(StudentWorld * world);
    ~TunnelMan();
    void doSomething();
    
    //Functions are defined, but objects have not yet been implemented. To be done in part 2.
    void incGoldNugs();
    void decGoldNugs();
    void incSonarCharges();
    void decSonarCharges();
    void incNumSquirts(); //FIXME - may not need these
    void decNumSquirts();
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
    bool checkIfTunnelManPickedUp();
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




#endif // ACTOR_H_
