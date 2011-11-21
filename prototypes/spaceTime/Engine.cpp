
#include "Engine.h"
#include "math/MathUtils.h"
#include <omp.h>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QTimerEvent>
#include <set>

using namespace SpaceTime;

const int kNameLength = 24;
int AstronomicalBodySizeRange[AstronomicalBody::tNumberOfType][2] = 
{ {0.5, 10000.0}, {0.001, 800.0}, {100.0, 5000.0}, {2000.0, 60000.0},
  {100000, 1000000} };

//{ {0.5, 10000.0}, {0.001, 800.0}, {100.0, 5000.0}, {2000.0, 6000.0},
//  {1000, 1000} };

//{ {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0},
//  {1.0, 1.0} };

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
  mTimerId(0),
  mCycles(0),
  mRadiusOfGeneration(100),
  mErrors()
{
	srand( time(NULL) );
  for(int i = 0; i < kNumKeys; ++i)
  	mKeyboard[i] = false;
}

//-----------------------------------------------------------------------------
Engine::~Engine()
{
  unregisterAllClients();
  deleteAllAstronomicalBodies();
}
//-----------------------------------------------------------------------------
void Engine::addAstronomicalBody(AstronomicalBody* ipAb, long long iIndex)
{
  mAstronomicalBodies.push_back(ipAb);
  mSortedPositions.insert(make_pair(ipAb->getTransformation().getTranslation(), iIndex));
}

//-----------------------------------------------------------------------------
void Engine::addError(const QString& iE)
{
	if(!iE.isNull() && !iE.isEmpty())
    mErrors += "\n" + iE;
}

//-----------------------------------------------------------------------------
void Engine::callClients(Client::message iM)
{
	for(unsigned int i = 0; i < mClients.size(); ++i)
  	mClients[i]->call(iM);
}

//-----------------------------------------------------------------------------
void Engine::deleteAllAstronomicalBodies()
{
	for(unsigned int i = 0; i < mAstronomicalBodies.size(); ++i)
  	delete mAstronomicalBodies[i];
  mAstronomicalBodies.clear();
  mSortedPositions.clear();
}

//-----------------------------------------------------------------------------
void Engine::fromBinary(const QByteArray& iB)
{
	deleteAllAstronomicalBodies();

	QDataStream ds(iB);
	
  qint32 readableVersion = 1;
  qint32 version;
  ds >> version;
  if(version != readableVersion)
  {
  	addError("Unable to read file version" + QString::number(version));
    return;
  }
  
  //on entre les corps astronomiques
  qlonglong numBodies;
  ds >> numBodies;
  
  QString name;
  double accelX, accelY, accelZ;
  double mass;
  double forceX, forceY, forceZ;
  double speedX, speedY, speedZ;
  double t00, t01, t02, t03;
  double t10, t11, t12, t13;
  double t20, t21, t22, t23;
  double t30, t31, t32, t33;
  qint32 type;
  double radius;
  for(long long i = 0; i < numBodies; ++i)
  {
    ds >> name >>
    accelX >> accelY >> accelZ >>
    mass >>
    forceX >> forceY >> forceZ >>
    speedX >> speedY >> speedZ >>
    t00 >> t01 >> t02 >> t03 >>
    t10 >> t11 >> t12 >> t13 >>
    t20 >> t21 >> t22 >> t23 >>
    t30 >> t31 >> t32 >> t33 >>
    type >> radius;
   
    AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setName(name);
    pAb->setAcceleration(Vector3d(accelX, accelY, accelZ));
    pAb->setMass(mass);
    pAb->setForce(Vector3d(forceX, forceY, forceZ));
    pAb->setSpeed(Vector3d(speedX, speedY, speedZ));
    Matrix4d m(t00, t01, t02, t03,
      t10, t11, t12, t13,
      t20, t21, t22, t23,
      t30, t31, t32, t33);
    pAb->setTransformation(m);
    pAb->setType( (AstronomicalBody::type)type );
    pAb->setRadius(radius);
    addAstronomicalBody(pAb, i);
  }
	
  //on entre les donne de l'engin
	ds >> mCycles >>
    mRadiusOfGeneration;
}

