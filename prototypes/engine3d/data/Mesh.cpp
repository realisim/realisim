
#include <cassert>
#include "Mesh.h"
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>

using namespace realisim;
  using namespace math;
using namespace engine3d;
  using namespace data;

//-------------------------------------------------------------------
Mesh::Mesh()
{}

//-------------------------------------------------------------------
Mesh::~Mesh()
{}

//-------------------------------------------------------------------
//void Mesh::makeFromObj(QString iPath)
//{
//  Mesh *m = new Mesh();
//
//  QFileInfo fi(iPath);
//  QFile file(iPath);
//
//  if( fi.isFile() &&
//     QString::compare(fi.suffix(), "obj", Qt::CaseInsensitive) == 0 &&
//     file.open(QIODevice::ReadOnly | QIODevice::Text))
//  {
//    
//    QTextStream in(&file);
//    while (!in.atEnd())
//    {
//      QString line = in.readLine();
//      QStringList tokens = line.split(" ", QString::SkipEmptyParts);
//      if( tokens.size() > 0 )
//      {
//        if( tokens[0] == "o" )
//        {}
//        else if( tokens[0] == "v" )
//        {
//          assert(tokens.size() == 4);
//          m->mVertices.push_back( Point3d(tokens[1].toDouble(),
//                                          tokens[2].toDouble(),
//                                          tokens[3].toDouble() ) );
//        }
//        else if( tokens[0] == "vn" )
//        {
//          assert(tokens.size() == 4);
//          m->mNormals.push_back( Vector3d(tokens[1].toDouble(),
//                                          tokens[2].toDouble(),
//                                          tokens[3].toDouble() ) );
//        }
//        else if( tokens[0] == "vt" )
//        {
//          assert(tokens.size() == 3);
//          m->mTexture2dCoordinates.push_back( Vector2d(tokens[1].toDouble(),
//                                          tokens[2].toDouble() ) );
//        }
//        else if( tokens[0] == "f" )
//        {
//          assert(tokens.size() >= 4);
//          for(int i = 1; i < 4; ++i)
//          {
//            face f;
//            QStringList indices = tokens[i].split("/", QString::SkipEmptyParts);
//            switch (indices.size())
//            {
//              case 2: //vertex index
//                f.mVertexIndices.push_back(indices[0].toInt());
//                break;
//              case 3: //vertex index/texture index
//                f.mVertexIndices.push_back(indices[0].toInt());
//                f.mTexture2dIndices.push_back(indices[1].toInt());
//                break;
//              case 4: //vertex index/texture index/normal index
//                f.mVertexIndices.push_back(indices[0].toInt());
//                f.mTexture2dIndices.push_back(indices[1].toInt());
//                f.mNormalIndices.push_back(indices[2].toInt());
//                break;
//                
//              default:break;
//            }
//
//          }
//        }
//        else if( tokens[0] == "usemtl" )
//        {}
//        else{}
//      }
//    }
//  }
//  return m;
//}