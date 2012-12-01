
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


//les types
//type{tBlackHole, tComet, tMoon, tPlanet, tStar, tNumberOfType};
//en km
int AstronomicalBodyRadiusRange[AstronomicalBody::tNumberOfType][2] = 
{ {5000, 30000}, {1.0, 10.0}, {10.0, 50.0}, {50.0, 800.0},
  {800, 2000} };

//{ {0.5, 10000.0}, {0.001, 800.0}, {100.0, 5000.0}, {2000.0, 6000.0},
//  {1000, 1000} };

//{ {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0},
//  {100000, 10000000} };
  
//en Kg/km3
int AstronomicalBodyDensityRange[AstronomicalBody::tNumberOfType][2] = 
{ {5000, 10000}, {1.0, 10.0}, {10.0, 50.0}, {50.0, 800.0},
  {800, 2000} };
  
//pourcentage (entre 0 et 1)
double AstronomicalOccurence[AstronomicalBody::tNumberOfType] = 
{ 0.005, 0.635, 0.24, 0.08, 0.02 };


int kTimeStep = 15; //en milliseconde
//-----------------------------------------------------------------------------
// --- AstronomicalBody
//-----------------------------------------------------------------------------
void AstronomicalBody::addToPath(const Point3d& iP)
{
	const unsigned int pathSize = 200;
  if (getPath().size() >= pathSize) 
  {
    mPath.pop_front();
  }
  mPath.push_back(iP);
}

//-----------------------------------------------------------------------------
void AstronomicalBody::engage(const AstronomicalBody* ipA)
{ mEngaged.insert(ipA); }

//-----------------------------------------------------------------------------
void AstronomicalBody::disengage(const AstronomicalBody* ipA)
{
	set<const AstronomicalBody*>::iterator it =
  	mEngaged.find(ipA);
  if(it != mEngaged.end())
  	mEngaged.erase(it);
}

//-----------------------------------------------------------------------------
void AstronomicalBody::disengageAll()
{ mEngaged.clear(); }

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
  mSpaceRadius(100000),
  mErrors(),
  mCollisions(),
  mMarkToDelete()
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
void Engine::applyForces()
{
	/*On calcule la somme des forces que chaque corps astronomiques
    appliquent sur les autres.*/
  
  AstronomicalBody* ab1;
  const AstronomicalBody* ab2;
  unsigned int i = 0;
  set<const AstronomicalBody*>::const_iterator it;
  
  for(; i < mAstronomicalBodies.size(); ++i)
  {
  	ab1 = mAstronomicalBodies[i];
    it = ab1->getEngagedBodies().begin();
  	for(; it != ab1->getEngagedBodies().end(); ++it)    
  	{
      ab2 = *it;
      Vector3d vf = computeAttractionForce(ab2, ab1);
      ab1->setForce( ab1->getForce() + vf );
    }
  }
}

//-----------------------------------------------------------------------------
void Engine::callClients(Client::message iM)
{
	for(unsigned int i = 0; i < mClients.size(); ++i)
  	mClients[i]->call(iM);
}

//-----------------------------------------------------------------------------
/*Calcule la force d'attraction appliquée à ipB par ipA*/
Vector3d Engine::computeAttractionForce(const AstronomicalBody* ipA,
                                        const AstronomicalBody* ipB)
{
	/*     
    http://en.wikipedia.org/wiki/Gravitational_constant
    
    F = G * (m1 * m2) / (r*r)
    G = 6.674 x 10^-11 N(m/Kg)^2    */
    
  Point3d pos1, pos2;
  double G = 6.674e-11;
      
  pos1 = ipA->getTransformation().getTranslation();
  pos2 = ipB->getTransformation().getTranslation();
  double dist = pos1.dist(pos2);
    
  dist = dist * 1000.0 * dist * 1000.0; // on met la distance en metre
    //et au carré
  dist = max(0.1, dist); //on cap avec une valeur minimale...

  double F = G * (ipA->getMass() * ipB->getMass()) / dist;
  Vector3d vf = toVector(pos1 - pos2);
  vf.normalise();
  vf *= F;
  return vf;
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
void Engine::deleteMarkedBodies()
{
	/*On commence par enlever toutes les references de cet object des listes
    de corps engagés aux calcul des forces. Ensuite on enleve et delete 
    cet object de la liste maitre (mAstronomicalBodies). */
  unsigned int i;
  set<AstronomicalBody*>::iterator toDeleteIt = mMarkToDelete.begin();
  vector<AstronomicalBody*>::iterator masterIt;
  
  for(; toDeleteIt != mMarkToDelete.end(); ++toDeleteIt)
  {
  	for(i = 0; i < mAstronomicalBodies.size(); ++i)
    	mAstronomicalBodies[i]->disengage(*toDeleteIt);

  	masterIt = find(mAstronomicalBodies.begin(),
    	mAstronomicalBodies.end(), *toDeleteIt);
    if(masterIt != mAstronomicalBodies.end())
    {
	  	mAstronomicalBodies.erase(masterIt);
      delete *toDeleteIt;
    }
    else {
      assert(0 && "Fack! d'ou vien ce corps!"); }
  }
  
  mMarkToDelete.clear();
}

//-----------------------------------------------------------------------------
void Engine::detectCollisions()
{
	mCollisions.clear();
  AstronomicalBody* ab1;
  AstronomicalBody* ab2;
  Point3d pos1, pos2;
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
      double dist = pos1.dist(pos2);
      
      //On ajoute un collision quand la distance entre deux objects
      // est plus petites que la somme de leurs rayons
      if(dist < (ab1->getRadius() + ab2->getRadius()) * 0.95)
      {
        mCollisions.insert(CollisionPair(i, j));
      }
  	}
  }
}

