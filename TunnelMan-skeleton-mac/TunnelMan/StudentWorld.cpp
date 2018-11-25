#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
void StudentWorld::makeIceField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - 4; ++j)
        {
            if(!(i >= 30 && i <= 33 && j >= 4)) //Making sure to skip the vertical shaft in the middle
            {
                iceField[i][j] = std::move(std::unique_ptr<Ice>(new Ice(i,j)));
            }
        }
    }
}

int StudentWorld::init()
{
    makeIceField();
    player = std::move(std::unique_ptr<TunnelMan>(new TunnelMan));
    player->setWorld(*this);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //player->doSomething();
    //if(player->isAlive())
    return GWSTATUS_CONTINUE_GAME;
    decLives();
    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
    //FIXME - do i need to manually clean up even though there are smart pointers?
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
