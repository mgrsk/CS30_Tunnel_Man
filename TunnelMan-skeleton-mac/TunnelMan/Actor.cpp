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
    
}
//------------------------------------------
bool Actor::isAlive()
{
    return stillAlive;
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
void Actor::setWorld(StudentWorld * worldPtr)
{
    this->world = std::unique_ptr<StudentWorld>(worldPtr);
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
    
    //First, get the key that was pressed and respond accordingly
    if(getWorld()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_LEFT:
            {
                if(getX() > 0)
                {
                    moveTo(getX()-1, getY());
                    setDirection(left);
                    
                }
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(getX() < VIEW_WIDTH - IMAGE_OFFSET)
                {
                    moveTo(getX()+1, getY());
                    setDirection(right);
                }
                break;
            }
            case KEY_PRESS_UP:
            {
                if(getY() < VIEW_HEIGHT - IMAGE_OFFSET)
                {
                    moveTo(getX(), getY() + 1);
                    setDirection(up);
                }
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if(getY() > 0)
                {
                    moveTo(getX(), getY() - 1);
                    setDirection(down);
                }
                break;
            }
        }
    }
    
    //Next, delete any ice at TunnelMan's current position
    getWorld()->deleteIce(getX(), getY());
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
