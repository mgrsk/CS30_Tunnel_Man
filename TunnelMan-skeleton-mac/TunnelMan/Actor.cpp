#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/*
 *
 ACTOR CLASS IMPLEMENTATION BELOW
 *
 */

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, double size, int depth, bool shouldDisplay): GraphObject(imageID, startX, startY, startDirection, size, depth)
{
    setVisible(shouldDisplay);
    stillAlive = true;
}
//------------------------------------------
Actor::~Actor()
{
    world.release();    //FIXME - is this necessary?
}
//------------------------------------------
bool Actor::isAlive()
{
    return isAlive();
}
//------------------------------------------
void Actor::setDead()
{
    stillAlive = false;
}
//------------------------------------------
std::unique_ptr<StudentWorld>& Actor::getWorld()
{
    return world;
}
//------------------------------------------
void Actor::setWorld(const StudentWorld & world)
{
    //this->world = std::move(std::unique_ptr<StudentWorld>(&world));
}



//------------------------------------------
/*
 *
 ICE CLASS IMPLEMENTATION BELOW
 *
 */

Ice::Ice(int startX, int startY): Actor(TID_EARTH, startX, startY, right, ICE_SIZE, ICE_DEPTH){}
//------------------------------------------------------------------------------------
void Ice::doSomething(){ return; } //implemented only because it ineherents from pure virtual function
//------------------------------------------------------------------------------------

/*
 *
 TUNNELMAN CLASS IMPLEMENTATION BELOW
 *
 */
TunnelMan::TunnelMan(): Actor(TID_PLAYER, TUNNEL_MAN_START_X, TUNNEL_MAN_START_Y, right)
{
    num_squirts = 5;
    sonar_charges = 1;
    gold_nuggets = 0;
}
//------------------------------------------------------------------------------------
void TunnelMan::doSomething()
{
    int key;
    if(getWorld()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_LEFT:
                if(getX() > 0)
                    moveTo(getX()-1, getY());
        }
    }
}
//----------------------------
void TunnelMan::incGoldNugs()
{
    ++gold_nuggets;
}
//----------------------------
void TunnelMan::decGoldNugs()
{
    if(gold_nuggets > 0)
        --gold_nuggets;
}
//----------------------------
void TunnelMan::incSonarCharges()
{
    ++sonar_charges;
}
//----------------------------
void TunnelMan::decSonarCharges()
{
    if(sonar_charges > 0)
        --sonar_charges;
}
//----------------------------
void TunnelMan::incNumSquirts()
{
    ++num_squirts;
}
//----------------------------
void TunnelMan::decNumSquirts()
{
    if(num_squirts > 0)
        --num_squirts;
}
//----------------------------
