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
	GraphObject(imageID, startX, startY, startDirection, size, depth), stillAlive(true), world(ptr)
{
    setVisible(shouldDisplay);
}
//------------------------------------------
Actor::~Actor()
{
    world = nullptr;    //FIXME - is this needed?
}
//------------------------------------------
bool Actor::isAlive() const
{
    return stillAlive;
}
//------------------------------------------
void Actor::setDead()
{
    stillAlive = false;
}
//------------------------------------------
StudentWorld * Actor::getWorld() const
{
    return world;
}
//------------------------------------------
bool Actor::moveInDirection(Direction d)
{
    switch(d)
    {
        case left:
        {
            if(getX() > 0)
            {
                moveTo(getX() - 1, getY());
                return true;
            }
            else
                return false;
        }
        case right:
        {
            if(getX() < MAX_COORDINATE)
            {
                moveTo(getX() + 1, getY());
                return true;
            }
            else
                return false;
        }
        case up:
        {
            if(getY() < MAX_COORDINATE)
            {
                moveTo(getX(), getY() + 1);
                return true;
            }
            else
                return false;
        }
        case down:
        {
            if(getY() > 0)
            {
                moveTo(getX(), getY() - 1);
                return true;
            }
            else
                return false;
        }
        case none:
            return false;
    }
}
//------------------------------------------
bool Actor::canBeAnnoyed() const
{
	return false;
}
//------------------------------------------
bool Actor::isBoulder() const
{
    return false;
}
//------------------------------------------
bool Actor::bribe()
{
    return false;
}  //To be overrided by necessary classes
//------------------------------------------
void Actor::annoy(int damage) {}  //To be overrided by necessary classes
//------------------------------------------

/*
 *
 EARTH CLASS IMPLEMENTATION BELOW
 *
 */

Earth::Earth(unsigned int startX, unsigned int startY):
	Actor(TID_EARTH, startX, startY, right, nullptr, EARTH_SIZE, DEPTH_EARTH)
{
}
//------------------------------------------------------------------------------------
void Earth::doSomething() //Earth does nothing
{
    return;
}
//------------------------------------------------------------------------------------



/*
 *
 PEOPLE CLASS IMPLEMENTATION BELOW
 *
 */

People::People(int imageID, unsigned int x, unsigned int y, Direction startDirection, StudentWorld * world, int maxHealth):
Actor(imageID, x, y, startDirection, world), health(maxHealth)
{
}
//------------------------------------
bool People::canBeAnnoyed() const
{
    return true;
}
//------------------------------------
int People::getHealth() const
{
    return health;
}
//------------------------------------
void People::takeDamage(int damage)
{
    health -= damage;
}
//------------------------------------

/*
 *
 TUNNELMAN CLASS IMPLEMENTATION BELOW
 *
 */