//-----------------------------------------------------------------------------
void Engine::generateAstronomicalBodies(long long	iNum)
{
	//on clear les anciennes données
  mCycles = 0;
  deleteAllAstronomicalBodies();
  mAstronomicalBodies.reserve(iNum);
	
  for(long long i = 0; i < iNum; ++i)
  {  	
    int numLetter = max(rand() % kNameLength, 1);
    QString name = generateName(numLetter);
   	Point3d pos(rand() % getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      rand() % getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      rand() % getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1));
    Matrix4d t;
    t.setTranslation(pos);
    int maxAccel = 25;
    Vector3d accel(rand() % maxAccel * (rand() % 2 == 0 ? 1 : -1),
        rand() % maxAccel * (rand() % 2 == 0 ? 1 : -1),
        rand() % maxAccel * (rand() % 2 == 0 ? 1 : -1) );

		AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setType( (AstronomicalBody::type)(rand() % AstronomicalBody::tNumberOfType));
    pAb->setName(name);
    pAb->setTransformation(t);
//    pAb->setAcceleration(accel);
    int radius = rand();
    if(radius > AstronomicalBodySizeRange[pAb->getType()][0])
    	radius = AstronomicalBodySizeRange[pAb->getType()][0] +
        radius % AstronomicalBodySizeRange[pAb->getType()][1];
    pAb->setRadius(1);
    double volume = 4 * PI * radius * radius * radius / 3;
    /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
    //pAb->setMass(volume * 2.70e+5);
    //pAb->setMass(volume);
    pAb->setMass(100000000);
    //pAb->setForce( pAb->getMass() * accel );
    
    addAstronomicalBody(pAb, i);
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
void Engine::generateTestBodies()
{
	//on clear les anciennes données
  mCycles = 0;
  deleteAllAstronomicalBodies();
	
  Matrix4d t;
  
  {
  AstronomicalBody* pAb = new AstronomicalBody();
  pAb->setType(AstronomicalBody::tPlanet);
  pAb->setName(generateName(5));
  t.setTranslation(Point3d(-10, 0, 20));
  pAb->setTransformation(t);
  pAb->setRadius(1);
  pAb->setMass(10000000000);
  addAstronomicalBody(pAb, 0);
  }
  
  {
  AstronomicalBody* pAb = new AstronomicalBody();
  pAb->setType(AstronomicalBody::tPlanet);
  pAb->setName(generateName(5));
  t.setTranslation(Point3d(10, 0, 20));
  pAb->setTransformation(t);
  pAb->setRadius(1);
  pAb->setMass(10000000000);
  addAstronomicalBody(pAb, 1);
  }
}

//-----------------------------------------------------------------------------
QString Engine::getAndClearLastErrors() const
{
	QString r = mErrors;
  mErrors.clear();
  return r;
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
const vector<AstronomicalBody*>& Engine::getAstronomicalBodies() const
{ return mAstronomicalBodies; }

//-----------------------------------------------------------------------------
const vector<AstronomicalBody*>
Engine::getAstronomicalBodies(const Point3d& iPos, double iRadius) const
{
	return getAstronomicalBodies(iPos, 0, iRadius);
}

//-----------------------------------------------------------------------------
const vector<AstronomicalBody*>
Engine::getAstronomicalBodies(const Point3d& iPos, double iInnerRadius,
                              double iOuterRadius) const
{
  vector<AstronomicalBody*> r;
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
  
  /*Todo: Aulieu de faire un fastDist, ce qui ajouter tous les points a
    l'interieur de la sphere, on devrait faire un methode qui retourne
    si le point est a l'intérieur du cube. Ainsi, le probleme de visualisation
    ou les corps disparaissent de la premiere region mais n'apparaisent pas dans
    la region suivante parce qu'ils sont entre la shere du niveau n et le cube
    du niveau n. C'est pour cela qu'ils n'apparaisent pas dans le niveau n + 1.
    Ils sont encore dans le niveau n, mais le test 'fastDist' les écarte.*/
  while(itOuterLeft != itOuterRight)
  {
  	const Point3d& p = mAstronomicalBodies[itOuterLeft->second]->getTransformation().getTranslation();
    double diameter = 2.0 * mAstronomicalBodies[itOuterLeft->second]->getRadius();
    if(p.fastDist(iPos) < (iOuterRadius + diameter) && p.fastDist(iPos) >= iInnerRadius - diameter)
	    r.push_back(mAstronomicalBodies[itOuterLeft->second]);
  	itOuterLeft++;
  }

  return r;
}
//-----------------------------------------------------------------------------
void Engine::generatingStep(int iMs)
{
	QTime timer;
  timer.start();

	/*On calcule la somme des forces que chaque corps astronomiques
    appliquent sur les autres.
    
    http://en.wikipedia.org/wiki/Gravitational_constant
    
    F = G * (m1 * m2) / (r*r)
    G = 6.674 x 10^-11 N(m/Kg)^2
    
    */
    
  const AstronomicalBody* ab1;
  AstronomicalBody* ab2;
  Point3d pos1, pos2;
  double G = 6.674e-11;
	for(unsigned int i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	ab1 = mAstronomicalBodies[i];
  	for(unsigned int j = 0; j < mAstronomicalBodies.size(); ++j)    
  	{
    	if(i == j)
      	continue;
      ab2 = mAstronomicalBodies[j];
      
      pos1 = ab1->getTransformation().getTranslation();
      pos2 = ab2->getTransformation().getTranslation();
      double dist = pos1.fastDist(pos2) * 1000.0; //en met la distance en metre
      dist *= dist;
      dist = max(0.1, dist);
      
      double F = G * (ab1->getMass() * ab2->getMass()) / dist;
      Vector3d vf = toVector(pos1 - pos2);
      vf.normalise();
      vf *= F;
      ab2->setForce( ab2->getForce() + vf );
      
      std::string name = ab2->getName().toStdString();
      printf("\nname: %s\n", name.c_str());
      printf("Force ajoutee: %f\n", vf.norm());
      printf("froce residuelle: %f\n", ab2->getForce().norm());
    }
  }

	for(unsigned int i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	AstronomicalBody& ab = *mAstronomicalBodies[i];
  	//La vitesse et position en fonction de lacceleration
    ab.setAcceleration( ab.getForce() / ab.getMass() );
  	Vector3d a = ab.getAcceleration() * ab.getTransformation();
	  Vector3d s = ab.getSpeed();
    Point3d p;
    Matrix4d m = ab.getTransformation();
    double t = (iMs / 1000.0); //en sec
    
    s += a * t; //en km / s  
    p = ab.getTransformation().getTranslation();
    p += toPoint(s * t + 0.5 * a * t * t);
    
    /*On applique les limites du monde*/
		if(p.getX() > 100)
    	p.setX(-100);
    if(p.getX() < -100)
    	p.setX(100);
    
    if(p.getY() > 100)
    	p.setY(-100);
    if(p.getY() < -100)
    	p.setY(100);
      
    if(p.getZ() > 100)
    	p.setZ(-100);
    if(p.getZ() < -100)
    	p.setZ(100);
      
    ab.setSpeed(s);
    m.setTranslation(p);
    ab.setTransformation(m);
  }
  
  printf("temps pour un cycle: %d ms\n", timer.elapsed());
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
          killTimer(mTimerId);
        	mTimerId = startTimer(kTimeStep);
        break;
        case sGenerating:
          killTimer(mTimerId);
          mState = sGenerating;
        	mTimerId = startTimer(kTimeStep);
        break;
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
          mState = sGenerating;        
        	mTimerId = startTimer(kTimeStep);
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
          killTimer(mTimerId);
          mState = sPlaying;
          mTimerId = startTimer(kTimeStep);          
          callClients(Client::mPlaying);
        break;
        case sGenerating: default: break;
      }      
    } 
    break;
    default: break;
  }
  callClients(Client::mStateChanged);
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
//printf("\naccel from User input: %f, %f, %f\n", accel.getX(),
//  accel.getY(), accel.getZ() );
  
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
void Engine::playingStep(int iMs)
{
  const int method = 1;
  
	//La vitesse et position du vaisseau en fonction de son acceleration
  Vector3d a = mShip.getAcceleration() * mShip.getTransformation();
  Vector3d s = mShip.getSpeed();
  Point3d p;
  Matrix4d m = mShip.getTransformation();
  double t = (iMs / 1000.0); //en sec
//printf("speed avant friction: %f, %f, %f\n", s.getX(), s.getY(), s.getZ() );
  
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
//printf("speed apres friction: %f, %f, %f\n", s.getX(), s.getY(), s.getZ() );
//printf("speed norm %f\n", s.fastNorm());

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
  
//printf("Speed: %f, %f, %f\n", s.getX(), s.getY(), s.getZ());
//printf("accel: %f, %f, %f\n", a.getX(), a.getY(), a.getZ() );
//printf("pos: %f, %f, %f\n", p.getX(), p.getY(), p.getZ() );
}

