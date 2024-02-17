#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath)
{
    peach = nullptr;
    yoshi = nullptr;
    for (int gy = 0; gy < BOARD_HEIGHT; gy++)
        for (int gx = 0; gx < BOARD_WIDTH; gx++)
            m_grid[gy][gx] = Board::empty;
    m_bankBalance = 0;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

Board* StudentWorld::getBoard() const {return m_board;}

Board::GridEntry StudentWorld::getContentsOf(int gx, int gy) const
{
    if (gx < 0  ||  gx >= BOARD_WIDTH  ||  gy < 0  ||  gy >= BOARD_HEIGHT)
        return Board::empty;
    return m_grid[gy][gx];
}

Player* StudentWorld::getPeach() {return peach;}
Player* StudentWorld::getYoshi() {return yoshi;}
int StudentWorld::getBankBalance() {return m_bankBalance;};
void StudentWorld::addBankBalance(int addedBalance) {
    if (m_bankBalance + addedBalance < 0) m_bankBalance = 0;
    else m_bankBalance += addedBalance;
};
void StudentWorld::newDroppingSquare(int x, int y) { // creates a Dropping Square when the Bowser class signals it to
    for (int i = 0; i < actors.size(); i++) {
        if (actors[i]->canBeImpacted()) continue; // makes sure Bowser and/or Boos aren't set inalive
        if (actors[i]->getX() == x && actors[i]->getY() == y) {
            actors[i]->setNotAlive(); // sets the previous square at the location as inalive
            actors.push_back(new DroppingSquare(x, y, this)); // creates the new Dropping square
            break;
        }
    }
}

int StudentWorld::teleportHelper() { // returns x and y int for the Avatar's teleport() method to teleport the Avatar to
    while(true) {
        int r = randInt(0, actors.size() - 1);
        if (!actors[r]->canBeImpacted()) return actors[r]->getX()*1000 + actors[r]->getY();
    }
}

void StudentWorld::createVortex(Player* p) {  // creates a vortex for a player when signaled to
    p->setHasVortex(false);
    actors.push_back(new Vortex(p->getX(), p->getY(), p->getWalkDir(), this));
}

Actor* StudentWorld::overlapsHelper(int x, int y, int dir) { // determines if a Vortex overlaps with a Bowser or Boo
    int adder = 0;
    if (dir == Actor::up || dir == Actor::right) adder = 15;
    for (int i = 0; i < actors.size(); i++) {
        Actor* a = actors[i];
        if (!a->canBeImpacted()) continue;
        if (x + adder >= a->getX() && x + adder < a->getX() + 16 && y + adder >= a->getY() && y + adder < a->getY() + 16) return a;
    }
    return nullptr;
}


int StudentWorld::init()
{
    Board bd;
    char boardNum = getBoardNumber() + '0';
    string board_file = assetPath() + "board0" + boardNum + ".txt";
    Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found) {
        cerr << "Could not find board0" << boardNum << ".txt data file\n";
        exit(1);
    }
    else if (result == Board::load_fail_bad_format) {
        cerr << "Your board was improperly formatted\n";
        exit(1);
    }
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
    }
    m_board = &bd;
    
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            Board::GridEntry ge = bd.getContentsOf(x, y);
            m_grid[y][x] = ge;
            switch(ge) {
                case Board::player: {
                    peach = new Player(IID_PEACH, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, 1);
                    yoshi = new Player(IID_YOSHI, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, 2);
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::bowser: {
                    actors.push_back(new Bowser(x*SPRITE_WIDTH, y*SPRITE_WIDTH, this));
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::boo: {
                    actors.push_back(new Boo(x*SPRITE_WIDTH, y*SPRITE_WIDTH, this));
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::blue_coin_square: {
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::red_coin_square: {
                    actors.push_back(new CoinSquare(IID_RED_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, -3));
                    break;
                }
                case Board::star_square: {
                    actors.push_back(new StarSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::up_dir_square: {
                    actors.push_back(new DirSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, Actor::up));
                    break;
                }
                case Board::down_dir_square: {
                    actors.push_back(new DirSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, Actor::down));
                    break;
                }
                case Board::right_dir_square: {
                    actors.push_back(new DirSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, Actor::right));
                    break;
                }
                case Board::left_dir_square: {
                    actors.push_back(new DirSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this, Actor::left));
                    break;
                }
                case Board::bank_square: {
                    actors.push_back(new BankSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                case Board::event_square: {
                    actors.push_back(new EventSquare(x*SPRITE_WIDTH, y*SPRITE_HEIGHT, this));
                    break;
                }
                default: break;
            }
        }
    }
    
	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        Player* winner;
        if (peach->getStars() > yoshi->getStars()) winner = peach;
        else if (peach->getStars() < yoshi->getStars()) winner = yoshi;
        else if (peach->getCoins() > yoshi->getCoins()) winner = peach;
        else if (peach->getCoins() < yoshi->getCoins()) winner = yoshi;
        else {
            if (randInt(1, 2) == 1) winner = peach;
            else winner = yoshi;
        }

        setFinalScore(winner->getStars(), winner->getCoins());
        return winner == peach ? GWSTATUS_PEACH_WON : GWSTATUS_YOSHI_WON;
    }
    
    peach->doSomething();
    yoshi->doSomething();
    for(int i = 0; i < actors.size(); i++) {
        if (actors[i]->isAlive()) {
            actors[i]->doSomething();
        }
    }
    
    // deleting inalive objects
    for(vector<Actor*>::iterator p = actors.begin(); p != actors.end();) {
        if (!(*p)->isAlive()) {
            delete *p;
            p = actors.erase(p);
        } else p++;
    }
    
    string gameText;
    gameText += "P1 Roll: " + to_string(peach->getRoll()) + " Stars: " + to_string(peach->getStars()) + " $$ " + to_string(peach->getCoins());
    if (peach->hasVortex()) gameText += " VOR ";
    gameText += " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(m_bankBalance) + " | ";
    gameText += "P2 Roll: " + to_string(yoshi->getRoll()) + " Stars: " + to_string(yoshi->getStars()) + " $$ " + to_string(yoshi->getCoins());
    if (yoshi->hasVortex()) gameText += " VOR ";
    setGameStatText(gameText);
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (peach != nullptr) {
        delete peach;
        peach = nullptr;
    }
    if (yoshi != nullptr) {
        delete yoshi;
        yoshi = nullptr;
    }
    for (int i = 0; i < actors.size(); i++) {
        if (actors[i] != nullptr) {
            delete actors[i];
            actors[i] = nullptr;
        }
    }

}
