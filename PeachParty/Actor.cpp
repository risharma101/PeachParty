#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// ACTOR BEGIN
Actor::Actor(int imageID, int startX, int startY, int dir, int depth, double size, StudentWorld* sw) : GraphObject(imageID, startX, startY, dir, depth, size), m_world(sw), m_alive(true), m_canBeImpacted(false), m_pFlag(false), m_yFlag(false) {}
Actor::~Actor(){}
StudentWorld* Actor::getWorld() const {return m_world;}
bool Actor::isAlive() const {return m_alive;}
void Actor::setNotAlive() {m_alive = false;}
bool Actor::canBeImpacted() const {return m_canBeImpacted;}
void Actor::setCanBeImpacted(bool canBeImpacted) {m_canBeImpacted = canBeImpacted;}
void Actor::impacted() {return;}
vector<Player*> Actor::playerLanded() { // checks if a Player (peach or yoshi) has landed on the Actor object
    vector<Player*> v;
    Player* p = getWorld()->getPeach();
    if(p->getState() == WAITING && p->getX() == getX() && p->getY() == getY()) {
        if (!m_pFlag) v.push_back(p); // flags to ensure it doesn't recount a player landing once
        m_pFlag = true;
    } else m_pFlag = false;
    p = getWorld()->getYoshi();
    if(p->getState() == WAITING && p->getX() == getX() && p->getY() == getY()) {
        if (!m_yFlag) v.push_back(p);
        m_yFlag = true;
    } else m_yFlag = false;
    return v;
}

vector<Player*> Actor::playerPassing() { // checks if a Player (peach or yoshi) is passing over the Actor object
    vector<Player*> v;
    Player* p = getWorld()->getPeach();
    if(p->getX() == getX() && p->getY() == getY() && p->getState() == WALKING) v.push_back(p);
    p = getWorld()->getYoshi();
    if(p->getX() == getX() && p->getY() == getY() && p->getState() == WALKING) v.push_back(p);
    return v;
}
// ACTOR END

// AVATAR BEGIN
Avatar::Avatar(int imageID, int startX, int startY, StudentWorld* sw) : Actor(imageID, startX, startY, right, 0, 1, sw), m_state(WAITING), m_direction(right), m_ticks_to_move(0) {}
bool Avatar::getState() const {return m_state;}
void Avatar::setState(bool state) {m_state = state;}
int Avatar::getTicksToMove() const {return m_ticks_to_move;}
void Avatar::setTicksToMove(int ticksToMove) {m_ticks_to_move = ticksToMove;}
int Avatar::getWalkDir() const {return m_direction;}
void Avatar::setWalkDir(int dir) {
    m_direction = dir;
    if (m_direction == left) setDirection(left);
    else setDirection(right);
}

bool Avatar::isNotValidDirection(int direction) { // checks if provided direction is a square for an Avatar (player or enemy) to walk onto
    int newX, newY;
    getPositionInThisDirection(direction, 2, newX, newY);
    int adder = 0;
    if (direction == right || direction == up) adder = SPRITE_WIDTH - 1;
    return newX + adder < 0 || newX + adder >= VIEW_WIDTH || newY + adder < 0 || newY + adder >= VIEW_HEIGHT || getWorld()->getContentsOf((newX+adder)/SPRITE_WIDTH, (newY+adder)/SPRITE_HEIGHT) == Board::empty;
}
bool Avatar::checkAtFork() { // checks if the Avatar is at a fork (i.e. can travel in multiple directions)
    if (getX() % 16 != 0 || getY() % 16 != 0) return false; // if it is not exactly on a square, it will return false
    if ((getWalkDir() == up || getWalkDir() == down)) {
        if (isNotValidDirection(getWalkDir())) return !isNotValidDirection(right) && !isNotValidDirection(left);
        else return !isNotValidDirection(right) || !isNotValidDirection(left);
    }
    else {
        if (isNotValidDirection(getWalkDir())) return !isNotValidDirection(up) && !isNotValidDirection(down);
        else return !isNotValidDirection(up) || !isNotValidDirection(down);
    }
}

int Avatar::randomWalkingDir() { // creates a random valid walking direction for the Avatar
    while (true) {
        int dir = randInt(0, 3) * 90;
        if (!isNotValidDirection(dir)) {
            return dir;
        }
    }
}

