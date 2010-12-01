//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef REALEDIT_OBJECTNODE_H
#define REALEDIT_OBJECTNODE_H

#include "math/Quaternion.h"
#include "math/Matrix4x4.h"
#include <QString>
#include "DataModel/DataModel.h"
#include "math/Vect.h"
#include <vector>

namespace realEdit
{
using namespace realisim::math;

class ObjectNode
{
public:
  ObjectNode( QString iName, unsigned int = 0 );
  ObjectNode( const ObjectNode& iObjectNode );
  ~ObjectNode();
  
  ObjectNode* addNode( const QString iName, unsigned int = 0 );
  void addNode( ObjectNode* ipNode );
//void ObjectNode* copy(const ObjectNode&);
  const ObjectNode* getChild( int iChildNumber ) const;
  ObjectNode* getChild( int iChildNumber );
  unsigned int getChildCount() const;
  unsigned int getId() const {return mId;}
  const RealEditModel& getModel() const;
  RealEditModel& getModel();
  const QString getName() const;
  const ObjectNode* getParentNode() const {return mpParentNode;}
  ObjectNode* getParentNode() {return mpParentNode;}
  const Point3d getTranslation() const;
  const Matrix4d& getTransformation() const;
  const ObjectNode& operator=( const ObjectNode& iObjectNode ){ assert(0); }
  void removeChild(ObjectNode*);
  void setId(unsigned int iId) {mId = iId;} 
  void setName(QString iS) {mName = iS;}
  void setTransformation( const Matrix4d& iMat );
  void translate( const Point3d& iPos );
  void translate( const Vector3d& iTranslation );
  void rotate( const double iAngle, const Vector3d& iAxis );
  void rotate( const double iAngle, const Vector3d& iAxis,
    const Point3d& iAxisPos );
  
private:
  void setParentNode (ObjectNode* ipNode) {mpParentNode = ipNode;}
  
  static unsigned int mIdCounter;
  unsigned int mId;
  RealEditModel mModel;
  ObjectNode* mpParentNode;
  std::vector<ObjectNode*> mChilds;
  Matrix4d mTransformation;
  QString mName;
};

/**/
class Path
{
public:
  Path(const ObjectNode* ipNode);
  const Matrix4d& getNodeToScene() {return mNodeToScene;}
  const Matrix4d& getSceneToNode() {return mSceneToNode;}
  
private:
  Matrix4d mNodeToScene;
  Matrix4d mSceneToNode;
};

}//realEdit

#endif  //REALEDIT_OBJECTNODE_H
