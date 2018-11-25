#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
//Forwarding declarations
class TunnelMan;
class Ice;



class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

    virtual int init();

    virtual int move();

    virtual void cleanUp();

private:
    std::unique_ptr<Ice> iceField[VIEW_WIDTH][VIEW_HEIGHT - 4];
    std::unique_ptr<TunnelMan> player;
    
    
    void makeIceField();
};

#endif // STUDENTWORLD_H_
