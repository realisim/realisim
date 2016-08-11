/**/

#include <3d/Camera.h>
#include <3d/Utilities.h>
#include <math/Point.h>
#include <math/Vect.h>
#include "MainDialog.h"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <utils/utilities.h>


using namespace realisim;
using namespace math;
using namespace treeD;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent)
{
	setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::draw()
{
	Widget3d::draw();
  
  glDisable(GL_LIGHTING);
  glColor3ub(0, 255, 0);
  mVbo.draw();
}

void Viewer::initializeGL()
{
	Widget3d::initializeGL();
	glClearColor(0.2, 0.2, 0.2, 0.0);

	//init mFbo
	mFbo.addColorAttachment(true);
	mFbo.getTexture(0).setMinificationFilter(GL_LINEAR);

  //init Vbo
  std::vector<Point3d> vertices;
  std::vector<int> indices;
  std::vector<Vector3d> normals;
  std::vector<float> colors;
  
  vertices.push_back(Point3d(-10.0, -10.0, 0.0));
  vertices.push_back(Point3d(10.0, -10.0, 0.0));
  vertices.push_back(Point3d(0.0, 10.0, 0.0));
  
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  
  colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0);
  colors.push_back(0.0); colors.push_back(1.0); colors.push_back(0.0); colors.push_back(1.0);
  colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0); colors.push_back(1.0);
  
  mVbo.setVertices(vertices);
  mVbo.setIndices(indices);
  mVbo.setColors(colors);
  mVbo.bake();
  
	loadModels();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key())
	{
	case Qt::Key_F5: ; break;
	default: break;
	}
	update();
}

//-----------------------------------------------------------------------------
void Viewer::loadModels()
{
  const QString assetsPath = utils::getAssetFolder();
  printf("assetsPath: %s\n", assetsPath.toStdString().c_str());
	//init shaders
//	{
//		QFile f(cwd + "/../assets/highIntensityFilter.frag");
//		f.open(QIODevice::ReadOnly);
//		QString content = f.readAll();
//		mHighIntensityFilter.addFragmentSource(content);
//	}
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
	Widget3d::paintGL();
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
	Widget3d::resizeGL(iW, iH);

	if (!(mFbo.getWidth() == iW && mFbo.getHeight() == iH))
	{
		mFbo.resize(iW, iH);
	}
}

//-----------------------------------------------------------------------------
//--- MainDialog
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
mpViewer(0)
{
	resize(800, 600);

	QWidget *pCentralWidget = new QWidget(this);
	setCentralWidget(pCentralWidget);

	QHBoxLayout* pLyt = new QHBoxLayout(pCentralWidget);
	pLyt->setMargin(5);
	{
		QVBoxLayout *pControlLyt = new QVBoxLayout();
		{
		}		

		mpViewer = new Viewer(pCentralWidget);

		pLyt->addLayout(pControlLyt, 1);
		pLyt->addWidget(mpViewer, 4);
	}

	treeD::Camera c = mpViewer->getCamera();
	c.set(Point3d(0.0, 0.0, 100),
		Point3d(), Vector3d(0, 1, 0));
	mpViewer->setCamera(c);
	mpViewer->setControlType(treeD::Widget3d::ctPan);

	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	update();
	mpViewer->update();
}