void Avatar::teleport(){ // tells StudentWorld class to find a random valid square to teleport the player to, then teleports the player
    int xy = getWorld()->teleportHelper();
    moveTo(xy/1000, xy%1000);
}
// AVATAR END

// PLAYER BEGIN
Player::Player(int imageID, int startX, int startY, StudentWorld* sw, int playerNum) : Avatar(imageID, startX, startY, sw), m_playerNum(playerNum), m_coins(0), m_stars(0), m_hasVortex(false), m_startOfGame(true), m_atDirSquare(false), m_roll(0) {}
int Player::getCoins() const {return m_coins;}
void Player::addCoins(int numCoins) {
    m_coins += numCoins;
    if (m_coins < 0) m_coins = 0;
}
int Player::getStars() const {return m_stars;}
void Player::addStars(int numStars) {m_stars += numStars;}
bool Player::hasVortex() {return m_hasVortex;}
void Player::setHasVortex(bool b) {m_hasVortex = b;}
bool Player::isAtDirSquare() {return m_atDirSquare;}
void Player::setAtDirSquare(bool b) {m_atDirSquare = b;}
int Player::getRoll() {return m_roll;}

bool Player::checkAtFork() {
    if (m_atDirSquare) return false; // returns false if player is on a directional square to override the fork
    return Avatar::checkAtFork();
}

void Player::swapCoins(){
    Player* other;
    if (this == getWorld()->getPeach()) other = getWorld()->getYoshi();
    else other = getWorld()->getPeach();
    int temp = this->getCoins();
    this->addCoins(-1 * temp + other->getCoins());
    other->addCoins(-1 * other->getCoins() + temp);
}
void Player::swapStars(){
    Player* other;
    if (this == getWorld()->getPeach()) other = getWorld()->getYoshi();
    else other = getWorld()->getPeach();
    int temp = this->getStars();
    this->addStars(-1 * temp + other->getStars());
    other->addStars(-1 * other->getStars() + temp);
}
void Player::swapState() {
    Player* other;
    if (this == getWorld()->getPeach()) other = getWorld()->getYoshi();
    else other = getWorld()->getPeach();
    int tempTicks = this->getTicksToMove();
    bool tempState = this->getState();
    int tempDir = this->getWalkDir();
    int tempX = this->getX();
    int tempY = this->getY();
    this->setTicksToMove(other->getTicksToMove());
    this->setState(other->getState());
    this->setWalkDir(other->getWalkDir());
    other->setTicksToMove(tempTicks);
    other->setState(tempState);
    other->setWalkDir(tempDir);
    this->moveTo(other->getX(), other->getY());
    other->moveTo(tempX, tempY);
}

void Player::teleport() {
    Avatar::teleport();
    setWalkDir(-1); // makes walk direction invalid
}

void Player::doSomething() { // TO DO
    if (getWalkDir() == -1) {
        setWalkDir(randomWalkingDir());
    }
    if (getState() == WAITING) {
        m_roll = 0;
        int action = getWorld()->getAction(m_playerNum);
        switch(action) {
            case ACTION_ROLL: {
                m_roll = randInt(1, 10);
                setTicksToMove(m_roll * 8);
                setState(WALKING);
                break;
            }
            case ACTION_FIRE: {
                if (!hasVortex()) return;
                getWorld()->createVortex(this);
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                break;
                
            }
            default: return;
        }
    }
    if (getState() == WALKING) {
        if (!m_startOfGame && !isAtDirSquare() && checkAtFork()) {
            int action = getWorld()->getAction(m_playerNum);
            switch(action) {
                case ACTION_LEFT:
                    if (getWalkDir() != right && !isNotValidDirection(left)) setWalkDir(left);
                    break;
                case ACTION_RIGHT:
                    if (getWalkDir() != left && !isNotValidDirection(right)) setWalkDir(right);
                    break;
                case ACTION_UP:
                    if (getWalkDir() != down && !isNotValidDirection(up)) setWalkDir(up);
                    break;
                case ACTION_DOWN:
                    if (getWalkDir() != up && !isNotValidDirection(down)) setWalkDir(down);
                    break;
                default: return;
            }
        }
        if (isNotValidDirection(getWalkDir())) { // if at corner, it changes the walking direction to the valid direction
            if (getWalkDir() == right || getWalkDir() == left) {
                if (!isNotValidDirection(up)) setWalkDir(up);
                else setWalkDir(down);
            } else {
                if (!isNotValidDirection(right)) setWalkDir(right);
                else setWalkDir(left);
            }
        }
        moveAtAngle(getWalkDir(), 2);
        m_startOfGame = false;
        setTicksToMove(getTicksToMove() - 1);
        m_roll = getTicksToMove() / 8;
        if (getTicksToMove() == 0) setState(WAITING);
    }
    m_atDirSquare = false;
}
// PLAYER END

