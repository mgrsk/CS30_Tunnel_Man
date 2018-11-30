#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/*
 *
 ACTOR CLASS IMPLEMENTATION BELOW
 *
 */

Actor::Actor(int imageID, unsigned int startX, unsigned int startY, Direction startDirection, StudentWorld * ptr,
	bool annoyable, double size, int depth, bool shouldDisplay): 
	GraphObject(imageID, startX, startY, startDirection, size, depth), stillAlive(true), world(ptr), canBeAnnoyed(annoyable)
{
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
bool Actor::getCanBeAnnoyed() 
{
	return canBeAnnoyed;
}
//------------------------------------------

//------------------------------------------
/*
 *
 ICE CLASS IMPLEMENTATION BELOW
 *
 */

Ice::Ice(unsigned int startX, unsigned int startY): 
	Actor(TID_EARTH, startX, startY, right, nullptr, false, ICE_SIZE, ICE_DEPTH)
{
}
//------------------------------------------------------------------------------------
void Ice::doSomething() //Implemented only because it ineherents from pure virtual function
{
    return;
}
//------------------------------------------------------------------------------------
void Ice::annoy(int damage) {}
//-----------------------------------
void Ice::bribe() {}
//------------------------------------------

/*
 *
 TUNNELMAN CLASS IMPLEMENTATION BELOW
 *
 */
TunnelMan::TunnelMan(StudentWorld * world): 
	Actor(TID_PLAYER, TUNNEL_MAN_START_X, TUNNEL_MAN_START_Y, right, world, true),
	num_squirts(5), sonar_charges(1), gold_nuggets(0), health(10)
{
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
			case KEY_PRESS_TAB:
			{
				if (gold_nuggets > 0) 
				{
					getWorld()->dropGoldNug(getX(), getY());
					--gold_nuggets;
				}
				break;
			}
			case 'z':
			{
				if (sonar_charges > 0) 
				{
					getWorld()->playSound(SOUND_SONAR);
					getWorld()->useSonar();
					--sonar_charges;
				}
				break;
			}
        }
    }
    
    //Next, delete any ice at TunnelMan's current position
    getWorld()->deleteIceAroundObject(getX(), getY());
}
//----------------------------
void TunnelMan::annoy(int damage) //FIXME - implement
{

}
//----------------------------
void TunnelMan::bribe() {}
//----------------------------
void TunnelMan::incGoldNugs()
{
	if(gold_nuggets != 99)
		++gold_nuggets;
}
//----------------------------
void TunnelMan::decGoldNugs()
{
    if(gold_nuggets > 0)
        --gold_nuggets;
}
//----------------------------
size_t TunnelMan::getGoldNugs() 
{
	return gold_nuggets;
}
//----------------------------
void TunnelMan::incSonarCharges()
{
	if(sonar_charges != 99)
		++sonar_charges;
}
//----------------------------
size_t TunnelMan::getSonarCharges() 
{
	return sonar_charges;
}
//----------------------------
void TunnelMan::increaseNumSquirts()
{
	num_squirts += 5;
	if (num_squirts > 99)
		num_squirts = 99;
}
//----------------------------
void TunnelMan::decNumSquirts()
{
    if(num_squirts > 0)
        --num_squirts;
}
//----------------------------
size_t TunnelMan::getNumSquirts() 
{
	return num_squirts;
}
//----------------------------
size_t TunnelMan::getHealth() 
{
	return health;
}

/*
*
PROTESTOR CLASS IMPLEMENTATION BELOW
*
*/


void Protester::doSomething() //FIXME - implement
{}
//------------------------------------------
void Protester::annoy(int damage) //FIXME - implement
{}
//------------------------------------------
void Protester::bribe() //FIXME - implement
{}
//------------------------------------------
void Protester::leaveOilField() //FIXME - implement
{}
//------------------------------------------

/*
 *
GOODIE CLASS IMPLEMENTATION BELOW
 *
 */