//-----------------------------------------------------------------------------
void Engine::engageBodies()
{
	unsigned int i, j;
  AstronomicalBody *pA, *pB;
	for(i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	pA = mAstronomicalBodies[i];
    pA->disengageAll();
  	for(j = 0; j < mAstronomicalBodies.size(); ++j)
    {
    	if(i == j)
      	continue;
        
      pB = mAstronomicalBodies[j];
      Vector3d vf = computeAttractionForce(pB, pA);
      /*Force necessaire pour accelerer un petite comete
        de density moyenne de 1m/s*/
      double radius = AstronomicalBodyRadiusRange[AstronomicalBody::tComet][0];
      double volume = 4 * PI * radius * radius * radius / 3;
      double threshold = volume * 2.7e+5;
      if(vf.fastNorm() > threshold)
      	pA->engage(pB);
    }
  }
}

//-----------------------------------------------------------------------------
void Engine::explodeAstronomicalBody(AstronomicalBody* ipA)
{
	for(unsigned int i = 0; i < 4; ++i)
  {
    AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setType(ipA->getType());
    
    Vector3d dir( rand(), rand(), rand() );
    dir.normalise();
    double mass = ipA->getMass() / 4.0;
		double volume = mass / ipA->getDensity();    
//  double radius = pow(3 * volume / (4.0 * PI), 1.0/3.0);
double radius = ipA->getRadius() / 4.0;
    Point3d pos = ipA->getTransformation().getTranslation() + 
      dir * normalizedRandom() * ipA->getRadius();
    Matrix4d transfo;
    transfo.setTranslation(pos);
    
    pAb->setName("crashed-" + generateName(5));
    pAb->setTransformation( transfo );
    pAb->setRadius( radius );
    pAb->setMass(mass);
    pAb->setSpeed(ipA->getSpeed());
    pAb->setEngagedBodies(ipA->getEngagedBodies());
    addAstronomicalBody(pAb, mAstronomicalBodies.size() - 1);
  }
}

//-----------------------------------------------------------------------------
void Engine::fonctionBidon()
{
	solveCollisionsOverlap();
}

