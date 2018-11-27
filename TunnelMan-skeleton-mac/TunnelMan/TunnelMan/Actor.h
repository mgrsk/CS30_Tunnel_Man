#ifndef ACTOR_H_
#define ACTOR_H_

#define IMAGE_OFFSET                (4)
#define MAX_COORDINATE				(VIEW_HEIGHT - IMAGE_OFFSET)	//Largest X or Y coordinate an object can have	
#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define STANDARD_IMAGE_SIZE         (1) //FIXME- is this necessary?
#define COMPLETELY_IN_BACKGROUND    (10)//FIXME- is this necessary?
#define COMPLETELY_IN_FOREGROUND    (0) //FIXME - is this necessary?
#define ICE_DEPTH                   (3)
#define ICE_SIZE                    (0.25)
#define BARREL_DEPTH				(2)
#include "GraphObject.h"
#include "StudentWorld.h"
#include <memory>

class StudentWorld; //Forwarding declaration


//------------------------------------------
class Actor : public GraphObject
{
private:
    bool stillAlive;
    std::unique_ptr<StudentWorld> world;    //Allows classes to see the gameworld    
    
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, double size = 1.0, int depth = 0, bool shouldDisplay = true);
    ~Actor();
    std::unique_ptr<StudentWorld>& getWorld();
    void setWorld(StudentWorld * worldPtr);
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
    TunnelMan();
    ~TunnelMan();
    void doSomething();
    
    //Functions are defined, but objects have not yet been implemented. To be done in part 2.
    void incGoldNugs();
    void decGoldNugs();
    void incSonarCharges();
    void decSonarCharges();
    void incNumSquirts();
    void decNumSquirts();
};

//------------------------------------------

class BarrelOfOil : public Actor
{
public:
	BarrelOfOil(unsigned int startX, unsigned int startY);

	void doSomething();
};

#endif // ACTOR_H_
