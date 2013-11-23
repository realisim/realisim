/* Particules.cpp */

#include "math/MathUtils.h"
#include "limits.h"
#include "Particules.h"
#include "qgl.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace std;

Particules::Particules() : 
	mTime(),
  mType( tOmniDirectional ),
  mVelocityRange( make_pair( 0.0, 1.0 ) ),
  mLifeRange( make_pair( 100, 1000 ) ),
  mParticules(),
  mpParticulesPosition( 0 ),
  mTexture(),
  mAttractors(),
  mPosition( 0.0 ),
  mColor( 255, 255, 255, 255 ),
  mSize( 4 ),
  mDirection( 1.0 ),
  mRadius( 1.0 ),
  mIsDecayEnabled( true ),
  mRate( -1 )
{
	srand( time( NULL ) );
  mTime.start();
}

Particules::Particules( const Particules& iRhs ) :
 mTime( iRhs.mTime ),
 mType( iRhs.mType ),
 mVelocityRange( iRhs.mVelocityRange ),
 mLifeRange( iRhs.mLifeRange ),
 mParticules( iRhs.mParticules ),
 mpParticulesPosition( new double[ iRhs.getNumberOfParticules() * 3 ] ),
 mTexture( iRhs.mTexture ),
 mAttractors( iRhs.mAttractors ),
 mPosition( iRhs.mPosition ),
 mColor( iRhs.mColor ),
 mSize( iRhs.mSize ),
 mDirection( iRhs.mDirection ),
 mRadius( iRhs.mRadius ),
 mIsDecayEnabled( iRhs.mIsDecayEnabled ),
 mRate( iRhs.mRate )
{
	srand( time( NULL ) );
  mTime.start();
}

Particules& Particules::operator=( const Particules& iRhs )
{ 
  mTime = iRhs.mTime;
  mType = iRhs.mType;
  mVelocityRange = iRhs.mVelocityRange;
  mLifeRange = iRhs.mLifeRange;
  mParticules = iRhs.mParticules;
  if( mpParticulesPosition ) delete[] mpParticulesPosition;
  mpParticulesPosition = new double[ iRhs.getNumberOfParticules() * 3 ];
  mTexture = iRhs.mTexture;
  mAttractors = iRhs.mAttractors;
  mPosition = iRhs.mPosition;
  mColor = iRhs.mColor;
  mSize = iRhs.mSize;
  mDirection = iRhs.mDirection;
  mRadius = iRhs.mRadius;
  mIsDecayEnabled = iRhs.mIsDecayEnabled;
  mRate = iRhs.mRate;
  srand( time( NULL ) );
  mTime.start();
  return *this;
}

