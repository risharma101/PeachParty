#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>
#include "Actor.h"
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Board* getBoard() const;
    Board::GridEntry getContentsOf(int gx, int gy) const;
    Player* getPeach();
    Player* getYoshi();
    int getBankBalance();
    void addBankBalance(int addedBalance);
    void newDroppingSquare(int x, int y);
    int teleportHelper();
    void createVortex(Player* p);
    Actor* overlapsHelper(int x, int y, int dir);

private:
    std::vector<Actor*> actors;
    Player* peach;
    Player* yoshi;
    Board* m_board;
    Board::GridEntry m_grid[BOARD_HEIGHT][BOARD_WIDTH];
    int m_bankBalance;
};

#endif // STUDENTWORLD_H_