// ENEMY BEGIN
Enemy::Enemy(int imageID, int startX, int startY, StudentWorld* sw) : Avatar(imageID, startX, startY, sw), m_pauseCounter(180) {setCanBeImpacted(true);}
int Enemy::getPauseCounter() const {return m_pauseCounter;}
void Enemy::setPauseCounter(int pauseVal) {m_pauseCounter = pauseVal;}
bool Enemy::sameSquare(Player* p) {return p->getState() == WAITING && p->getX() == getX() && p->getY() == getY();}

void Enemy::impacted(){
    teleport();
    setWalkDir(right);
    setState(PAUSED);
    setPauseCounter(180);
}

void Enemy::teleport() {
    Avatar::teleport();
    setWalkDir(right);
    setState(PAUSED);
    setPauseCounter(180);
}

void Enemy::enemyPausedCode(int roll) { // what an enemy does in the paused state
    if (getState() == PAUSED) {
        vector<Player*> v = playerLanded();
        for (int i = 0; i < v.size(); i++) {
            Player* p = v[i];
            enemyAction(p); // pure virtual method because different for Bowser and Boo
        }
        setPauseCounter(getPauseCounter() - 1);
        if (getPauseCounter() == 0) {
            setTicksToMove(8 * randInt(1, roll));
            setWalkDir(randomWalkingDir());
            setState(WALKING);
        }
    }
}
    
void Enemy::enemyWalkCode() { // what an enemy does in its walking state
    if (getState() == WALKING) {
        if (checkAtFork()) {
            while (true) {
                int rand = randInt(0, 3) * 90;
                if (rand != (getWalkDir() + 180) % 360 && !isNotValidDirection(rand)) {
                    setWalkDir(rand);
                    break;
                }
            }
        }
        if (isNotValidDirection(getWalkDir())) {
            if (getWalkDir() == right || getWalkDir() == left) {
                if (!isNotValidDirection(up)) setWalkDir(up);
                else setWalkDir(down);
            } else {
                if (!isNotValidDirection(right)) setWalkDir(right);
                else setWalkDir(left);
            }
        }
        moveAtAngle(getWalkDir(), 2);
        setTicksToMove(getTicksToMove() - 1);
        if (getTicksToMove() == 0) {
            setState(PAUSED);
            setPauseCounter(180);
            bowserEndAction(); // pure virtual method because different for Bowser and Boo
        }
    }
}

// ENEMY END

// BOWSER BEGIN
Bowser::Bowser(int startX, int startY, StudentWorld* sw) : Enemy(IID_BOWSER, startX, startY, sw) {}
void Bowser::enemyAction(Player* p) {
    if (randInt(1, 2) == 1) {
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
        p->addCoins(-1 * p->getCoins());
    }
}
void Bowser::bowserEndAction() {
    if (randInt(1,4) == 1) {
        getWorld() -> newDroppingSquare(getX(), getY());
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}
void Bowser::doSomething() {
    enemyPausedCode(10);
    enemyWalkCode();
}
// BOWSER END

// BOO BEGIN
Boo::Boo(int startX, int startY, StudentWorld* sw) : Enemy(IID_BOO, startX, startY, sw) {}
void Boo::enemyAction(Player* p) {
    if (randInt(1, 2) == 1) p->swapCoins();
    else p->swapStars();
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}
void Boo::bowserEndAction() {return;}
void Boo::doSomething() {
    enemyPausedCode(3);
    enemyWalkCode();
}
// BOO END

// VORTEX BEGIN
Vortex::Vortex(int startX, int startY, int dir, StudentWorld* sw) : Actor(IID_VORTEX, startX, startY, dir, 0, 1, sw), m_vortexDir(dir) {setCanBeImpacted(false);}
Actor* Vortex::overlaps() {return getWorld()->overlapsHelper(getX(), getY(), m_vortexDir);}; // tells Student World to check if teh Vortex overlaps with a Bowser or Boo and if so return a pointer to that Enemy

void Vortex::doSomething() { // TO DO
    if(!isAlive()) return;
    moveAtAngle(m_vortexDir, 2);
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT) setNotAlive();
    Actor* p = overlaps();
    
    if (p != nullptr) {
        p->impacted();
        setNotAlive();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}
// VORTEX END


// SQUARE BEGIN
Square::Square(int imageID, int startX, int startY, int dir, int depth, double size, StudentWorld* sw) : Actor(imageID, startX, startY, dir, depth, size, sw) {setCanBeImpacted(false);}

// SQUARE END

// COINSQUARE BEGIN
CoinSquare::CoinSquare(int imageID, int startX, int startY, StudentWorld* sw, int value) : Square(imageID, startX, startY, right, 1, 1, sw), m_value(value){}
    
void CoinSquare::doSomething() {
    vector<Player*> v = playerLanded();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        p->addCoins(m_value);
        if (m_value == 3) getWorld()->playSound(SOUND_GIVE_COIN);
        else getWorld()->playSound(SOUND_TAKE_COIN);
    }
}
// COINSQUARE END

