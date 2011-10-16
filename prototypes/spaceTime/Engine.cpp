
#include "Engine.h"
#include "math/MathUtils.h"
#include <QTimerEvent>
#include <set>

using namespace SpaceTime;

const int kNameLength = 24;
int AstronomicalBodySizeRange[AstronomicalBody::tNumberOfType][2] = 
//{ {0.5, 10000.0}, {0.001, 800.0}, {100.0, 5000.0}, {2000.0, 60000.0},
//  {100000, 1000000} };

//{ {0.5, 10000.0}, {0.001, 800.0}, {100.0, 5000.0}, {2000.0, 6000.0},
//  {1000, 1000} };

{ {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0},
  {1.0, 1.0} };

int kTimeStep = 15; //en milliseconde
//-----------------------------------------------------------------------------
// --- ENGINE
//-----------------------------------------------------------------------------
Engine::Engine() : 
  QObject(),
  mClients(),
  mAstronomicalBodies(),
  mSortedPositions(),
  mShip(),
  mKeyboard(),
  mMouse(),
  mIsDebugging(true),
  mState(sPaused),
  mTimerId(0)
{
	srand( time(NULL) );
  for(int i = 0; i < kNumKeys; ++i)
  	mKeyboard[i] = false;
}

//-----------------------------------------------------------------------------
Engine::~Engine()
{
  unregisterAllClients();
}

//-----------------------------------------------------------------------------
void Engine::callClients(Client::message iM)
{
	for(unsigned int i = 0; i < mClients.size(); ++i)
  	mClients[i]->call(iM);
}

//-----------------------------------------------------------------------------
void Engine::fromString(const QString& iS)
{}

//-----------------------------------------------------------------------------
void Engine::generateAstronomicalBodies(long long	iNum)
{
	//on clear les anciennes données
  mAstronomicalBodies.clear();
  mAstronomicalBodies.reserve(iNum);
  
  for(long long i = 0; i < iNum; ++i)
  {  	
    int numLetter = max(rand() % kNameLength, 1);
    QString name = generateName(numLetter);
    const int kUniverseRadius = 100;
   	Point3d pos(rand() % kUniverseRadius * (rand() % 2 == 0 ? 1 : -1),
      rand() % kUniverseRadius * (rand() % 2 == 0 ? 1 : -1),
      rand() % kUniverseRadius * (rand() % 2 == 0 ? 1 : -1));
    Matrix4d t;
    t.setTranslation(pos);

		AstronomicalBody ab;
    ab.setType( (AstronomicalBody::type)(rand() % AstronomicalBody::tNumberOfType));
    ab.setName(name);
    ab.setTransformation(t);
    
		int radius = rand();
    if(radius > AstronomicalBodySizeRange[ab.getType()][0])
    	radius = AstronomicalBodySizeRange[ab.getType()][0] +
        radius % AstronomicalBodySizeRange[ab.getType()][1];
    ab.setRadius(radius);
    ab.setMass(ab.getRadius());
		mAstronomicalBodies.push_back(ab);
    mSortedPositions.insert(make_pair(pos, i));
  }
  
  if(isDebugging())
  {
  	printf("%d corps astronomique(s)\n", mAstronomicalBodies.size());
//  	printf("//--- Astronomical bodies\n");
//  	for(uint i = 0; i < mAstronomicalBodies.size(); ++i)
//    {
//      const AstronomicalBody& ab = mAstronomicalBodies[i];
//      printf("name: %s, pos: %2.2f, %2.2f, %2.2f\n", 
//        ab.getName().toAscii().constData(), 
//        ab.getTransformation().getTranslation().getX(), ab.getTransformation().getTranslation().getY(), ab.getTransformation().getTranslation().getZ() );  
//    }  	 
//    printf("\n\n//--- Astronomical bodies position triee\n");
//    map<Point3d, long long>::iterator it = mSortedPositions.begin();
//    for(; it != mSortedPositions.end(); ++it)
//    {
//    	const AstronomicalBody& cab = mAstronomicalBodies[it->second];
//    	printf("pos: %2.2f, %2.2f, %2.2f\n", cab.getTransformation().getTranslation().getX(),
//      	cab.getTransformation().getTranslation().getY(), cab.getTransformation().getTranslation().getZ());
//    }
  }
}