//-----------------------------------------------------------------------------
void Engine::fromBinary(const QByteArray& iB)
{
	deleteAllAstronomicalBodies();

	QDataStream ds(iB);
	
  qint32 oldestReadable = 1;
  qint32 readableVersion = 2;
  qint32 version;
  ds >> version;
  if(version < oldestReadable || version > readableVersion)
  {
  	addError("Unable to read file version" + QString::number(version));
    return;
  }
  
  switch (version) 
  {
    case 1:
    {
      //on entre les corps astronomiques
      qlonglong numBodies;
      ds >> numBodies;
      
      QString name;
      double accelX, accelY, accelZ;
      /*puisque l'engin prend maintenant la density en compte dans les
        calculs, il faut donner un densité raisonable, sinon la simulation
        n'aura aucun sens...*/
      double density = 2.70e+7; 
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
        pAb->setDensity(density);
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
    } break;
    case 2:
    {
      //on entre les corps astronomiques
      qlonglong numBodies;
      ds >> numBodies;
      
      QString name;
      double accelX, accelY, accelZ;
      double density;
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
        density >>
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
        pAb->setDensity(density);
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
        mRadiusOfGeneration >>
        mSpaceRadius;
    } break;
    default: break;
  }
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
   	Point3d pos(normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1));
    Matrix4d t;
    t.setTranslation(pos);
    
    AstronomicalBody::type type = (AstronomicalBody::type)(rand() % AstronomicalBody::tNumberOfType);
    int radius = rand();
    if(radius > AstronomicalBodyRadiusRange[type][0])
    	radius = AstronomicalBodyRadiusRange[type][0] +
        radius % AstronomicalBodyRadiusRange[type][1];

    double maxSpeed = 30; //km/s
    Vector3d speed( normalizedRandom() * maxSpeed * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * maxSpeed * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * maxSpeed * (rand() % 2 == 0 ? 1 : -1));

		AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setType( type );
    pAb->setName(name);
    pAb->setTransformation(t);
    pAb->setRadius(1);
    double volume = 4 * PI * radius * radius * radius / 3;
    /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
    pAb->setMass(volume * 2.70e+7);
    pAb->setSpeed(speed);
        
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
void Engine::generateTestBodies1()
{
	//on clear les anciennes données
  mCycles = 0;
  deleteAllAstronomicalBodies();
	
  Matrix4d t;
  int radius;
  double volume;
  
  {
  AstronomicalBody* pAb = new AstronomicalBody();
  pAb->setType(AstronomicalBody::tStar);
  
  radius = rand();
  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
  volume = 4 * PI * radius * radius * radius / 3;
  
  pAb->setName(generateName(5));
  t.setTranslation(Point3d(-10, 0, 20));
  pAb->setTransformation(t);
  pAb->setRadius(1);
  /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
  pAb->setDensity(2.70e+8);
  pAb->setMass(volume * pAb->getDensity());
  //pAb->setSpeed(Vector3d(2, 1, 0));
  addAstronomicalBody(pAb, 0);
  }
  
  for( unsigned int i = 0; i < 5; ++i )
  {
    AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setType(AstronomicalBody::tPlanet);
    
    radius = rand();
    if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
      radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
        radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
    volume = 4 * PI * radius * radius * radius / 3;
    
    Point3d pos(normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      normalizedRandom() * getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1));
    Matrix4d t;
    t.setTranslation(pos);
    
    pAb->setName(generateName(5));
    t.setTranslation( pos );
    pAb->setTransformation(t);
    pAb->setRadius(2);
    pAb->setDensity(2.70e+7);
    pAb->setMass(volume * pAb->getDensity());
    pAb->setSpeed(Vector3d(-2, 4, 0));
    addAstronomicalBody(pAb, 1);
  }
  
  //-- autre system
//    {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tStar);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(50, 0, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(1);
//  /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
//      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
//  pAb->setDensity(2.70e+8);
//  pAb->setMass(volume * pAb->getDensity());
//  pAb->setSpeed(Vector3d(-2, -2, 0));
//  addAstronomicalBody(pAb, 2);
//  }
//  
//  {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tPlanet);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(50, 10, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(2);
//  pAb->setDensity(2.70e+7);
//  pAb->setMass(volume * pAb->getDensity());
//  pAb->setSpeed(Vector3d(-2, 1, 0));
//  addAstronomicalBody(pAb, 3);
//  }

}

//-----------------------------------------------------------------------------
//void Engine::generateTestBodies1_nice_orbits()
//{
//	//on clear les anciennes données
//  mCycles = 0;
//  deleteAllAstronomicalBodies();
//	
//  Matrix4d t;
//  int radius;
//  double volume;
//  
//  {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tStar);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(-10, 0, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(1);
//  /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
//      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
//  pAb->setDensity(2.70e+10);
//  pAb->setMass(volume * pAb->getDensity());
//  addAstronomicalBody(pAb, 0);
//  }
//  
//  {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tPlanet);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(10, 10, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(2);
//  pAb->setDensity(2.70e+7);
//  pAb->setMass(volume * pAb->getDensity());
//  pAb->setSpeed(Vector3d(-22, 23, 0));
//  addAstronomicalBody(pAb, 1);
//  }
//  
//  {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tComet);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(-10, -10, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(0.5);
//  pAb->setDensity(2.70e+7);
//  pAb->setMass(volume * pAb->getDensity());
//  pAb->setSpeed(Vector3d(25, -6, 0));
//  addAstronomicalBody(pAb, 2);
//  }
//  
//  {
//  AstronomicalBody* pAb = new AstronomicalBody();
//  pAb->setType(AstronomicalBody::tComet);
//  
//  radius = rand();
//  if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
//    radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
//      radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
//  volume = 4 * PI * radius * radius * radius / 3;
//  
//  pAb->setName(generateName(5));
//  t.setTranslation(Point3d(-10, 28, 20));
//  pAb->setTransformation(t);
//  pAb->setRadius(0.5);
//  pAb->setDensity(2.70e+7);
//  pAb->setMass(volume * pAb->getDensity());
//  pAb->setSpeed(Vector3d(15, 6, 0));
//  addAstronomicalBody(pAb, 3);
//  }
//}

