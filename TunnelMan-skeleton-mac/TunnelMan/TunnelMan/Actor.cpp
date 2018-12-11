#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/*
 *
 ACTOR CLASS IMPLEMENTATION BELOW
 *
 */

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld * ptr, double size, int depth, bool shouldDisplay):
	GraphObject(imageID, startX, startY, startDirection, size, depth), stillAlive(true), world(ptr)
{
    setVisible(shouldDisplay);
}
//------------------------------------------
Actor::~Actor()
{
    world = nullptr;    
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
            if(getX() > 0 && getWorld()->noBouldersBlocking(getX(), getY(), left, this))
            {
                moveTo(getX() - 1, getY());
                return true;
            }
            else
                return false;
        }
        case right:
        {
            if(getX() < MAX_COORDINATE && getWorld()->noBouldersBlocking(getX(), getY(), right, this))
            {
                moveTo(getX() + 1, getY());
                return true;
            }
            else
                return false;
        }
        case up:
        {
            if(getY() < MAX_COORDINATE && getWorld()->noBouldersBlocking(getX(), getY(), up, this))
            {
                moveTo(getX(), getY() + 1);
                return true;
            }
            else
                return false;
        }
        case down:
        {
            if(getY() > 0 && getWorld()->noBouldersBlocking(getX(), getY(), down, this))
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

Earth::Earth(int startX, int startY):
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
 * BOULDER CLASS IMPLEMENTATION BELOW
 *
 */

Boulder::Boulder(int x, int y, StudentWorld * world):
Actor(TID_BOULDER, x, y, down, world, STANDARD_IMAGE_SIZE, DEPTH_BOULDER), atRest(true), ticksWaiting(0)
{
}

//------------------------------------------
void Boulder::doSomething()
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
    
    //Checking that there is no Earth or other boulders blocking the boulder from falling
    if (getWorld()->noEarthBlocking(getX(), getY(), down) && getWorld()->noBouldersBlocking(getX(), getY(), down, this))
    {
        //Checking if it can move, and moving down if so.
		if (getY() > 0)
			moveTo(getX(), getY() - 1);
		else
            setDead();  //Movement failed - rock reached the bottom.
    }
    else
        setDead();  //Earth was blocking the rock
}
//------------------------------------------
bool Boulder::isBoulder() const
{
    return true;
}
//------------------------------------------


/*
 *
 * SQUIRT CLASS IMPLEMENTATION BELOW
 *
 */

Squirt::Squirt(int x, int y, Direction startDirection, StudentWorld * world):
Actor(TID_WATER_SPURT, x, y, startDirection, world, STANDARD_IMAGE_SIZE
      , DEPTH_SQUIRT), ticksAlive(0)
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
    if(getWorld()->noEarthBlocking(getX(), getY(), getDirection()))
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
 PEOPLE CLASS IMPLEMENTATION BELOW
 *
 */