//-----------------------------------------------------------------------------
void Engine::refreshSortedPositions()
{
	mSortedPositions.clear();
  unsigned int i = 0;
  AstronomicalBody* pAb;
  for(; i < mAstronomicalBodies.size(); ++i)
  {
    pAb = mAstronomicalBodies[i];
    mSortedPositions.insert(make_pair(pAb->getTransformation().getTranslation(), i));
  }
}

//-----------------------------------------------------------------------------
void Engine::timerEvent(QTimerEvent* ipE)
{
	if(ipE->timerId() != mTimerId)
    return;
  
  switch (getState()) 
  {
    case sPlaying:
   		handleUserInput();
      playingStep(kTimeStep);
  	  callClients(Client::mFrameReady);   
      break;
    case sGenerating:
    	handleUserInput();
      generatingStep(5000);
      mCycles++;
      if(getNumberOfCycles() % 5 == 0)
      {
        refreshSortedPositions();
	      callClients(Client::mFrameReady);
      }
    break;
    default: break;
  }  
}

//-----------------------------------------------------------------------------
QByteArray Engine::toBinary() const
{
  QByteArray ba;
  QDataStream ds(&ba, QIODevice::WriteOnly);
  
  qint32 version = 1;
  ds << version;
  
  //on dompe les corps astrnomiques
  qlonglong numBodies = mAstronomicalBodies.size();
  ds << numBodies;
  for(long long i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	const AstronomicalBody* pAb = mAstronomicalBodies[i];
  	ds << pAb->getName() <<
      pAb->getAcceleration().getX() << pAb->getAcceleration().getY() << pAb->getAcceleration().getZ() <<
      pAb->getMass() <<
      pAb->getForce().getX() << pAb->getForce().getY() << pAb->getForce().getZ() <<
      pAb->getSpeed().getX() << pAb->getSpeed().getY() << pAb->getSpeed().getZ() <<
      pAb->getTransformation()(0,0) << pAb->getTransformation()(0,1) << pAb->getTransformation()(0,2) << pAb->getTransformation()(0,3) <<
      pAb->getTransformation()(1,0) << pAb->getTransformation()(1,1) << pAb->getTransformation()(1,2) << pAb->getTransformation()(1,3) <<
      pAb->getTransformation()(2,0) << pAb->getTransformation()(2,1) << pAb->getTransformation()(2,2) << pAb->getTransformation()(2,3) <<
      pAb->getTransformation()(3,0) << pAb->getTransformation()(3,1) << pAb->getTransformation()(3,2) << pAb->getTransformation()(3,3) <<
      (qint32)pAb->getType() << pAb->getRadius();
  }
  
  //on entre les donne de l'engin
  //le ship
  ds << getNumberOfCycles() <<
    getRadiusOfGeneration();
  return ba;
}

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