#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
//---------------------------------------------------------------
void StudentWorld::makeIceField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - IMAGE_OFFSET; ++j)
        {
            if(!(i >= 30 && i <= 33 && j >= 4)) //Making sure to skip the vertical shaft in the middle
            {
                iceField[i][j] = std::unique_ptr<Ice>(new Ice(i,j));
            }
        }
    }
}
//---------------------------------------------------------------
int StudentWorld::init()
{
    makeIceField();
    player = std::unique_ptr<TunnelMan>(new TunnelMan);
    player->setWorld(this);
    return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    player->doSomething();
    if(player->isAlive())
    return GWSTATUS_CONTINUE_GAME;
    decLives();
    return GWSTATUS_PLAYER_DIED;
}
//---------------------------------------------------------------
void StudentWorld::cleanUp()
{
    //FIXME - do i need to manually clean up even though there are smart pointers?
    //Make this its own function if it's necessary
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - IMAGE_OFFSET; ++j)
        {
            iceField[i][j] = nullptr;
        }
    }
    
    
    
    
}
//---------------------------------------------------------------
void StudentWorld::deleteIce(unsigned int xCord, unsigned int yCord) //FIXME - not deleting properly
{
    int newX;   //Will store the x-Coordinates near TunnelMan
    int newY;   //Will store the y-Coordinates near TunnelMan
    
    for(int i = 0; i < IMAGE_OFFSET; ++i)
    {
        for(int j = 0; j < IMAGE_OFFSET; ++j)
        {
            newX = xCord + i;
            newY = yCord + j;
            
            //Validating that coordinate is in bounds
            if(newX < VIEW_WIDTH && newY < VIEW_HEIGHT - IMAGE_OFFSET)
                iceField[i][j] = nullptr;
        }
    }
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
