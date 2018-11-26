#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <vector>
//Forwarding declarations
class Actor;
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
    
    void deleteIce(unsigned int xCord, unsigned int yCord);
    

private:
    std::unique_ptr<Ice> iceField[VIEW_WIDTH][VIEW_HEIGHT - IMAGE_OFFSET];
    std::unique_ptr<TunnelMan> player;
    std::vector<std::unique_ptr<Actor>> gameObjects;
    
    
    void makeIceField();
};

#endif // STUDENTWORLD_H_
