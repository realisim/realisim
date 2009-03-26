//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef OBJECTNODE_H
#define OBJECTNODE_H

#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Vect.h"

#include <cassert>
#include <string>
#include <vector>

namespace RealEdit { class ObjectNode; }
namespace RealEdit { class RealEditModel; }


class RealEdit::ObjectNode
{
public:
  ObjectNode( const std::string& iName );
  ObjectNode( const ObjectNode& iObjectNode ){ assert(0); }
  ~ObjectNode();
  
  ObjectNode* addNode( const std::string& iName );
  const ObjectNode* getChild( int iChildNumber ) const;
  ObjectNode* getChild( int iChildNumber );
  unsigned int getChildCount() const;
  const RealEditModel* getModel() const;
  RealEditModel* getModel();
  const std::string& getName() const;
  const Realisim::Point3d getTranslation() const;
  const Realisim::Matrix4d& getTransformation() const;
  const ObjectNode& operator=( const ObjectNode& iObjectNode ){ assert(0); }
  void setTransformation( const Realisim::Matrix4d& iMat );
  void translate( const Realisim::Point3d& iPos );
  void translate( const Realisim::Vector3d& iTranslation );
  void rotate( const double iAngle, const Realisim::Vector3d& iAxis );
  void rotate( const double iAngle, const Realisim::Vector3d& iAxis,
    const Realisim::Point3d& iAxisPos );
  
  
private:
  RealEditModel* mpModel;
  ObjectNode* mpParentNode;
  std::vector<ObjectNode*> mChilds;
  Realisim::Matrix4d mTransformation;
  
  std::string mName;
};

#endif  //OBJECTNODE_H