Particules::~Particules()
{
	if( mpParticulesPosition ) delete[] mpParticulesPosition;
}
//-----------------------------------------------------------------------------
void Particules::addAttractor( attractorType iType, Point3d iP, Vector3d iF,
	double iM )
{ mAttractors.push_back( Attractor(iType, iP, iF, iM) ); }
//-----------------------------------------------------------------------------
void Particules::draw() const
{ iterate(); drawParticules(); }
//-----------------------------------------------------------------------------
void Particules::enableDecay( bool iE )
{ mIsDecayEnabled = iE; }
//-----------------------------------------------------------------------------
void Particules::drawParticules() const
{
	int index;
	for( int i = 0; i < getNumberOfParticules(); ++i )
  {
  	Particule& p = mParticules[i];
  	index = i * 3;
  	mpParticulesPosition[ index ] = p.p.getX();
    mpParticulesPosition[ index + 1 ] = p.p.getY();
    mpParticulesPosition[ index + 2 ] = p.p.getZ();
  }

  glColor4ub( mColor.red(), mColor.green(), mColor.blue(), mColor.alpha() );
  glDisable( GL_LIGHTING );
  glDepthMask( GL_FALSE );
	glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glActiveTexture(GL_TEXTURE0);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glBindTexture( GL_TEXTURE_2D, mTexture.getId() );
  glPointSize( mSize );
  
  glEnableClientState( GL_VERTEX_ARRAY );
  glVertexPointer( 3, GL_DOUBLE, 0, mpParticulesPosition );
  glDrawArrays( GL_POINTS, 0, getNumberOfParticules() );
  glDisableClientState( GL_VERTEX_ARRAY );
  
  glDisable(GL_POINT_SPRITE);
  glDisable( GL_BLEND );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glDisable( GL_TEXTURE_2D );
  glDepthMask( GL_TRUE );
  glEnable( GL_LIGHTING );
}
//-----------------------------------------------------------------------------
const Vector3d& Particules::getAttractorForce( int iIndex ) const
{ return mAttractors[iIndex].f; }
//-----------------------------------------------------------------------------
double Particules::getAttractorMagnitude( int iIndex ) const
{ return mAttractors[iIndex].m; }
//-----------------------------------------------------------------------------
const Point3d& Particules::getAttractorPosition( int iIndex ) const
{ return mAttractors[iIndex].p; }
//-----------------------------------------------------------------------------
Particules::attractorType Particules::getAttractorType( int iIndex ) const
{ return mAttractors[iIndex].t; }
//-----------------------------------------------------------------------------
QColor Particules::getColor() const
{ return mColor; }
//-----------------------------------------------------------------------------
const Vector3d& Particules::getDirection() const
{ return mDirection; }
//-----------------------------------------------------------------------------
pair< int, int > Particules::getLifeRange() const
{ return mLifeRange; }
//-----------------------------------------------------------------------------
int Particules::getNumberOfAttractors() const
{ return mAttractors.size(); }
//-----------------------------------------------------------------------------
int Particules::getNumberOfParticules() const
{ return mParticules.size(); }
//-----------------------------------------------------------------------------
double Particules::getRadius() const
{ return mRadius; }
//-----------------------------------------------------------------------------
// voir méthode setRate
int Particules::getRate() const
{ return mRate; }
//-----------------------------------------------------------------------------
const Point3d& Particules::getPosition() const
{ return mPosition; }
//-----------------------------------------------------------------------------
double Particules::getSize() const
{ return mSize; }
//-----------------------------------------------------------------------------
Particules::type Particules::getType() const
{ return mType; }
//-----------------------------------------------------------------------------
pair< double, double > Particules::getVelocityRange() const
{ return mVelocityRange; }
//-----------------------------------------------------------------------------
bool Particules::isDecayEnabled() const
{ return mIsDecayEnabled; }
//-----------------------------------------------------------------------------
void Particules::iterate() const
{
  int elapsed = mTime.elapsed();
  mTime.start();
  int r1, r2, r3;
  double m1, m2, m3;
  int numberOfParticuleCreated = 0;
	for( int i = 0; i < getNumberOfParticules(); ++i )
  {
  	Particule& p = mParticules[i];
    /* Si la particule est morte, on en génènre une nouvelle. Un rate de -1
      signifie qu'on génère toutes les particules mortes, sinon on génère
      seulement le nombre maximal alloué par mRate pour le temps écoulé.*/
  	if( p.t <= 0.0 ) 
    {
    	if( ( mRate == -1 || 
         numberOfParticuleCreated++ <= elapsed * mRate / 1000.0 ) )
      {
        r1 = rand();
        r2 = rand();
        r3 = rand();
        m1 = r1 % 2 == 0 ? 1.0 : -1.0;
        m2 = r2 % 2 == 0 ? 1.0 : -1.0;
        m3 = r3 % 2 == 0 ? 1.0 : -1.0;
        switch (mType) 
        {
          case tOmniDirectional:
            p.v = Vector3d( m1 * r1, m2 * r2, m3 * r3 );
            p.v.normalise();
            p.v *= r2 * r2 / (double)RAND_MAX * (mVelocityRange.second - 
            	mVelocityRange.first ) +  mVelocityRange.first;            
            p.p = mPosition;
          break;
          case tDirectional:
            p.v = mDirection;
            p.v *= r2 / (double)RAND_MAX * (mVelocityRange.second - 
            	mVelocityRange.first ) +  mVelocityRange.first;
            p.p = mPosition;
          break;
          case tDisk:
          {
            p.v = mDirection;
            p.v *= r2 / (double)RAND_MAX * (mVelocityRange.second - 
            	mVelocityRange.first ) +  mVelocityRange.first;
            //on trouve un vecteur a 90 degree
            /*Pour trouver le vecteur perpendiculaire (w) à v tel que
              v = (ai, bj, ck) il faut satisfaire l'équation
              w * v = 0.
              Le plus facile est de permutter 2 composante, ajouter un signe
              négatif et completer avec un 0.
              Par example:
              w = (-bi, aj, 0k)
              w * v = (ai, bj, ck) * ( -bi, aj, 0k) = -ab + ab + 0 = 0  */
            Vector3d v( mDirection.getY(), -mDirection.getX(), 0.0 );          
            v.normalise();
            /*on fait une rotation random autour de mDirection*/
            Matrix4d r = getRotationMatrix( r1 % 360 * 3.1415629 / 180.0, 
              mDirection );
            v = v * r;
            /* on ajoute un valeur random entre 0.0 et mRadis */
            v *= r2 / (double)RAND_MAX * mRadius;
            p.p = mPosition + v;
          }
          break;
          case tCone:
          {
          	p.v = mDirection;
            Vector3d v( mDirection.getY(), -mDirection.getX(), 0.0 );          
            v.normalise();
            /*on fait une rotation random autour de mDirection*/
            Matrix4d r = getRotationMatrix( r1 % 360 * 3.1415629 / 180.0, 
              mDirection );
            v = v * r;
            /* on ajoute un valeur random entre 0.0 et mRadis */
            v *= r2 / (double)RAND_MAX * mRadius;
            p.v += v;
            p.v *= r2 / (double)RAND_MAX * (mVelocityRange.second - 
            	mVelocityRange.first ) +  mVelocityRange.first;
            p.p = mPosition;
          }
          break;
          default: break;
        }
        
        p.t = r3*r3 / (double)RAND_MAX * (mLifeRange.second - mLifeRange.first ) + 
          mLifeRange.first;
      }
      else 
      {
        p.p = Point3d( std::numeric_limits<double>::quiet_NaN() ) ;
      }

    }
    else 
    {
    	/*le temps ecoulé est en ms et la vitesse en x/s */
      
      /* on calcule les forces exercées par les attracteurs */
      Vector3d f(0.0);
      double dist = 0.0;
      for( int j = 0; j < getNumberOfAttractors(); ++j )
      {
      	const Attractor& a = mAttractors[j];        
        switch (a.t) 
        {
          case atPositional: 
          	dist = (p.p - a.p).fastNorm();
          	f += (p.p - a.p).normalise() * a.m / dist;
          break;
          case atGlobal: f += a.f * a.m; break;
          default: break;
        }
      }
      
      p.v += f * (elapsed / 1000.0);
    	p.p += toPoint( p.v * (elapsed / 1000.0) );
      if( isDecayEnabled() )
      { p.t -= elapsed; }
    }
  }
}
//-----------------------------------------------------------------------------
void Particules::removeAttractor( int iIndex )
{}
//-----------------------------------------------------------------------------
void Particules::setAttractorForce( int iIndex, const Vector3d& iF )
{ mAttractors[ iIndex ].f = iF; }
//-----------------------------------------------------------------------------
void Particules::setAttractorMagnitude( int iIndex, double iM )
{ mAttractors[ iIndex ].m = iM; }
//-----------------------------------------------------------------------------
void Particules::setAttractorPosition( int iIndex , const Point3d& iP )
{ mAttractors[ iIndex ].p = iP; }
//-----------------------------------------------------------------------------
void Particules::setAttractorType( int iIndex, attractorType iT )
{ mAttractors[ iIndex ].t = iT; }
//-----------------------------------------------------------------------------
void Particules::setColor( int iR, int iG, int iB, int iA )
{ mColor.setRgb( iR, iG, iB, iA ); }
//-----------------------------------------------------------------------------
void Particules::setDirection( const Vector3d& iD )
{ mDirection = iD; mDirection.normalise(); }
//-----------------------------------------------------------------------------
void Particules::setImage( QImage iImage )
{ mTexture.set( iImage ); }
//-----------------------------------------------------------------------------
void Particules::setLifeRange( int iLowerBound, int iUpperBound )
{ mLifeRange = make_pair( iLowerBound, iUpperBound ); }
//-----------------------------------------------------------------------------
void Particules::setNumberOfParticules( int iN )
{ 
  mParticules.resize( iN );
  if( mpParticulesPosition ) delete[] mpParticulesPosition;
  mpParticulesPosition = new double[ getNumberOfParticules() * 3 ];
}
//-----------------------------------------------------------------------------
void Particules::setPosition( const Point3d& iP )
{ mPosition = iP; }
//-----------------------------------------------------------------------------
void Particules::setRadius( double iR )
{ mRadius = iR; }
//-----------------------------------------------------------------------------
/*Indique combien de particules par seconde la source emettra. La valeur
  par defaut est de -1 et signifie un rate infini... Donc la source emettra
  le maximum de particules possible.*/
void Particules::setRate( int iR /*= -1*/ )
{ mRate = iR; }
//-----------------------------------------------------------------------------
void Particules::setSize( double iS )
{ mSize = iS; }
//-----------------------------------------------------------------------------
void Particules::setType( type iType )
{
	mType = iType;
}
//-----------------------------------------------------------------------------
void Particules::setVelocityRange( double iLowerBound, double iUpperBound )
{
	mVelocityRange = make_pair( iLowerBound, iUpperBound );
}
//-----------------------------------------------------------------------------
//--- particule
//-----------------------------------------------------------------------------
Particules::Particule::Particule() : 
  v( Vector3d() ),
  p( Point3d() ),
  t( -1.0 )
{
}
Particules::Particule::Particule( const Vector3d& iV, const Point3d& iP, 
	int iT ) : 
  v( iV ),
  p( iP ),
  t( iT )
{
}
//-----------------------------------------------------------------------------
//--- particule
//-----------------------------------------------------------------------------
Particules::Attractor::Attractor( attractorType iType,
  const Point3d& iP, const Vector3d& iF, double iM ) :
  t( iType ),
  p( iP ),
  f( iF ),
  m( iM )
{ f.normalise(); }
