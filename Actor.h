//
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>

/* Checklist:
 -  move flownBy Projectile function into common function since they're all the same
 - Collision function
 - adjust getHitPoints (snagglegon has 10* not 5*) when adding Snagglegon
 */

class NachenBlaster;
class StudentWorld;

class Actor: public GraphObject {
public:
    Actor(int objID, int x, int y, StudentWorld* sptr, int dir=0, double size=1.0, int depth=0)
    : m_isAlive(true), m_sWorld(sptr), GraphObject(objID, x, y, dir, size, depth) {}
    virtual void doSomething() = 0;
    bool isAlive() const {return m_isAlive;}
    void updateLifeStatus(bool status) {m_isAlive= status;}
    StudentWorld* getWorld() const {return m_sWorld;}
    NachenBlaster* getNBlaster() const;
    bool collisionWithNachenBlaster();
    virtual void sufferDamage() {}
    virtual void changeHitPts() {}
    virtual bool isAlien() const =0;
    virtual bool isProj() const= 0;
    virtual ~Actor() {}
private:
    StudentWorld* m_sWorld;
    bool m_isAlive;
};

class Star: public Actor {
public:
    Star(int x, int y, StudentWorld* sptr, double size= static_cast<double>(randInt(5, 50))/100.0, int objID= IID_STAR ,int dir=0, int depth= 3) : Actor(objID, x, y, sptr, dir, size, depth) {}
    virtual void doSomething();
    bool isAlien() const {return false;}
    bool isProj() const {return false;}
    virtual void moveALittle();
    virtual ~Star() {}
};

class Explosion: public Actor {
public:
    Explosion(int x, int y, StudentWorld* sptr, double size=1.0, int objID= IID_EXPLOSION, int dir=0, int depth= 0) :
    Actor(objID, x, y, sptr, dir, size, depth) {}
    virtual void doSomething();
    bool isAlien() const {return false;}
    bool isProj() const {return false;}
    virtual ~Explosion() {}
};

class NachenBlaster: public Actor {
public:
    NachenBlaster(int x, int y, StudentWorld* sptr, int objID= IID_NACHENBLASTER, int dir=0, double size=1.0, int depth=0)
    : Actor(objID, x, y, sptr, dir, size, depth) {
        hitPoints= 50;
        cabbagePoints= 30;
        TorpedoCount= 10;
    }
    virtual void sufferDamage();
    void changeHitPts(int x) {hitPoints= hitPoints+ x;}
    int getHitPoints() const {return hitPoints;}
    int getCabbagePts() const {return cabbagePoints;}
    int getTorpedoCount() const {return TorpedoCount;}
    void changeTorpedoCount(int x) {TorpedoCount= TorpedoCount+ x;}
    virtual void doSomething();
    bool isAlien() const {return false;}
    bool isProj() const {return false;}
    virtual ~NachenBlaster() {}
private:
    bool fireCabbage();
    bool fireTorpedo();
    int hitPoints, cabbagePoints, TorpedoCount;
};

class Projectile: public Actor {  //Projectile objects
public:
    Projectile(int objID, int x, int y, StudentWorld* sptr, Actor* a, int dir, double size, int depth, int moveBy, bool isProj=true): Actor(objID, x, y, sptr, dir, size, depth), m_moveBy(moveBy), firedBy(a) {}
    virtual void doSomething();
    bool isAlien() const {return false;}
    bool isProj() const {return true;}
    virtual void moveBy() {
        moveTo(getX()+ m_moveBy, getY());
    }
    virtual int damageDone()= 0;
    virtual bool flownOff()= 0;
    Actor* actorThatFired() const {return firedBy;}
    virtual ~Projectile() {}
private:
    virtual void doDifferentiatedStuff()= 0;
    int m_moveBy;
    Actor* firedBy; //Pointer to Actor that fired it
};

class Cabbage: public Projectile {
public:
    Cabbage(int x, int y, StudentWorld* sptr, Actor* a, int objID= IID_CABBAGE, int dir=0, double size= 0.5, int depth= 1, int moveby=8) : Projectile(objID, x, y, sptr, a, dir, size, depth, moveby) {}
    virtual void doDifferentiatedStuff();
    bool flownOff() { return getX()>= VIEW_WIDTH; }
    virtual int damageDone() {return -2;}
    virtual ~Cabbage() {}
private:
    void rotate() {setDirection(50); } //rotate direction by 20 degrees
};

class Turnip: public Projectile {
public:
    Turnip(int x, int y, StudentWorld* sptr, Actor *a, int objID= IID_TURNIP, int dir=0, double size= 0.5, int depth= 1, int moveby=-6) : Projectile(objID, x, y, sptr, a, dir, size, depth, moveby) {}
    virtual void doDifferentiatedStuff();
    virtual bool flownOff() { return getX()<0; }
    virtual int damageDone(){return -2;}
    virtual ~Turnip() {}
private:
    void rotate() {setDirection(50);}
};

