#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/*
 *
 ACTOR CLASS IMPLEMENTATION BELOW
 *
 */

Actor::Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection, StudentWorld * ptr, double size, int depth, bool shouldDisplay): 
	GraphObject(imageID, startX, startY, startDirection, size, depth), stillAlive(true)
{
	setWorld(ptr);
    setVisible(shouldDisplay);
}
//------------------------------------------
Actor::~Actor()
{
    world = nullptr;    //FIXME - is this needed?
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
StudentWorld * Actor::getWorld()
{
    return world;
}
//------------------------------------------
void Actor::setWorld(StudentWorld * worldPtr)
{
    this->world = worldPtr;
}



//------------------------------------------
/*
 *
 ICE CLASS IMPLEMENTATION BELOW
 *
 */

Ice::Ice(unsigned int startX, unsigned int startY): Actor(TID_EARTH, startX, startY, right, nullptr , ICE_SIZE, ICE_DEPTH)
{
}
//------------------------------------------------------------------------------------
void Ice::doSomething() //Implemented only because it ineherents from pure virtual function
{
    return;
}
//------------------------------------------------------------------------------------



/*
 *
 TUNNELMAN CLASS IMPLEMENTATION BELOW
 *
 */
TunnelMan::TunnelMan(StudentWorld * world): Actor(TID_PLAYER, TUNNEL_MAN_START_X, TUNNEL_MAN_START_Y, right, world)
{
    num_squirts = 5;
    sonar_charges = 1;
    gold_nuggets = 0;
}
//------------------------------------------------------------------------------------
TunnelMan::~TunnelMan() //Function is empty because no deallocation is needed at this time
{
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
                if(getX() > 0 && getDirection() == left)
                {
                    moveTo(getX()-1, getY());
                }
                setDirection(left);
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(getX() < MAX_COORDINATE && getDirection() == right)
                {
                    moveTo(getX()+1, getY());
                }
                setDirection(right);
                break;
            }
            case KEY_PRESS_UP:
            {
                if(getY() < MAX_COORDINATE && getDirection() == up)
                {
                    moveTo(getX(), getY() + 1);
                }
                setDirection(up);
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if(getY() > 0 && getDirection() == down)
                {
                    moveTo(getX(), getY() - 1);
                }
                setDirection(down);
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



//------------------------------------------

/*
 *
GOODIE CLASS IMPLEMENTATION BELOW
 *
 */


Goodie::Goodie(int imageID, unsigned int startX, unsigned int startY, StudentWorld * worldPtr, bool shouldDisplay, bool pickup):
    Actor(imageID, startX, startY, right, worldPtr, STANDARD_IMAGE_SIZE, GOODIE_DEPTH, shouldDisplay),
    canBePickedUp(pickup)
{
}
//-----------------------------------------------------------
bool Goodie::checkIfTunnelManPickedUp(int soundToPlay, int scoreToIncrease)
{
    double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());
    
    if(distanceFromTunnelMan <= 3.0)
    {
        setDead();
        getWorld()->playSound(soundToPlay);
        getWorld()->increaseScore(scoreToIncrease);
        return true;
    }
    return false;
}


/*
*
BARREL OF OIL CLASS IMPLEMENTATION BELOW
*
*/

BarrelOfOil::BarrelOfOil(unsigned int startX, unsigned int startY, StudentWorld * world): 
	Goodie(TID_BARREL, startX, startY, world, false, true)
{
}
//------------------------------------------
void BarrelOfOil::doSomething()
{
    if(!isAlive())
        return;

	double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());

	if (distanceFromTunnelMan <= 4.0 && !isVisible())
	{
		setVisible(true);
		return;
	}
	else if(checkIfTunnelManPickedUp(SOUND_FOUND_OIL, 1000))
    {
		getWorld()->decBarrels();
	}
}


/*
 *
 GOLDNUGGET CLASS IMPLEMENTATION BELOW
 *
 */