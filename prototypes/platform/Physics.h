/* */

#ifndef realisim_platform_Physics_hh
#define realisim_platform_Physics_hh

namespace realisim
{
namespace platform 
{
class Actor;
class Engine;
class GameEntity;
class Monster;
class Player;
class Projectile;
class Stage;

class Physics
{
public:
	Physics();
  virtual ~Physics();
  
  void explode( const Point2d&, double, double, Engine& );
  double getTimeIncrement() const { return 0.015; }
  Intersection2d checkCollisions( GameEntity&, GameEntity& );
  void resolveCollisions( GameEntity&, Stage& );
  void resolveCollisions( Projectile&, Stage& );
  void update( GameEntity& );
  
protected:
	void applyFriction( GameEntity& );
	Vector2d penetration( const Rectangle&, const Rectangle& );
	Vector2d reflect( const Vector2d&, const Vector2d&, double );
  //Vector2d mGravity;
};


} //namespace platform
} //namespace realisim

#endif