class FlatulenceTorpedo: public Projectile {
public:
    FlatulenceTorpedo(int x, int y, StudentWorld* sptr, Actor *a, int objID= IID_TORPEDO, int dir=0, double size= 0.5, int depth= 1, int moveby=-8): Projectile(objID, x, y, sptr, a, dir, size, depth, moveby) {}
    virtual bool flownOff() { return (getX()<0 || getX()>= VIEW_WIDTH); }
    virtual void doDifferentiatedStuff();
    int damageDone() {return -8;}
    virtual ~FlatulenceTorpedo() {}
private:
};

class Alien: public Actor { //Includes NachenBlaster and all Alien Ships
public:
    Alien(int x, int y, StudentWorld* sptr, int objID, int dir, double size, int depth, double speed)
    : travelSpeed(speed) ,hitPoints(5* getStartingHitPoints()), flightPlanLength(0), Actor(objID,  x, y, sptr, dir, size, depth) {}
    void doSomething();
    virtual void possiblyDropGoodie()=0;
    virtual int damageDone()=0;
    virtual int pointsAwardedForDeath()=0;
    bool possiblyFireProjectile();
    virtual bool differentiatedFire()= 0;
    int getStartingHitPoints() const ;
    int getCurrHitPts() const {return hitPoints;}
    std::string getDirection()const {return travelDirection;}
    void changeDirection(std::string s) {travelDirection= s;}
    void setFlightLength(int x) {flightPlanLength= x;}
    double getSpeed() const {return travelSpeed;}
    void setSpeed(double x) {travelSpeed= x;}
    void changeHitPts(int x) {hitPoints= hitPoints+x;}
    bool flownOff() {return getX()<0 ;}
    bool isAlien() const {return true;}
    bool isProj() const {return false;}
    void sufferDamage();
    virtual ~Alien() {}
private:
    int hitPoints;
    double travelSpeed;
    int flightPlanLength;
    std::string travelDirection;
};

class Smallgon: public Alien {
public:
    Smallgon(int x, int y, StudentWorld* sptr, int objID=IID_SMALLGON, int dir=0, double size= 1.5, int depth=1, double speed= 2.0) : Alien(x, y, sptr, objID, dir, size, depth, speed) {}
    virtual int damageDone() {return -5;}
    virtual int pointsAwardedForDeath() {return 250;}
    virtual bool differentiatedFire();
    virtual void possiblyDropGoodie() {}
    virtual ~Smallgon() {}
private:
    
};

class Smoregon: public Alien {
public:
    Smoregon(int x, int y, StudentWorld* sptr, int objID= IID_SMOREGON, int dir=0, double size= 1.5, int depth=1, double speed= 2.0) : Alien(x, y, sptr, objID, dir, size, depth, speed) {}
    void dropGoodie() {}
    virtual bool differentiatedFire();
    virtual void possiblyDropGoodie();
    virtual int damageDone() {return -5;}
    virtual int pointsAwardedForDeath() {return 250;}
    virtual ~Smoregon() {}
private:
};

class Snagglegon: public Alien {
public:
    Snagglegon(int x, int y, StudentWorld* sptr, int objID= IID_SNAGGLEGON, int dir=0, double size= 1.5, int depth=1, double speed= 1.75) : Alien(x, y, sptr, objID, dir, size, depth, speed) {}
    virtual bool differentiatedFire();
    virtual void possiblyDropGoodie();
    virtual int damageDone() {return -15;}
    virtual int pointsAwardedForDeath() {return 1000;}
    virtual ~Snagglegon() {}
private:
};

class Goodie: public Actor {
public:
    Goodie(int x, int y, StudentWorld* sptr, int objID, int dir, double size, int depth)
    : Actor(objID, x, y, sptr, dir, size, depth) {}
    virtual void doSomething();
    virtual void doDifferentiatedStuff()= 0;
    bool flownOff();
    bool isAlien() const {return false;}
    bool isProj() const {return false;}
    virtual ~Goodie() {}
private:
};

class LifeGoodie: public Goodie {
public:
    LifeGoodie(int x, int y, StudentWorld* sptr, int objID= IID_LIFE_GOODIE, int dir=0, double size=0.5, int depth=1) : Goodie(x, y, sptr, objID, dir, size, depth) {}
    virtual void doDifferentiatedStuff();
    virtual ~LifeGoodie() {}
private:
};

class RepairGoodie: public Goodie {
public:
    RepairGoodie(int x, int y, StudentWorld* sptr, int objID= IID_LIFE_GOODIE, int dir=0, double size=0.5, int depth=1): Goodie(x, y, sptr, objID, dir, size, depth) {}
    virtual void doDifferentiatedStuff();
    virtual ~RepairGoodie() {}
private:
};

class TorpedoGoodie: public Goodie {
public:
    TorpedoGoodie(int x, int y, StudentWorld* sptr, int objID= IID_TORPEDO_GOODIE, int dir=0, double size=0.5, int depth=1): Goodie(x, y, sptr, objID, dir, size, depth) {}
    virtual void doDifferentiatedStuff();
    virtual ~TorpedoGoodie() {}
private:
    
    
};







#endif // ACTOR_H_

