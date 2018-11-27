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
private:
    size_t ticksPassed;     //Keeps track of how many ticks passed per life
    
    std::unique_ptr<Ice> iceField[VIEW_WIDTH][VIEW_HEIGHT - IMAGE_OFFSET];
    std::unique_ptr<TunnelMan> player;
    std::vector<std::unique_ptr<Actor>> gameObjects; //Not needed for part 1. Will be implemented in part 2
    
    
    void makeIceField();    //Generates the ice field in the game
    void destroyIceField(); //Destroys the ice field during cleanup
    
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    void deleteIce(unsigned int xCord, unsigned int yCord);
    


};

#endif // STUDENTWORLD_H_