//-----------------------------------------------------------------------------
QString Engine::generateName(int iNumLetter) const
{
  static const char alphanum[] =
        "0123456789-"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        
  QString r;
	for(int j = 0; j < iNumLetter; ++j)
  {
    r += alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  
  return r;
}

//-----------------------------------------------------------------------------
const vector<AstronomicalBody>& Engine::getAstronomicalBodies() const
{ return mAstronomicalBodies; }

//-----------------------------------------------------------------------------
vector<AstronomicalBody>
Engine::getAstronomicalBodies(const Point3d& iPos, double iRadius) const
{
	return getAstronomicalBodies(iPos, 0, iRadius);
}

//-----------------------------------------------------------------------------
vector<AstronomicalBody>
Engine::getAstronomicalBodies(const Point3d& iPos, double iInnerRadius,
                              double iOuterRadius) const
{
  vector<AstronomicalBody> r;
  iOuterRadius = max(iOuterRadius, 0.0);
  iInnerRadius = max(iInnerRadius, 0.0);
  
  //early out
  if(iOuterRadius <= iInnerRadius)
  	return r;
  
  set<long long> found;
  multimap<Point3d, long long>::const_iterator itOuterLeft, itOuterRight;
  Point3d outerLeft = iPos - Point3d(iOuterRadius, 0, 0);
  Point3d outerRight = iPos + Point3d(iOuterRadius, 0, 0);
  itOuterLeft = mSortedPositions.lower_bound(outerLeft);
  itOuterRight = mSortedPositions.lower_bound(outerRight);
  
  while(itOuterLeft != itOuterRight)
  {
  	const Point3d& p = mAstronomicalBodies[itOuterLeft->second].getTransformation().getTranslation();
    if(p.fastDist(iPos) < iOuterRadius && p.fastDist(iPos) >= iInnerRadius)
	    r.push_back(mAstronomicalBodies[itOuterLeft->second]);
  	itOuterLeft++;
  }

  return r;
}
//-----------------------------------------------------------------------------
void Engine::goToState(state iS)
{
	switch (getState()) 
  {
  	case sPaused:
    {
      switch (iS) 
      {
        case sPlaying:
          mState = sPlaying;
          callClients(Client::mPlaying);
        	mTimerId = startTimer(kTimeStep);
        break;
        case sGenerating: break;
        default: break;
      }
    } 
    break;
    
    case sPlaying:
    {
      switch (iS) 
      {
        case sPaused:
        	killTimer(mTimerId);
          mTimerId = 0;
          mState = sPaused;
          callClients(Client::mPaused);
        break;
        case sGenerating:
          killTimer(mTimerId);
          mTimerId = 0;
          mState = sGenerating;
        break;
        default: break;
      }
    } 
    break;
    
    case sGenerating:
    {
      switch (iS) 
      {
        case sPaused:
        	killTimer(mTimerId);
          mTimerId = 0;
          mState = sPaused;
          callClients(Client::mPaused);
        break;
        case sPlaying:
          mTimerId = startTimer(kTimeStep);
          mState = sPlaying;
          callClients(Client::mPlaying);
        break;
        default: break;
      }      
    } 
    break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Engine::handleUserInput()
{  
	//handle keyboard
  //double a = 1.00; //km / s2 -> acceleration galactique!
  double a = 10.00; //km / s2 -> acceleration galactique!
  double roll = 0.0; //angle du roll en radian
	Vector3d accel(0.0);
	if(mKeyboard[kW])
	  accel += Vector3d(0.0, 0.0, -a);
  if(mKeyboard[kA])
    accel += Vector3d(-a, 0.0, 0.0);
  if(mKeyboard[kS])
    accel += Vector3d(0.0, 0.0, a);
  if(mKeyboard[kD])
    accel += Vector3d(a, 0.0, 0.0);
  if(mKeyboard[kQ])
    roll = 1.0;
  if(mKeyboard[kE])
  	roll = -1.0;
    
  mShip.setRoll(roll);
  mShip.setAcceleration(accel);
  printf("\naccel from User input: %f, %f, %f\n", accel.getX(),
    accel.getY(), accel.getZ() );
  
  //handle mouse
  /*800 pixel = 2pi radian*/
  Matrix4d m = mShip.getTransformation();
  m = rotate(mShip.getTransformation(), -mMouse.mDeltaY * DEUX_PI / 800.0,
    m.getBaseX(), mShip.getTransformation().getTranslation());
  m = rotate(m, -mMouse.mDeltaX * DEUX_PI / 500.0,
    m.getBaseY(), m.getTranslation());
  mShip.setTransformation(m);
  
  mMouse.mDeltaX = 0;
  mMouse.mDeltaY = 0;
}

//-----------------------------------------------------------------------------
void Engine::mouseMoved(int iX, int iY)
{
//printf("mouse moved: %d, %d; delta: %d, %d\n", iX, iY, mMouse.mDeltaX, mMouse.mDeltaY);
  if(mMouse.mPos.x() > 0 && mMouse.mPos.y() > 0)
  {
  	mMouse.mDeltaX += iX - mMouse.mPos.x();
  	mMouse.mDeltaY += iY - mMouse.mPos.y();
  }
  mMouse.mPos = QPoint(iX, iY);
}

//-----------------------------------------------------------------------------
/*Cette fonction compense le delta de la souris pour un mouvement qu'on
  veut ignorer. Par exemple, quand on replace la souris au centre de 
  l'écran, on ne veut pas en tenir compte, donc on utilise cette fonction.*/
void Engine::ignoreMouseMove(QPoint iP)
{
	if(mMouse.mPos.x() > 0 && mMouse.mPos.y() > 0)
  {
//printf("delta compensation: %d, %d\n", iP.x() - mMouse.mPos.x(),
//    iP.y() - mMouse.mPos.y());
    mMouse.mDeltaX -= iP.x() - mMouse.mPos.x();
    mMouse.mDeltaY -= iP.y() - mMouse.mPos.y();
  }
}

//-----------------------------------------------------------------------------
void Engine::keyPressed(key iKey)
{ mKeyboard[iKey] = true;}

//-----------------------------------------------------------------------------
void Engine::keyReleased(key iKey)
{ mKeyboard[iKey] = false;}

//-----------------------------------------------------------------------------
void Engine::registerClient(Client* iC)
{
	if(iC)
  {
		mClients.push_back(iC);
    iC->call(Client::mRegistered);
  }
}

//-----------------------------------------------------------------------------
void Engine::step(int iMs)
{
  const int method = 1;
  
	//La vitesse et position du vaisseau en fonction de son acceleration
  Vector3d a = mShip.getAcceleration() * mShip.getTransformation();
  Vector3d s = mShip.getSpeed();
  Point3d p;
  Matrix4d m = mShip.getTransformation();
  double t = (iMs / 1000.0); //en sec
  printf("speed avant friction: %f, %f, %f\n", s.getX(), s.getY(), s.getZ() );
  
  switch (method) 
  {
    case 1:
    {
      //on soustrait la friction statique de l'espace!
      //on ralentit de x% du speed quand on n'accellere pas
      if(s.fastNorm() >= 0.001)
      {
        Vector3d sInv = -s;
        sInv.normalise();
        sInv = sInv * 0.05 * s.fastNorm();
        s += sInv;
      }
      printf("speed apres friction: %f, %f, %f\n", s.getX(), s.getY(), s.getZ() );
      printf("speed norm %f\n", s.fastNorm());

      s += a * t; //en km / s  
      p = mShip.getTransformation().getTranslation();
		  p += toPoint(s * t + 0.5 * a * t * t);
      
      double rollAngle = mShip.getRoll() * PI * t;
      Vector3d zAxis = Vector3d(0, 0, 1) * mShip.getTransformation();
      Point3d axisPos = mShip.getTransformation().getTranslation();
      m = rotate(m, rollAngle, zAxis, axisPos);
    }
      break;
    case 2:
    {
    	Vector3d dir = Vector3d(0.0, 0.0, -1.0) * mShip.getTransformation();
      dir.normalise();
      
      if(s.fastNorm() >= 0.001)
      {
        Vector3d sInv = -s;
        sInv.normalise();
        sInv = sInv * 0.05 * s.fastNorm();
        s += sInv;
      }
      
      s += a * t; //en km / s
      double speed = s.fastNorm();
      p = mShip.getTransformation().getTranslation();
		  p += toPoint(dir * speed * t + 0.5 * a * t * t);
    }
    break;
    default:
      break;
  }
  
  
  mShip.setSpeed(s);
  m.setTranslation(p);
  mShip.setTransformation(m);
  
  printf("Speed: %f, %f, %f\n", s.getX(), s.getY(), s.getZ());
  printf("accel: %f, %f, %f\n", a.getX(), a.getY(), a.getZ() );
  printf("pos: %f, %f, %f\n", p.getX(), p.getY(), p.getZ() );
}

//-----------------------------------------------------------------------------
void Engine::timerEvent(QTimerEvent* ipE)
{
	if(ipE->timerId() != mTimerId)
    return;
    
  handleUserInput();
  step(kTimeStep);
	callClients(Client::mFrameReady);
}

//-----------------------------------------------------------------------------
QString Engine::toString() const
{ return QString(); }

//-----------------------------------------------------------------------------
void Engine::unregisterClient(Client* iC)
{
	if(iC && find(mClients.begin(), mClients.end(), iC) != mClients.end())
  {
  	iC->call(Client::mUnregistered);
    mClients.erase(
      remove(mClients.begin(), mClients.end(), iC),
      mClients.end() );
  }
}

//-----------------------------------------------------------------------------
void Engine::unregisterAllClients()
{
	callClients(Client::mUnregistered);
	mClients.clear();
}