People::People(int imageID, int x, int y, Direction startDirection, StudentWorld * world, int maxHealth):
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
					if(moveInDirection(left))   //moveInDirection verifies coordinates are in bounds and no boulders are blocking
                    {
						if (!getWorld()->areaIsClearOfEarth(getX(), getY()))
                            getWorld()->playSound(SOUND_DIG);   //Playing the sound only if the area TunnelMan moved to had Earth in it
                    }
                }
                else
                    setDirection(left);
                
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(getDirection() == right)
                {
                    if(moveInDirection(right))
                    {
                        if (!getWorld()->areaIsClearOfEarth(getX(), getY()))
                            getWorld()->playSound(SOUND_DIG);
                    }
                }
                else
                    setDirection(right);
                
                break;
            }
            case KEY_PRESS_UP:
            {
                if(getDirection() == up)
                {
                    if(moveInDirection(up))
                    {
                        if (!getWorld()->areaIsClearOfEarth(getX(), getY()))
                            getWorld()->playSound(SOUND_DIG);
                    }
                }
                else
                    setDirection(up);
                
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if(getDirection() == down)
                {
                    if(moveInDirection(down))
                    {
                        if (!getWorld()->areaIsClearOfEarth(getX(), getY()))
                            getWorld()->playSound(SOUND_DIG);
                    }
                }
                else
                    setDirection(down);
                
                break;
            }
			case KEY_PRESS_TAB:
			{
				if (gold_nuggets > 0) 
				{
					getWorld()->dropGoldNug();
					--gold_nuggets;
				}
				break;
			}
            case KEY_PRESS_SPACE:
            {
                if(num_squirts > 0)
                {
                    getWorld()->useSquirtGun();
                    --num_squirts;
                }
                break;
            }
			case 'z':
            case 'Z':
			{
				if (sonar_charges > 0) 
                {
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
	if(gold_nuggets != MAX_ITEM_COUNT)
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
	if(sonar_charges != MAX_ITEM_COUNT)
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
    
	if (num_squirts > MAX_ITEM_COUNT)
		num_squirts = MAX_ITEM_COUNT;
}
//----------------------------
size_t TunnelMan::getNumSquirts() const
{
	return num_squirts;
}
//----------------------------






/*
*
PROTESTOR CLASS IMPLEMENTATION BELOW
*
*/

RegularProtester::RegularProtester(StudentWorld * world, int level, int imageID, int maxHealth, int scoreFromBeingSquirted):
    People(imageID, START_X_PROTESTOR, START_Y_PROTESTOR, left, world, maxHealth),
    leavingOilField(false), currentRestingTicks(0), levelNumber(level), stunned(false),
    totalNonRestingTicks(0), scoreFromSquirt(scoreFromBeingSquirted)
{
    ticksToWaitBetweenMoves = MAX(0, 3 - levelNumber / 4);
    setNumSquaresToMoveInCurrentDirection();
    
    //This is set to negative so that it can shout within its first 15 ticks if TunnelMan is close enough
    nonRestingTickShoutedAt = -TICKS_BETWEEN_SHOUTS;
    
    //Set to negative so that it can turn within its first 200 ticks if it hits an intersection
    lastTickTurnMade = -TICKS_BETWEEN_TURNS;
}
//------------------------------------------
void RegularProtester::doSomething()
{
    if(!isAlive())
        return;
    
    
    //Checking that the proper amount of ticks have passed for the next action
    if(currentRestingTicks < ticksToWaitBetweenMoves)
    {
        ++currentRestingTicks;
        return;
    }

    if(stunned)
    {
        //Resetting how long the protester has to wait because it's not stunned anymore
        ticksToWaitBetweenMoves = MAX(0, 3 - levelNumber / 4);
        stunned = false;
    }
    
    if(leavingOilField)
    {
        leaveOilField();
        currentRestingTicks = 0;
        return;
        
    }
    else if(getWorld()->getTunnelManDistance(getX(), getY()) <= 4.0
            && getWorld()->canProteserShoutAtTunnelMan(getX(), getY(), getDirection()))
    {
        if(totalNonRestingTicks - nonRestingTickShoutedAt >= TICKS_BETWEEN_SHOUTS)
        {
            getWorld()->shoutAtTunnelMan();
            nonRestingTickShoutedAt = totalNonRestingTicks;
        }
        currentRestingTicks = 0;
        ++totalNonRestingTicks;
        return;
    }
    else if(getWorld()->getTunnelManDistance(getX(), getY()) > 4.0
            && canProtesterDetectTunnelMan())
    {
            numSquaresToMoveInCurrentDirection = 0;
            currentRestingTicks = 0;
            ++totalNonRestingTicks;
            return;
    }
    
    --numSquaresToMoveInCurrentDirection;
    
    if(numSquaresToMoveInCurrentDirection <= 0)
    {
        pickRandomDirection();
        
        //Checking that the new direction doesn't have any earth or boulders blocking
        while(!getWorld()->noEarthBlocking(getX(), getY(), getDirection()) ||
              !getWorld()->noBouldersBlocking(getX(), getY(), getDirection(), nullptr))
        {
            pickRandomDirection(); //Generating a new direction due to blocking
        }
        
        setNumSquaresToMoveInCurrentDirection();
    }
    else    //Checking if the protester can make a turn
    {
        if(totalNonRestingTicks - lastTickTurnMade >= TICKS_BETWEEN_TURNS)  //Checking that no turns have been made in last 200 ticks
        {
            if(checkIfProtesterIsAtIntersection())
            {
                lastTickTurnMade = totalNonRestingTicks;
                setNumSquaresToMoveInCurrentDirection();
            }
        }
    }
    
    tryToMove();
    
    //Resetting # of resting ticks and increasing # of non-resting ticks because an action was performed.
    currentRestingTicks = 0;
    ++totalNonRestingTicks;
}
//------------------------------------------
void RegularProtester::tryToMove()
{
    if(getWorld()->noEarthBlocking(getX(), getY(), getDirection()))
    {
        //If function call is successful, protester will move. Function will fail if coordinates are out of bounds.
        if(!moveInDirection(getDirection()))
            numSquaresToMoveInCurrentDirection = 0;
    }
    else
        numSquaresToMoveInCurrentDirection = 0;
}
//------------------------------------------
void RegularProtester::annoy(int damage)
{
    //Checking that the protester is not already leaving the oil field, in which case it can't be annoyed.
    if(!leavingOilField)
    {
        takeDamage(damage); //Reducing its health appropriately
        
        if(getHealth() <= 0)    //Protester was annoyed completely
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            
            //Increasing the score based on how the protester was annoyed
            if(damage == DAMAGE_SQUIRT_GUN)
                getWorld()->increaseScore(scoreFromSquirt);
            else
                getWorld()->increaseScore(SCORE_PROTESTER_BONKED);
            
            leavingOilField = true;
            currentRestingTicks = ticksToWaitBetweenMoves; //Allowing protester to move immediately
        }
        else    //Protester was annoyed but still has health left
        {
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
            stunned = true; //Prevents protester from moving for a bit, based on formula below
            ticksToWaitBetweenMoves = MAX(50, 100 - levelNumber * 10);
        }
    }
}
//------------------------------------------
bool RegularProtester::bribe()
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
void RegularProtester::leaveOilField()
{
    if(getX() == MAX_COORDINATE && getY() == MAX_COORDINATE)
    {
        setDead();
        getWorld()->decNumberOfProtesters();
        return;
    }
    else
    {
        Direction directionTowardsExit = getWorld()->getDirectionToExit(getX(), getY());
        setDirection(directionTowardsExit);
        moveInDirection(directionTowardsExit);
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
bool RegularProtester::checkIfProtesterIsAtIntersection()
{
    //These two variables will store the directions that the protester can turn in, if any
    Direction direction1 = none;
    Direction direction2 = none;
    
    //If protester is facing up/down, we need to check if he can make a left/right turn, or vice versa
    switch(getDirection())
    {
        case up:
        case down:
        {
            //Checking that no Boulders or Earth can block the protester in the left direction
            if(getWorld()->noBouldersBlocking(getX(), getY(), left, nullptr) && getWorld()->noEarthBlocking(getX(), getY(), left))
            {
                direction1 = left;
            }
            if(getWorld()->noBouldersBlocking(getX(), getY(), right, nullptr) && getWorld()->noEarthBlocking(getX(), getY(), right))
            {
                direction2 = right;
            }
            break;
        }
        case right:
        case left:
        {
            if(getWorld()->noBouldersBlocking(getX(), getY(), up, nullptr) && getWorld()->noEarthBlocking(getX(), getY(), up))
            {
                direction1 = up;
            }
            if(getWorld()->noBouldersBlocking(getX(), getY(), down, nullptr) && getWorld()->noEarthBlocking(getX(), getY(), down))
            {
                direction2 = down;
            }
            break;
        }
        case none:
            return false;
    }
    
    if(direction1 != none || direction2 != none)    //Checking if at least one turn can be made
    {
        if(direction1 != none && direction2 != none)    //Two different turns can be made. Pick one randomly
        {
            int directionChoice = rand() % 2; //Generate a 0 or 1
            
            //Rick a new direction based on the randomly generated directionChoice
            if(directionChoice == 0)
                setDirection(direction1);
            else
                setDirection(direction2);
        }
        else if(direction1 != none) //Only the first turn can be made
        {
            setDirection(direction1);
        }
        else    //Only the second turn can be made
        {
            setDirection(direction2);
        }
        
        return true;
    }
    return false;
}
//------------------------------------------
bool RegularProtester::canProtesterDetectTunnelMan()
{
    GraphObject::Direction directionToTunnelMan;
    if(getWorld()->tunnelManIsInStraightLineOfSight(getX(), getY(), directionToTunnelMan))
    {
        setDirection(directionToTunnelMan);
        moveInDirection(directionToTunnelMan);
        return true;
    }
    return false;
}



/*
 *
 HARDCORE PROTESTER CLASS IMPLEMENTATION BELOW
 *
 */

HardcoreProtestor::HardcoreProtestor(StudentWorld * world, int levelNumber, int scoreFromBeingSquirted):
RegularProtester(world, levelNumber, TID_HARD_CORE_PROTESTER, DEFAULT_HEALTH_HARDCORE, scoreFromBeingSquirted)
{
}
//------------------------------------------
bool HardcoreProtestor::bribe()
{
    if(!leavingOilField)
    {
        stunned = true;
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(SCORE_BRIBE_GOLD);
        ticksToWaitBetweenMoves = MAX(50, 100 - (levelNumber * 10));
        return true;
    }
    return false;
}
//------------------------------------------
bool HardcoreProtestor::canProtesterDetectTunnelMan()
{
    int maxNumberOfMovesToDetectTunnelMan = 16 + (levelNumber * 2);
    Direction directionToTunnelMan = none;
    
    int numberOfMovesFromTunnelMan = getWorld()->getNumberOfMovesFromTunnelMan(getX(), getY(),directionToTunnelMan);
    
    if(numberOfMovesFromTunnelMan <= maxNumberOfMovesToDetectTunnelMan)
    {
        setDirection(directionToTunnelMan);
        moveInDirection(directionToTunnelMan);
        return true;
    }
    else if(getWorld()->tunnelManIsInStraightLineOfSight(getX(), getY(), directionToTunnelMan))
    {
        setDirection(directionToTunnelMan);
        moveInDirection(directionToTunnelMan);
        return true;
    }
    return false;
}





/*
 *
GOODIE CLASS IMPLEMENTATION BELOW
 *
 */


Goodie::Goodie(int imageID, int startX, int startY, StudentWorld * worldPtr, int score, int sound, int maxTicks, bool shouldDisplay):
    Actor(imageID, startX, startY, right, worldPtr, STANDARD_IMAGE_SIZE, DEPTH_GOODIE, shouldDisplay),
	scoreValue(score), soundToPlay(sound), ticksPassed(0), maxTickLife(maxTicks)
{
}
//------------------------------------------
bool Goodie::checkIfTunnelManPickedUp(const double &distanceFromTunnelMan)
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

BarrelOfOil::BarrelOfOil(int startX, int startY, StudentWorld * world):
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

GoldNugget::GoldNugget(int startX, int startY, StudentWorld * world, bool shouldDisplay, bool canPickup):
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

WaterPool::WaterPool(int startX, int startY, StudentWorld * world, int maxTicks):
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

Sonar::Sonar(int startX, int startY, StudentWorld * world, int maxTicks):
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