// STARSQUARE BEGIN
StarSquare::StarSquare(int startX, int startY, StudentWorld* sw) : Square(IID_STAR_SQUARE, startX, startY, right, 1, 1, sw) {}

void StarSquare::doSomething(){
    vector<Player*> v = playerPassing();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        if (p == nullptr || p->getCoins() < 20) return;
        
        p->addCoins(-20);
        p->addStars(1);
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
    
    vector<Player*> v2 = playerLanded();
    for (int i = 0; i < v2.size(); i++) {
        Player* p = v2[i];
        if (p == nullptr || p->getCoins() < 20) return;
        
        p->addCoins(-20);
        p->addStars(1);
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
}
// STARSQUARE END

// DIRSQUARE BEGIN
DirSquare::DirSquare(int startX, int startY, StudentWorld* sw, int forcingDir) : Square(IID_DIR_SQUARE, startX, startY, right, 1, 1, sw), m_forcingDir(forcingDir){setDirection(forcingDir);}

void DirSquare::doSomething(){
    
    vector<Player*> v = playerPassing();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        p->setWalkDir(m_forcingDir);
        p->setAtDirSquare(true);
    }
    
    vector<Player*> v2 = playerLanded();
    for (int i = 0; i < v2.size(); i++) {
        Player* p = v2[i];
        p->setWalkDir(m_forcingDir);
        p->setAtDirSquare(true);
    }
}
// DIRSQUARE END

// BANKSQUARE BEGIN
BankSquare::BankSquare(int startX, int startY, StudentWorld* sw) : Square(IID_BANK_SQUARE, startX, startY, right, 1, 1, sw) {}

void BankSquare::doSomething(){
    vector<Player*> v = playerLanded();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        p->addCoins(getWorld()->getBankBalance());
        getWorld()->addBankBalance(-1 * getWorld()->getBankBalance());
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
    }
    
    vector<Player*> v2 = playerPassing();
    for (int i = 0; i < v2.size(); i++) {
        Player* p = v2[i];
        int initCoins = p->getCoins();
        p->addCoins(-5);
        getWorld()->addBankBalance(initCoins - p->getCoins());
        getWorld()->playSound(SOUND_DEPOSIT_BANK);
    }
}
// BANKSQUARE END

// EVENTSQUARE BEGIN
EventSquare::EventSquare(int startX, int startY, StudentWorld* sw) : Square(IID_EVENT_SQUARE, startX, startY, right, 1, 1, sw) {}

void EventSquare::doSomething(){
    vector<Player*> v = playerLanded();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        int rand = randInt(1, 3);
        switch(rand) {
            case 1:
                p->teleport();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 2:
                p->swapState();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 3:
                if (!p->hasVortex()) {
                    p->setHasVortex(true);
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                }
        }
    }
}
// EVENTSQUARE END

// DROPPINGSQUARE BEGIN
DroppingSquare::DroppingSquare(int startX, int startY, StudentWorld* sw) : Square(IID_DROPPING_SQUARE, startX, startY, right, 1, 1, sw) {}

void DroppingSquare::doSomething(){
    vector<Player*> v = playerLanded();
    for (int i = 0; i < v.size(); i++) {
        Player* p = v[i];
        int rand = randInt(1, 2);
        switch(rand) {
            case 1:
                p->addCoins(-10);
                break;
            case 2:
                if (p->getStars() > 0) p->addStars(-1);
                break;
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    }
}
// DROPPINGSQUARE END
