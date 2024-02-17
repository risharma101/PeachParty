#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;
class Player;
class Vortex;

const bool WAITING = 0;
const bool PAUSED = 0;
const bool WALKING = 1;

class Actor : public GraphObject
{
public:
    Actor(int imageID , int startX, int startY, int dir, int depth, double size, StudentWorld* sw);
    virtual ~Actor();
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const;
    bool isAlive() const;
    void setNotAlive();
    bool canBeImpacted() const;
    virtual void impacted();
    void setCanBeImpacted(bool canBeImpacted);
    std::vector<Player*> playerLanded();
    std::vector<Player*> playerPassing();
private:
    StudentWorld* m_world;
    bool m_alive;
    bool m_canBeImpacted;
    bool m_pFlag;
    bool m_yFlag;
};

class Avatar : public Actor
{
public:
    Avatar(int imageID, int startX, int startY, StudentWorld* sw);
    virtual void doSomething() = 0;
    bool getState() const;
    void setState(bool state);
    int getTicksToMove() const;
    void setTicksToMove(int ticksToMove);
    int getWalkDir() const;
    void setWalkDir(int dir);
    bool isNotValidDirection(int direction);
    virtual bool checkAtFork();
    int randomWalkingDir();
    virtual void teleport();
    

private:
    bool m_state;
    int m_ticks_to_move;
    int m_direction;
};

class Player : public Avatar
{
public:
    Player(int imageID, int startX, int startY, StudentWorld* sw, int playerNum);
    int getStars() const;
    void addCoins(int numCoins);
    int getCoins() const;
    void addStars(int numStars);
    bool hasVortex();
    void setHasVortex(bool b);
    bool isAtDirSquare();
    void setAtDirSquare(bool b);
    int getRoll();
    
    virtual bool checkAtFork();
    void swapCoins();
    void swapStars();
    void swapState();
    virtual void doSomething();
    virtual void teleport();
    
private:
    int m_playerNum;
    int m_coins;
    int m_stars;
    bool m_hasVortex;
    bool m_startOfGame;
    bool m_atDirSquare;
    int m_roll;
};

class Enemy : public Avatar
{
public:
    Enemy(int imageID, int startX, int startY, StudentWorld* sw);
    virtual void doSomething() = 0;
    virtual void enemyAction(Player* p) = 0;
    virtual void bowserEndAction() = 0;
    virtual void enemyPausedCode(int roll);
    virtual void enemyWalkCode();
    int getPauseCounter() const;
    void setPauseCounter(int pauseVal);
    bool sameSquare(Player* p);
    void impacted();
    virtual void teleport();
private:
    int m_pauseCounter;
};

class Bowser : public Enemy
{
public:
    Bowser(int startX, int startY, StudentWorld* sw);
    virtual void enemyAction(Player* p);
    virtual void bowserEndAction();
    virtual void doSomething();
};

class Boo : public Enemy
{
public:
    Boo(int startX, int startY, StudentWorld* sw);
    virtual void enemyAction(Player* p);
    virtual void bowserEndAction();
    virtual void doSomething();
};

class Vortex : public Actor {
public:
    Vortex(int startX, int startY, int dir, StudentWorld* sw);
    void doSomething();
    Actor* overlaps();
private:
    int m_vortexDir;
    
};

class Square : public Actor
{
public:
    Square(int imageID, int startX, int startY, int dir, int depth, double size, StudentWorld* sw);
    virtual void doSomething() = 0;
};


class CoinSquare : public Square
{
public:
    CoinSquare(int imageID, int startX, int startY, StudentWorld* sw, int value = 3);
    void doSomething();
private:
    int m_value;
};

class StarSquare : public Square
{
public:
    StarSquare(int startX, int startY, StudentWorld* sw);
//        ~StarSquare();
    void doSomething();
};

class DirSquare : public Square
{
public:
    DirSquare(int startX, int startY, StudentWorld* sw, int m_forcingDir);
//        ~DirSquare();
    void doSomething();
private:
    int m_forcingDir;
};

class BankSquare : public Square
{
public:
    BankSquare(int startX, int startY, StudentWorld* sw);
//        ~StarSquare();
    void doSomething();
};

class EventSquare : public Square
{
public:
    EventSquare(int startX, int startY, StudentWorld* sw);
//        ~StarSquare();
    void doSomething();
};

class DroppingSquare : public Square
{
public:
    DroppingSquare(int startX, int startY, StudentWorld* sw);
//        ~StarSquare();
    void doSomething();
};
#endif // ACTOR_H_
