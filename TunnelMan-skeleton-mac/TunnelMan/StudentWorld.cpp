#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
//---------------------------------------------------------------
StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir)
{
}
//---------------------------------------------------------------
StudentWorld::~StudentWorld()   //Destructor is empty because cleanup() handles this process
{
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
    ticksPassed = 0;
    makeIceField();
    player = std::unique_ptr<TunnelMan>(new TunnelMan);
    player->setWorld(this);
    return GWSTATUS_CONTINUE_GAME;
}
//---------------------------------------------------------------
int StudentWorld::move()
{
    player->doSomething();
    ++ticksPassed;  //FIXME - find proper place for this in the function
    if(player->isAlive())
        return GWSTATUS_CONTINUE_GAME;
    decLives();
    return GWSTATUS_PLAYER_DIED;
}
//---------------------------------------------------------------
void StudentWorld::cleanUp()
{
    destroyIceField();  //De-initializes all ice objects
}
//---------------------------------------------------------------
void StudentWorld::deleteIce(unsigned int xCord, unsigned int yCord)
{
    int newX;   //Will store the x-Coordinates in TunnelMan's 4x4 image
    int newY;   //Will store the y-Coordinates in TunnelMan's 4x4 image
    
    for(int i = 0; i < IMAGE_OFFSET; ++i)   //Iterating through x coordinates of image
    {
        for(int j = 0; j < IMAGE_OFFSET; ++j)   //Iterating through y coordinates of image
        {
            newX = xCord + i;
            newY = yCord + j;
            
            //Validating that coordinate is in bounds
            if(newX < VIEW_WIDTH && newY < VIEW_HEIGHT - IMAGE_OFFSET)
                iceField[newX][newY] = nullptr;     //Frees unique pointer and deletes ice at that point
        }
    }
}
//---------------------------------------------------------------
void StudentWorld::destroyIceField()
{
    for(int i = 0; i < VIEW_WIDTH; ++i)
    {
        for(int j = 0; j < VIEW_HEIGHT - IMAGE_OFFSET; ++j)
        {
            iceField[i][j] = nullptr;   //Frees unique pointer and deletes ice at that point
        }
    }
}
