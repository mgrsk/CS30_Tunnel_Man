#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>

#define MIN_EUCLIDIAN_DIST	(6)	//Minimum distance objects must be spawned from each other

//Forwarding declarations
class Actor;
class TunnelMan;
class Ice;
class BarrelOfOil;



class StudentWorld : public GameWorld
{
private:
    size_t tickCount;     //Keeps track of how many ticks passed per life
	size_t barrelCount;		//Keeps track of number of active barrels
    
    std::unique_ptr<Ice> iceField[VIEW_WIDTH][VIEW_HEIGHT - IMAGE_OFFSET];	//Holds the ice field
    std::unique_ptr<TunnelMan> player;	//The player object
    std::list<std::unique_ptr<Actor>> gameObjects;	//All other game objects, eg. sonar, enemies, etc
    
    void makeIceField();    //Generates the ice field in the game
    void destroyIceField(); //Destroys the ice field during cleanup
	void askPlayerAndObjectsToDoSomething();	//Asks player and all objects in gameObjects list to do something
	void destroyDeadObjects();	//Removes dead objects from the gameObjects list
	void distributeBarrelsAndGold();	//Randomly distributes these barrels/gold in the field during initialization
	void generateGoodies();
	double calculateEuclidianDistance(double x1, double y1, double x2, double y2);
    
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	void setDisplayText();
    
	void decBarrels();
    void deleteIce(unsigned int xCord, unsigned int yCord);	//Deletes ice in area overlapping TunnelMan's image
	bool areaIsClear(unsigned int x, unsigned int y);	//Determines if an object can be spawned by checking area surrounding it for objects
	double getTunnelManDistance(unsigned int x, unsigned int y);	//Tells an object if TunnelMan is close enough to interact with it
    void addToTunnelManInventory(int change);	//Adds items to TunnelMan's Inventory (sonar, water, gold, etc)
	void useSonar();



};

#endif // STUDENTWORLD_H_