//-----------------------------------------------------------------------------
void Engine::generateTestBodies2(long long iNum)
{
	//on clear les anciennes données
  mCycles = 0;
  deleteAllAstronomicalBodies();
  mAstronomicalBodies.reserve(iNum);
	
  for(long long i = 0; i < iNum; ++i)
  {  	
    AstronomicalBody* pAb = new AstronomicalBody();
    pAb->setType( (AstronomicalBody::type)(rand() % AstronomicalBody::tNumberOfType));
    
    int numLetter = max(rand() % kNameLength, 1);
    QString name = generateName(numLetter);
   	Point3d pos(rand() % getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      rand() % getRadiusOfGeneration() * (rand() % 2 == 0 ? 1 : -1),
      -25000);
    Matrix4d t;
    t.setTranslation(pos);
    //int maxAccel = 25;
    //Vector3d accel(rand() % maxAccel * (rand() % 2 == 0 ? 1 : -1),
    //    rand() % maxAccel * (rand() % 2 == 0 ? 1 : -1),
    //    0 );
    int maxspeed = 5;
    Vector3d speed(rand() % maxspeed * (rand() % 2 == 0 ? 1 : -1),
        rand() % maxspeed * (rand() % 2 == 0 ? 1 : -1),
        0 );
    int radius = rand();
    if(radius > AstronomicalBodyRadiusRange[pAb->getType()][0])
    	radius = AstronomicalBodyRadiusRange[pAb->getType()][0] +
        radius % AstronomicalBodyRadiusRange[pAb->getType()][1];
    double volume = 4 * PI * radius * radius * radius / 3;

    pAb->setName(name);
    pAb->setTransformation(t);
//    pAb->setAcceleration(accel);
		pAb->setSpeed(speed);
    //pAb->setRadius(radius);
    pAb->setRadius(1);    
    /*la densité du fer 7.87 g/cm 3 donc 7.87 e 12 Kg/km3
      de laluminum      2.70 g/cm3  donc 2.70 e 12 kg/km3*/
    pAb->setMass(volume * 2.70e+7);
    addAstronomicalBody(pAb, i);
  }
  
  if(isDebugging())
  {
  	printf("%d corps astronomique(s)\n", mAstronomicalBodies.size());
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
void Engine::simulate(int iMs)
{
//	QTime timer;
//  timer.start();

	if(mCycles % 5 == 0)
  	engageBodies();

	applyForces();
  moveBodies(iMs * 3);
  detectCollisions();
  handleCollisions(iMs * 3);
  deleteMarkedBodies();
  solveCollisionsOverlap();
  
  mCycles++;
  //printf("temps pour un cycle: %d ms\n", timer.elapsed());
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
        case sSimulating:
          killTimer(mTimerId);
          mState = sSimulating;
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
        case sSimulating:
          killTimer(mTimerId);
          mState = sSimulating;        
        	mTimerId = startTimer(kTimeStep);
        break;
        default: break;
      }
    } 
    break;
    
    case sSimulating:
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
        case sSimulating: default: break;
      }      
    } 
    break;
    default: break;
  }
  callClients(Client::mStateChanged);
}

