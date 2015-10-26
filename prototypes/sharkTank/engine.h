#ifndef SHARKTANK_ENGINE_HH 
#define SHARKTANK_ENGINE_HH

#include "coreLibrary, glSceneGraph.h"
#include "coreLibrary, types.h"
#include <QObject>
#include <QTimerEvent>
#include <vector>

namespace sharkTank{class engine;}

namespace sharkTank
{
using namespace resonant;
   using namespace coreLibrary;

//-----------------------------------------------------------------------------
//--- actor
//-----------------------------------------------------------------------------
class actor
{
public:
   actor(engine& e): mMaxAccel(100), mMaxVelocity(100), mMarkedForDelete(false),
      mEngine(e) {}
   virtual ~actor() {}

   vector3 getAcceleration() const {return mAcceleration;}
   vector3 getAppliedForce() const {return mAppliedForce; }
   double getMaxAccel() const {return mMaxAccel;}
   double getMaxVelocity() const {return mMaxVelocity;}
   matrix4 getOrientation() const {return mOrientation;}
   point3 getPosition() const{return mPosition;}
   vector3 getVelocity() const {return mVelocity;}
   bool isMarkedForDeletion() const {return mMarkedForDelete;}
   void markForDeletion() {mMarkedForDelete = true;}
   void setAcceleration(const vector3& a){ mAcceleration= a; }
   void setAppliedForce(vector3 f) {mAppliedForce = f;}
   void setMaxAccel(double a) {mMaxAccel = a;}
   void setMaxVelocity(double v) {mMaxVelocity = v;}
   void setOrientation(matrix4 o) {mOrientation = o;}
   void setPosition(const point3& p){ mPosition = p; }
   void setVelocity(const vector3& v){ mVelocity = v; }
   virtual void update(){}

protected:
   vector3 mAppliedForce;
   point3 mPosition;
   vector3 mVelocity; // m / sec
   vector3 mAcceleration; //m^2 / sec
   matrix4 mOrientation;
   double mMaxAccel;
   double mMaxVelocity;
   bool mMarkedForDelete;

   engine& mEngine;
};
//-----------------------------------------------------------------------------
//--- fish food
//-----------------------------------------------------------------------------
class fishFood : public actor
{
public:
   fishFood(engine& e) : actor(e) {}
   virtual ~fishFood() {}

   virtual void update();
protected:
   //double mCountdown;
};
//-----------------------------------------------------------------------------
//--- fish
//-----------------------------------------------------------------------------
class fish : public actor
{
public:
   explicit fish(engine&);
   virtual ~fish() {}

   vector3 getFoodForce() const {return mFoodForce;}
   vector3 getGroupingForce() const {return mGroupingForce;}
   vector3 getSeparationForce() const {return mSeparationForce;}
   double getSearchNeighbourRadius() const {return mSearchNeighbourRadius;}
   void setSearchNeighbourRadius(double r) {mSearchNeighbourRadius = r;}
   void setFoodForce(vector3 f) {mFoodForce = f;}
   void setGroupingForce(vector3 f) {mGroupingForce = f;}
   void setSeparationForce(vector3 f) {mSeparationForce = f;}
   virtual void update();

protected:
   vector3 mFoodForce;
   vector3 mGroupingForce;
   vector3 mSeparationForce;
   vector3 mEvadeForce;

   double mSearchNeighbourRadius;   

   fish();
};
//-----------------------------------------------------------------------------
//--- shark
//-----------------------------------------------------------------------------
class shark : public actor
{
public:   
   explicit shark(engine& e);
   virtual ~shark() {}

   vector3 getEatingForce() const {return mEatingForce;}
   double getFovAngle() const {return mFovAngle;}
   double getSearchNeighbourRadius() const {return mSearchNeighbourRadius;}
   void setFovAngle(double a) {mFovAngle = a;}
   void setSearchNeighbourRadius(double r) {mSearchNeighbourRadius = r;}

   virtual void update();
protected:
   shark();

   double mSearchNeighbourRadius;
   double mFovAngle;
   vector3 mEatingForce;
   vector3 mWanderingForce;
   point3 mCircleAround;
};

//-----------------------------------------------------------------------------
//--- fishFoodNode
//-----------------------------------------------------------------------------
class fishFoodNode : public resonant::coreLibrary::drawableNode
{
public:
   fishFoodNode( QString t, fishFood* ff) : drawableNode(t), mpFishFood(ff) {}
   virtual ~fishFoodNode() {}

protected:
   virtual void begin();

   fishFood* mpFishFood;
};

//-----------------------------------------------------------------------------
//--- fishNode
//-----------------------------------------------------------------------------
class fishNode : public resonant::coreLibrary::drawableNode
{
public:
   fishNode( QString, fish* );
   virtual ~fishNode();

protected:
   virtual void begin();

   fish* mpFish;
   static GLuint mDisplayList;
};

//-----------------------------------------------------------------------------
//--- sharkNode
//-----------------------------------------------------------------------------
class sharkNode : public resonant::coreLibrary::drawableNode
{
public:
   sharkNode( QString, shark* );
   virtual ~sharkNode();

protected:
   virtual void begin();

   shark* mpShark;
   static GLuint mDisplayList;
};

//-----------------------------------------------------------------------------
//--- engine
//-----------------------------------------------------------------------------
class engine : QObject
{
   Q_OBJECT
public:
   engine();
   virtual ~engine();

   enum message{ mFrameReady, mStateChanged, mDestroyingEngine};
   enum state{ sStarted, sPaused, sStopped };

   class listener
   {
      public:
         listener();
         virtual ~listener(){}

      virtual void received(message) = 0;
   };

   void addShark();
   void addFood();
   void addListener(listener*);
   fishFood* getClosestFood(point3);
   int getFishMinimalNumberOfNeighbourToFlock() const {return mFishMinimalNumberOfNeighbourToFlock;}
   double getFishMinimalSeparationDistance() const {return mFishMinimalSeparationDistance;}
   int getElapsedTime() const;
   std::vector<fish*>& getFishes() {return mFishes;}
   sceneGraph& getSceneGraph() {return mSceneGraph;}
   std::vector<shark*>& getSharks() {return mSharks;}
   state getState() const { return mState; }
   void nudgeFish();
   void pause();
   void remove(fish*);
   void remove(fishFood*);
   void removeListener(listener*);
   void setFishMinimalNumberOfNeighbourToFlock(int n) {mFishMinimalNumberOfNeighbourToFlock = n;}
   void setFishMinimalSeparationDistance(double d) {mFishMinimalSeparationDistance = d;}   
   void start();
   void stop();
   void updatePhysics(actor*);

protected:
   void goToState(state);
   void initialize();

   void send(message);
   void timerEvent(QTimerEvent*);

   state mState;
   std::vector<listener*> mListeners;
   int mTimerId;
   QTime mTime;
   //flock mFlock;
   std::vector<fish*> mFishes;
   std::vector<fishFood*> mFishFood;
   std::vector<shark*> mSharks;
   double mFishMinimalSeparationDistance;
   int mFishMinimalNumberOfNeighbourToFlock;

   sceneGraph mSceneGraph;
};

}

#endif