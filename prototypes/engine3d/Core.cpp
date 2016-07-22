
#include "3d/Camera.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"


using namespace realisim;
  using namespace math;
  using namespace treeD;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent)
{
  //startTimer(16);
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::initializeGL()
{
  Widget3d::initializeGL();
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
}