TunnelMan::TunnelMan(StudentWorld * world):
    People(TID_PLAYER, START_X_TUNNELMAN, START_Y_TUNNELMAN, right, world, DEFAULT_HEALTH_TUNNELMAN),
    num_squirts(DEFAULT_WATER_SQUIRTS), sonar_charges(DEFAULT_SONAR_CHARGES), gold_nuggets(DEFAULT_GOLD_NUGGETS)
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
                if(getDirection() == left)
                {
                    if(getWorld()->noBouldersBlocking(getX(), getY(), left))
                        moveInDirection(left);
                }
                else
                    setDirection(left);
                
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(getDirection() == right)
                {
                    if(getWorld()->noBouldersBlocking(getX(), getY(), right))
                        moveInDirection(right);
                }
                else
                    setDirection(right);
                
                break;
            }
            case KEY_PRESS_UP:
            {
                if(getDirection() == up)
                {
                    if(getWorld()->noBouldersBlocking(getX(), getY(), up))
                        moveInDirection(up);
                }
                else
                    setDirection(up);
                
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if(getDirection() == down)
                {
                    if(getWorld()->noBouldersBlocking(getX(), getY(), down))
                        moveInDirection(down);
                }
                else
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
            case KEY_PRESS_SPACE:
            {
                if(num_squirts > 0)
                {
                    getWorld()->useSquirtGun(getDirection());
                    --num_squirts;
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
    
    //Next, delete any earth at TunnelMan's current position
    getWorld()->deleteEarthAroundObject(getX(), getY());
}
//----------------------------
void TunnelMan::annoy(int damage)
{
    takeDamage(damage);
    
    if(getHealth() <= 0)
        setDead();
}
//----------------------------
void TunnelMan::incGoldNugs()
{
	if(gold_nuggets != 99)
		++gold_nuggets;
}
//----------------------------
size_t TunnelMan::getGoldNugs() const
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
size_t TunnelMan::getSonarCharges() const
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
size_t TunnelMan::getNumSquirts() const
{
	return num_squirts;
}
//------------------------------------------

/*
 *
 * SQUIRT CLASS IMPLEMENTATION BELOW
 *
 */

Squirt::Squirt(unsigned int x, unsigned int y, Direction startDirection, StudentWorld * world):
Actor(TID_WATER_SPURT, x, y, startDirection, world, false, 1.0, DEPTH_SQUIRT), ticksAlive(0)
{
}
//------------------------------------------
void Squirt::doSomething()
{
    if(!isAlive())
        return;
    
    //Checking if the squirt has been alive for too long
    if(ticksAlive == SQUIRT_LIFETIME)
    {
        setDead();
        return;
    }
    
    //Checking if the squirt hit anything. If it did, it will be destroyed.
    if(getWorld()->checkForSquirtGunHits(getX(), getY()))
    {
        setDead();
        return;
    }

    ///Checking that there is no earth or boulders in the way
    if(getWorld()->noEarthBlocking(getX(), getY(), getDirection())
       && getWorld()->noBouldersBlocking(getX(), getY(), getDirection()))
    {
        //Tries to move. If successful, moves in its current direction.
        if(!moveInDirection(getDirection()))
            setDead();  //Movement failed because it went out of bounds
    }
    else
        setDead();  //Squirt hit boulder or Earth
    
    ++ticksAlive;
}
//------------------------------------------

/*
*
* BOULDER CLASS IMPLEMENTATION BELOW
*
*/

Boulder::Boulder(unsigned int x, unsigned int y, StudentWorld * world):
	Actor(TID_BOULDER, x, y, down, world, STANDARD_IMAGE_SIZE, DEPTH_BOULDER), atRest(true), ticksWaiting(0)
{
}

//------------------------------------------
void Boulder::doSomething() //FIXME - implement
{
	if (!isAlive())
		return; 

	if(atRest) 
	{
		if (getWorld()->noEarthBlocking(getX(), getY(), down))
			atRest = false;
	}
	else 
	{
		if (ticksWaiting >= MAX_TICKS_BOULDER_WAITING)
		{
			fall();
            
            //Playing the sound if this is the initial fall
            if (ticksWaiting == MAX_TICKS_BOULDER_WAITING)
                getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		++ticksWaiting;
	}
	
}
//------------------------------------------
void Boulder::fall()
{
	getWorld()->checkForBoulderHits(getX(), getY());
	if (getWorld()->noEarthBlocking(getX(), getY(), down))
    {
        //Checking if it can move, and moving down if so.
		if(!moveInDirection(down))
            setDead();  //Movement failed - rock reached the bottom. 
    }
    else
        setDead();
}
//------------------------------------------
bool Boulder::isBoulder() const
{
    return true;
}
//------------------------------------------

/*
*
PROTESTOR CLASS IMPLEMENTATION BELOW
*
*/

RegularProtester::RegularProtester(StudentWorld * world, int level, int imageID, int maxHealth):
    People(imageID, START_X_PROTESTOR, START_Y_PROTESTOR, left, world, DEFAULT_HEALTH_PROTESTER),
    leavingOilField(false), currentRestingTicks(0), levelNumber(level), stunned(false), totalNonRestingTicks(0)
{
    ticksBetweenMoves = MAX(0, 3 - levelNumber / 4);
    setNumSquaresToMoveInCurrentDirection();
    
    //This is set to -15 so that it can immediately shout at tunnelman once created
    nonRestingTickShoutedAt = -TICKS_BETWEEN_SHOUTS;
}
//------------------------------------------
void RegularProtester::doSomething() //FIXME - implement
{
    if(!isAlive())
        return;
    
    //Checking that the proper amount of ticks have passed for the next action
    //Does something every (ticksBetweenMoves + 1) ticks
    if(currentRestingTicks < ticksBetweenMoves)
    {
        ++currentRestingTicks;
        return;
    }

    if(stunned)
    {
        //Resetting how long the protester has to wait because it's not stunned anymore
        ticksBetweenMoves = MAX(0, 3 - levelNumber / 4);
        stunned = false;
    }
    
    if(leavingOilField)
    {
        leaveOilField(); //FIXME - implement
    }
    else if(getWorld()->canProteserShoutAtTunnelMan(getX(), getY(), getDirection()))
    {
        //Checking that it has been at least 15 ticks since a shout occurred
        if(totalNonRestingTicks - nonRestingTickShoutedAt >= TICKS_BETWEEN_SHOUTS)
        {
            getWorld()->shoutAtTunnelMan();
            nonRestingTickShoutedAt = totalNonRestingTicks;
        }
    }
    else if(getWorld()->tunnelManIsInStraightLineOfSight(getX(), getY()))
    {
        return; //FIXME - implement
    }
    
    --numSquaresToMoveInCurrentDirection;
    
    if(numSquaresToMoveInCurrentDirection == 0)
    {
        pickRandomDirection();
        
        //Checking that the new direction doesn't have any earth or boulders blocking
        while(!getWorld()->noEarthBlocking(getX(), getY(), getDirection()) ||
              !getWorld()->noBouldersBlocking(getX(), getY(), getDirection()))
        {
            pickRandomDirection(); //Generating a new direction due to blocking
        }
        
        setNumSquaresToMoveInCurrentDirection();
    }
    else
    {
        //checkIfProtesterIsAtIntersection; //FIXME - implement
    }
    
    tryToMove();
    
    //Resetting the number of resting ticks and increasing
    //number of non-resting ticks because an action was performed.
    currentRestingTicks = 0;
    ++totalNonRestingTicks;
}
//------------------------------------------
void RegularProtester::tryToMove()
{
    if(getWorld()->noBouldersBlocking(getX(), getY(), getDirection())
       && getWorld()->noEarthBlocking(getX(), getY(), getDirection()))
    {
        //If function call is successful, protester will move. Otherwise it will not.
        if(!moveInDirection(getDirection()))
            numSquaresToMoveInCurrentDirection = 0;
    }
    else
        numSquaresToMoveInCurrentDirection = 0;
}
//------------------------------------------
void RegularProtester::annoy(int damage) //FIXME - implement
{
    //Checking that the protester is not already leaving the oil field
    if(!leavingOilField)
    {
        takeDamage(damage);
        if(getHealth() <= 0)
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            
            if(damage == DAMAGE_SQUIRT_GUN)
                getWorld()->increaseScore(SCORE_PROTESTER_SQUIRTED);
            else
                getWorld()->increaseScore(SCORE_PROTESTER_BONKED);
            
            leavingOilField = true;
            currentRestingTicks = ticksBetweenMoves; //Allowing protester to move immediately
        }
        else
        {
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
            stunned = true;
            ticksBetweenMoves = MAX(50, 100 - levelNumber * 10);
        }
    }
}
//------------------------------------------
bool RegularProtester::bribe() //FIXME - not working properly when protester is annoyed
{
    if(!leavingOilField)
    {
        leavingOilField = true;
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(SCORE_BRIBE_GOLD);
        return true;
    }
    return false;
}
//------------------------------------------
void RegularProtester::leaveOilField() //FIXME - implement
{
    if(getX() == MAX_COORDINATE && getY() == MAX_COORDINATE)
    {
        setDead();
        getWorld()->decNumberOfProtesters();
        return;
    }
    
    else if(true) //FIXME - implement
    {
        return;
    }
}
//------------------------------------------
void RegularProtester::setNumSquaresToMoveInCurrentDirection()
{
    //Generates a random number between 8 and 60, inclusive
    numSquaresToMoveInCurrentDirection = (rand() % 53) + 8;
}
//------------------------------------------
void RegularProtester::pickRandomDirection()
{
    int choice = rand() % 4;
    switch(choice)
    {
        case 0:
        {
            setDirection(up);
            break;
        }
        case 1:
        {
            setDirection(right);
            break;
        }
        case 2:
        {
            setDirection(down);
            break;
        }
        case 3:
        {
            setDirection(left);
            break;
        }
    }
}
//------------------------------------------

/*
 *
 HARDCORE PROTESTER CLASS IMPLEMENTATION BELOW
 *
 */

HardcoreProtestor::HardcoreProtestor(StudentWorld * world, int levelNumber):
RegularProtester(world, levelNumber, TID_HARD_CORE_PROTESTER, DEFAULT_HEALTH_HARDCORE)
{
}

void HardcoreProtestor::doSomething(){}
//------------------------------------------

/*
 *
GOODIE CLASS IMPLEMENTATION BELOW
 *
 */


Goodie::Goodie(int imageID, unsigned int startX, unsigned int startY, StudentWorld * worldPtr, int score, int sound, int maxTicks, bool shouldDisplay):
    Actor(imageID, startX, startY, right, worldPtr, STANDARD_IMAGE_SIZE, DEPTH_GOODIE, shouldDisplay),
	scoreValue(score), soundToPlay(sound), ticksPassed(0), maxTickLife(maxTicks)
{
}
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
int Goodie::getMaxTickLife() const
{
	return maxTickLife;
}
//------------------------------------------
int Goodie::getTicksPassed() const
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
	else	//This was gold that TunnelMan dropped 
    {
        checkIfProtestorPickedUp();
    }
}
//------------------------------------------
void GoldNugget::checkIfProtestorPickedUp()
{
    if (getTicksPassed() == getMaxTickLife())
    {
        setDead();
    }
    else
    {
        if (getWorld()->checkForBribes(getX(), getY()))
        {
            setDead();
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            getWorld()->increaseScore(SCORE_BRIBE_GOLD);
        }
        
        incTicksPassed();
    }
}
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
void WaterPool::doSomething() 
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

void Sonar::doSomething()
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

