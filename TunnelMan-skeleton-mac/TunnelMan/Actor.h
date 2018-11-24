#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#define NUM_ICE_COLUMNS (64)
#define NUM_ICE_ROWS    (60)

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class Actor : public GraphObject
{
private:
    bool isAlive;
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, double size = 1.0, int depth = 0, bool shouldDisplay = true, bool alive = true): GraphObject(imageID, startX, startY, startDirection, size, depth) //FIXME move to cpp
    {
        setVisible(shouldDisplay);
        isAlive = alive;
    }
    ~Actor(){}
    virtual void doSomething() = 0;
    void moveOneSquareinDirection(Direction d){} //FIXME - do i need this?
    
};

class Ice : public Actor
{
public:
    Ice(int startX = 0, int startY = 0): Actor(TID_EARTH, startX, startY, right, 0.25, 3){}
    //FIXME - move to cpp
    void doSomething(){ return; } //Method is only implemented because it inherets from pure virtual function
};


#endif // ACTOR_H_