//-----------------------------------------------------------------------------
void Engine::handleCollisions(int iMs)
{
	AstronomicalBody* ab1;
  AstronomicalBody* ab2;
  set<CollisionPair>::iterator it = mCollisions.begin();
  for(; it != mCollisions.end(); ++it)
  {
  	ab1 = mAstronomicalBodies[it->mIndex1];
    ab2 = mAstronomicalBodies[it->mIndex2]; 
     
    double t = (iMs / 1000.0); //en sec
    double m1 = ab1->getMass();
    double m2 = ab2->getMass();
    Vector3d v1 = ab1->getSpeed();
    Vector3d v2 = ab2->getSpeed();
    Point3d pos1 = ab1->getTransformation().getTranslation();
    Point3d pos2 = ab2->getTransformation().getTranslation();
    Vector3d collisionNormal(pos1, pos2);
    collisionNormal.normalise();
    double v1ProjectedOnNormal = collisionNormal & v1;
    double v2ProjectedOnNormal = collisionNormal & v2;

		//on traite ab1
    {
    	if(ab1->getType() == AstronomicalBody::tBlackHole )
      {
        mMarkToDelete.insert(ab1);
      	merge(ab2, ab1);
      }
      else if(ab1->getMass() / ab2->getMass() <= 1e-5)
      {
        mMarkToDelete.insert(ab1); 
        merge(ab1, ab2);
      }
      else 
      {
      	Vector3d tangent = collisionNormal ^ (collisionNormal ^ v1);
        tangent.normalise();
        double v1ProjectedOnTangent = tangent & v1;
        double vScalarNormal = (v1ProjectedOnNormal*(m1 - m2) + 2*m2*v2ProjectedOnNormal) / (m1 + m2);
        Vector3d v1n = collisionNormal * vScalarNormal;
        Vector3d vf = v1n + tangent * v1ProjectedOnTangent;
        
        Vector3d delta = ab1->getSpeed() - vf;
        delta = delta / t;
        double kineticEnergyOfdeceleration = 0.5 * ab1->getMass() *
          delta.norm() * delta.norm(); 
        if(kineticEnergyOfdeceleration > 10000 * ab1->getMass())
        {
          //on marque l'objet pour etre detruit et on ajoute
          //4 nouveau qui on le quart de la masse original
          mMarkToDelete.insert(ab1);   
          ab1->setSpeed(vf * 0.8);                 
          explodeAstronomicalBody(ab1);          
        }
        else 
        {
          ab1->setSpeed(vf);
        }
      }
    }
    
    //on traite ab2
    {
      if(ab2->getType() == AstronomicalBody::tBlackHole )
      {
        mMarkToDelete.insert(ab1);
      	merge(ab1, ab2);
      }
      else if(ab2->getMass() / ab1->getMass() <= 1e-5)
      {
        mMarkToDelete.insert(ab2);
        merge(ab2, ab1);
      }
      else
      {
      	Vector3d tangent = collisionNormal ^ (collisionNormal ^ v2);
        tangent.normalise();
        double v2ProjectedOnTangent = tangent & v2;
        double vScalarNormal = (v2ProjectedOnNormal*(m2 - m1) + 2*m1*v1ProjectedOnNormal) / (m1 + m2);
        Vector3d v2n = collisionNormal * vScalarNormal;
        Vector3d vf = v2n + tangent * v2ProjectedOnTangent;

        Vector3d delta = ab2->getSpeed() - vf;
        delta = delta / t;
        double kineticEnergyOfdeceleration = 0.5 * ab2->getMass() *
          delta.norm() * delta.norm(); 
        if(kineticEnergyOfdeceleration > 10000 * ab2->getMass())
        {
          //on marque l'objet pour etre detruit et on ajoute
          //4 nouveau qui on le quart de la masse original
          mMarkToDelete.insert(ab2);   
          ab2->setSpeed(vf * 0.8);       
          explodeAstronomicalBody(ab2);          
        }
        else 
        {
          ab2->setSpeed(vf);
        }
      }
    }
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
  double turbo = 1.0;
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
  if(mKeyboard[kShift])
  	turbo = 100.0;
    
  mShip.setRoll(roll);
  mShip.setAcceleration(accel * turbo);
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
/*ipA se fusionne avec ipB*/
void Engine::merge(AstronomicalBody* ipA, AstronomicalBody* ipB)
{
	double m = ipA->getMass() + ipB->getMass();
  double v = m / ipB->getDensity();
  //double r = pow(3 * v / (4.0 * PI), 1.0/3.0);
  ipB->setMass(m);
	//ipB->setRadius(r);
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
void Engine::moveBodies(int iMs)
{
	//On deplace les objects
	for(unsigned int i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	AstronomicalBody& ab = *mAstronomicalBodies[i];
  	//La vitesse et position en fonction de lacceleration
    ab.setAcceleration( ab.getForce() / ab.getMass() );
  	Vector3d a = ab.getAcceleration();
	  Vector3d s;
    Point3d p;
    Matrix4d m = ab.getTransformation();
    double t = (iMs / 1000.0); //en sec
    
    s = ab.getSpeed() + a * t; //en km / s  
    p = ab.getTransformation().getTranslation() + s * t;
    
    /*On applique les limites du monde*/
		if(p.getX() > mSpaceRadius)
    {
    	p.setX(-mSpaceRadius);
      ab.resetPath();
    }
    if(p.getX() < -mSpaceRadius)
    {
    	p.setX(mSpaceRadius);
      ab.resetPath();
    }
    
    if(p.getY() > mSpaceRadius)
    {
    	p.setY(-mSpaceRadius);
      ab.resetPath();
    }
    if(p.getY() < -mSpaceRadius)
    {
    	p.setY(mSpaceRadius);
      ab.resetPath();
    }
      
    if(p.getZ() > mSpaceRadius)
    {
    	p.setZ(-mSpaceRadius);
      ab.resetPath();
    }
    if(p.getZ() < -mSpaceRadius)
    {
    	p.setZ(mSpaceRadius);
      ab.resetPath();
    }
      
    ab.setSpeed(s);
    m.setTranslation(p);
    ab.setTransformation(m);
    ab.setForce(Vector3d(0.0));
    if(isDebugging())
	    ab.addToPath(p);
  }
}

//-----------------------------------------------------------------------------
double Engine::normalizedRandom() const
{ return rand() / (double)RAND_MAX; }

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
void Engine::play(int iMs)
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
void Engine::solveCollisionsOverlap()
{
  AstronomicalBody* pAb1;
  AstronomicalBody* pAb2;
  Point3d pos1, pos2;

	detectCollisions();  
  while(!mCollisions.empty())
  {
printf("num coll: %d\n", mCollisions.size());    
    set<CollisionPair>::iterator it = mCollisions.begin();
    //for(; !mCollisions.empty() && it != mCollisions.end(); ++it)
    if(!mCollisions.empty())
    {
      pAb1 = mAstronomicalBodies[it->mIndex1];
      pAb2 = mAstronomicalBodies[it->mIndex2]; 
//printf("collision entre: %d et %d\n", it->mIndex1, it->mIndex2);
       
      Point3d pos1 = pAb1->getTransformation().getTranslation();
      Point3d pos2 = pAb2->getTransformation().getTranslation();

      //on repostionne les objects en collisions pour qu'il n'y ait pas
      //d'overlap
      Vector3d d = toVector(pos2 - pos1);
      double offset = d.norm();
      if( offset >= -EPSILON && offset <= EPSILON )
      	d.setXYZ(rand(), rand(), rand());
      d.normalise();
//printf("norm: %f\n", offset);
      offset -= pAb1->getRadius() + pAb2->getRadius();
      offset = fabs(offset);      
      Matrix4d m;
//printf("offset: %f\n", offset);      
      m.setTranslation(pos1 - 
        toPoint(d * 0.5 * offset ));
      pAb1->setTransformation(m);
      m.setTranslation(pos2 + 
        toPoint(d * 0.5 * offset ));
      pAb2->setTransformation(m);
    }
    detectCollisions();
  }
printf("solveCollisionsOverlap terminé\n");
}

//-----------------------------------------------------------------------------
void Engine::step()
{
	simulate(kTimeStep);
  refreshSortedPositions();
  callClients(Client::mFrameReady);
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
      play(kTimeStep);
  	  callClients(Client::mFrameReady);   
      break;
    case sSimulating:
    	handleUserInput();
      simulate(kTimeStep);
//      if(getNumberOfCycles() % 50 == 0)
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
  
  qint32 version = 2;
  ds << version;
  
  //on dompe les corps astrnomiques
  qlonglong numBodies = mAstronomicalBodies.size();
  ds << numBodies;
  for(unsigned int i = 0; i < mAstronomicalBodies.size(); ++i)
  {
  	const AstronomicalBody* pAb = mAstronomicalBodies[i];
  	ds << pAb->getName() <<
      pAb->getAcceleration().getX() << pAb->getAcceleration().getY() << pAb->getAcceleration().getZ() <<
      pAb->getDensity() <<
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
    getRadiusOfGeneration() <<
    getSpaceRadius();
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