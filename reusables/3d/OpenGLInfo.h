#ifndef Realisim_TreeD_OpenGLInfo_hh
#define Realisim_TreeD_OpenGLInfo_hh

/*Cette classe permet d'afficher et de demander des informations relatives
  a opengl. Le numéro de version d"openGL, le numéro de version du langage
  de shader, le manufacturier de la carte et les extensions supportées.*/

#include "openGlHeaders.h"
#include <vector>
#include <string>

namespace realisim
{
namespace treeD
{

class OpenGLInfo
{
public:
  OpenGLInfo();
  bool isExtensionSupported(const char* ext) const; 
  void print();
  
protected:
  bool isValid() const {return mIsValid;}
  void refresh();

  std::string mVendor;
  std::string mRenderer;
  std::string mVersion;
  std::string mGlSlVersion;
  std::vector <std::string> mExtensions;
  int mMaxTextureSize;
  int mMax3dTextureSize;
  bool mIsValid;
};
   
}//treeD
}//realisim

#endif