#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>



// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	virtual int init()
	{
        makeIceField();
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	virtual void cleanUp()
	{
	}

private:
    std::unique_ptr<Ice> iceField[NUM_ICE_COLUMNS][NUM_ICE_ROWS];
    void makeIceField()//FIXME - move to cpp
    {
        for(int i=0; i<NUM_ICE_COLUMNS; ++i)
        {
            for(int j=0; j<NUM_ICE_ROWS; ++j)
            {
                if(!(i >= 30 && i <= 33 && j >= 4)) //Making sure to skip the vertical shaft in the middle
                {
                    iceField[i][j] = std::move(std::unique_ptr<Ice>(new Ice(i,j)));
                }
            }
        }
    }
};

#endif // STUDENTWORLD_H_
