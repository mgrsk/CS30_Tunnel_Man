#ifndef ACTOR_H_
#define ACTOR_H_

#define TUNNEL_MAN_START_X          (30)
#define TUNNEL_MAN_START_Y          (60)
#define STANDARD_IMAGE_SIZE         (1) //FIXME- is this necessary?
#define COMPLETELY_IN_BACKGROUND    (10)
#define COMPLETELY_IN_FOREGROUND    (0) //FIXME - is this necessary?
#define IMAGE_OFFSET                (4)
#define ICE_DEPTH                   (3)
#define ICE_SIZE                    (0.25)
#include "GraphObject.h"
#include "StudentWorld.h"
#include <memory>
class StudentWorld; //Forwarding declaration


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
    void moveOneSquareinDirection(Direction d){} //FIXME - do i need this?
    
};

//------------------------------------------

class Ice : public Actor
{
public:
    Ice(int startX = 0, int startY = 0);
    void doSomething(); //Method is only implemented because it inherets from pure virtual function
};

//------------------------------------------

class TunnelMan: public Actor
{
public:
    TunnelMan();
    void doSomething();
    void incGoldNugs();
    void decGoldNugs();
    void incSonarCharges();
    void decSonarCharges();
    void incNumSquirts();
    void decNumSquirts();
    
private:
    unsigned int health; //FIXME -should I make a base class for protestors/tunnelman w/ health/annoy member?
    unsigned int gold_nuggets;
    unsigned int sonar_charges;
    unsigned int num_squirts;
};




#endif // ACTOR_H_
