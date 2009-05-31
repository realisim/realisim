//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef REALEDIT_OBJECTNODE_H
#define REALEDIT_OBJECTNODE_H

#include "Quaternion.h"
#include "Matrix4x4.h"
#include <QString>
#include "DataModel.h"
#include "Vect.h"
#include <vector>

namespace RealEdit { class ObjectNode; }
namespace RealEdit { class Path; }
namespace RealEdit { class RealEditModel; }


class RealEdit::ObjectNode
{
public:
  ObjectNode( QString iName );
  ObjectNode( const ObjectNode& iObjectNode );
  ~ObjectNode();
  
  ObjectNode* addNode( const QString iName );
  void addNode( ObjectNode* ipNode );
  const ObjectNode* getChild( int iChildNumber ) const;
  ObjectNode* getChild( int iChildNumber );
  unsigned int getChildCount() const;
  const RealEditModel& getModel() const;
  RealEditModel& getModel();
  const QString getName() const;
  const ObjectNode* getParentNode() const {return mpParentNode;}
  const Realisim::Point3d getTranslation() const;
  const Realisim::Matrix4d& getTransformation() const;
  const ObjectNode& operator=( const ObjectNode& iObjectNode ){ assert(0); }
  void setTransformation( const Realisim::Matrix4d& iMat );
  void setParentNode (ObjectNode* ipNode) {mpParentNode = ipNode;}
  void translate( const Realisim::Point3d& iPos );
  void translate( const Realisim::Vector3d& iTranslation );
  void rotate( const double iAngle, const Realisim::Vector3d& iAxis );
  void rotate( const double iAngle, const Realisim::Vector3d& iAxis,
    const Realisim::Point3d& iAxisPos );
  
private:
  RealEditModel mModel;
  ObjectNode* mpParentNode;
  std::vector<ObjectNode*> mChilds;
  Realisim::Matrix4d mTransformation;
  QString mName;
};

class RealEdit::Path
{
public:
  Path(const ObjectNode* ipNode);
  const Realisim::Matrix4d& getSceneTransformation() {return mSceneToNode;}
  
private:
  Realisim::Matrix4d mSceneToNode;
};

#endif  //REALEDIT_OBJECTNODE_H