Goodie::Goodie(int imageID, unsigned int startX, unsigned int startY, StudentWorld * worldPtr, int score, int sound, int maxTicks, bool shouldDisplay):
    Actor(imageID, startX, startY, right, worldPtr, false, STANDARD_IMAGE_SIZE, GOODIE_DEPTH, shouldDisplay),
	scoreValue(score), soundToPlay(sound), ticksPassed(0), maxTickLife(maxTicks)
{
}
//-----------------------------------------------------------
void Goodie::annoy(int damage) {}
//------------------------------------------
void Goodie::bribe() {}
//------------------------------------------
bool Goodie::checkIfTunnelManPickedUp(const double & distanceFromTunnelMan)
{
    if(distanceFromTunnelMan <= PICKUP_DISTANCE)
    {
        setDead();
        getWorld()->playSound(soundToPlay);
        getWorld()->increaseScore(scoreValue);
        return true;
    }
    return false;
}
//------------------------------------------
int Goodie::getMaxTickLife() 
{
	return maxTickLife;
}
//------------------------------------------
int Goodie::getTicksPassed() 
{
	return ticksPassed;
}
//------------------------------------------
void Goodie::incTicksPassed() 
{
	++ticksPassed;
}
//------------------------------------------

/*
*
BARREL OF OIL CLASS IMPLEMENTATION BELOW
*
*/

BarrelOfOil::BarrelOfOil(unsigned int startX, unsigned int startY, StudentWorld * world): 
	Goodie(TID_BARREL, startX, startY, world, SCORE_OIL, SOUND_FOUND_OIL, 0, false)
{
}
//------------------------------------------
void BarrelOfOil::doSomething()
{
    if(!isAlive())
        return;

	double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());

	if (distanceFromTunnelMan <= MAX_DISTANCE_INVISIBLE && !isVisible())
	{
		setVisible(true);
		return;
	}
	else if(checkIfTunnelManPickedUp(distanceFromTunnelMan))
    {
		getWorld()->decBarrels();
	}
}
//------------------------------------------

/*
 *
 GOLDNUGGET CLASS IMPLEMENTATION BELOW
 *
 */

GoldNugget::GoldNugget(unsigned int startX, unsigned int startY, StudentWorld * world, bool shouldDisplay, bool canPickup): 
	Goodie(TID_GOLD, startX, startY, world, SCORE_PICKUP_GOLD, SOUND_GOT_GOODIE, GOLD_LIFETIME, shouldDisplay),
	canBePickedUpByTunnelMan(canPickup)
{
}
//------------------------------------------
void GoldNugget::doSomething()
{
	if (!isAlive())
		return;

	//Checking if this is gold that was hidden originally
    if(canBePickedUpByTunnelMan)
    {   
        double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());
        
        if (distanceFromTunnelMan <= MAX_DISTANCE_INVISIBLE && !isVisible())
        {
            setVisible(true);
            return;
        }
        else if(checkIfTunnelManPickedUp(distanceFromTunnelMan))
        {
            getWorld()->addToTunnelManInventory(ADD_GOLD_NUGGET);
        }
    }
	//This was gold that TunnelMan dropped 
    else //FIXME - implement protestor bribe code
    {
		if (getTicksPassed() == getMaxTickLife())
		{
			setDead();
		}
		else 
		{
			if (getWorld()->checkForBribes(getX(), getY()))
				setDead();

			incTicksPassed();
		}
    }
}
//------------------------------------------
void GoldNugget::checkIfProtestorPickedUp(){}


//------------------------------------------

/*
*
* WATER POOL CLASS IMPLEMENTATION BELOW
*
*/

WaterPool::WaterPool(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks): 
	Goodie(TID_WATER_POOL, startX, startY, world, SCORE_WATER_POOL, SOUND_GOT_GOODIE, maxTicks, true)
{	
}
//------------------------------------------
void WaterPool::doSomething() //FIXME - implement temporary lifespan
{
	if (!isAlive())
		return;

	if (getTicksPassed() == getMaxTickLife())
		setDead();
	else
	{
		double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());

		if (checkIfTunnelManPickedUp(distanceFromTunnelMan))
		{
			getWorld()->addToTunnelManInventory(ADD_SQUIRTS);
		}

		incTicksPassed();
	}
}
//------------------------------------------

/*
*
* SONAR CLASS IMPLEMENTATION BELOW
*
*/

Sonar::Sonar(unsigned int startX, unsigned int startY, StudentWorld * world, int maxTicks):
	Goodie(TID_SONAR, startX, startY, world, SCORE_SONAR, SOUND_GOT_GOODIE, maxTicks, true)
{
}

void Sonar::doSomething() //FIXME - implement temporary lifespan
{
	if (!isAlive())
		return;

	if (getTicksPassed() == getMaxTickLife()) 
		setDead();
	else
	{
		double distanceFromTunnelMan = getWorld()->getTunnelManDistance(getX(), getY());

		if (checkIfTunnelManPickedUp(distanceFromTunnelMan))
		{
			getWorld()->addToTunnelManInventory(ADD_SONAR);
		}

		incTicksPassed();
	}
}

