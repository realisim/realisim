/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <QKeyEvent>
#include <qgroupbox.h>
#include <qlayout.h>
#include <QFile.h>
#include <qradiobutton.h>
#include <qslider.h>

using namespace realisim;
  using namespace math;
  using namespace treeD;

namespace  			
{
	enum view{vAxial, vSagittal, vCoronal};
  view gView = vAxial;

  Vector3d gVolumeSize(73, 512, 512);
	float gTexValue = 0.5;
  Vector3d gTexCoord0(gTexValue, 0.0, 0.0);
  Vector3d gTexCoord1(gTexValue, 0.0, 1.0);
  Vector3d gTexCoord2(gTexValue, 1.0, 1.0);
  Vector3d gTexCoord3(gTexValue, 1.0, 0.0);
  Vector3d gSpacing(3.0, 0.91, 0.91);
  Vector3d gScale = gVolumeSize * gSpacing;
  
  Vector3f gOffset9[9] = {
    Vector3f(0, -1, 1), Vector3f(0, 0, 1), Vector3f(0, 1, 1),
    Vector3f(0, -1, 0), Vector3f(0, 0, 0), Vector3f(0, 1, 0),
    Vector3f(0, -1, -1), Vector3f(0, 0, -1), Vector3f(0, 1, -1)};
    
  Vector3f gOffset25[25] = {
    Vector3f(0, -2, 2), Vector3f(0, -1, 2), Vector3f(0, 0, 2), Vector3f(0, 1, 2), Vector3f(0, 2, 2),
    Vector3f(0, -2, 1), Vector3f(0, -1, 1), Vector3f(0, 0, 1), Vector3f(0, 1, 1), Vector3f(0, 2, 1),
    Vector3f(0, -2, 0), Vector3f(0, -1, 0), Vector3f(0, 0, 0), Vector3f(0, 1, 0),
    Vector3f(0, -2, -1), Vector3f(0, -1, -1), Vector3f(0, 0, -1), Vector3f(0, 1, -1), Vector3f(0, 2, -1),
    Vector3f(0, -2, -2), Vector3f(0, -1, -2), Vector3f(0, 0, -2), Vector3f(0, 1, -2), Vector3f(0, 2, -2)};
    
  Vector3f gOffset27[27] = {
    Vector3f(-1, -1, 1), Vector3f(0, -1, 1), Vector3f(1, -1, 1),
    Vector3f(-1, 0, 1), Vector3f(0, 0, 1), Vector3f(1, 0, 1),
    Vector3f(-1, 1, 1), Vector3f(0, 1, 1), Vector3f(1, 1, 1),
    
    Vector3f(-1, -1, 0), Vector3f(0, -1, 0), Vector3f(1, -1, 0),
    Vector3f(-1, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 0, 0),
    Vector3f(-1, 1, 0), Vector3f(0, 1, 0), Vector3f(1, 1, 0),
    
    Vector3f(-1, -1, -1), Vector3f(0, -1, -1), Vector3f(1, -1, -1),
    Vector3f(-1, 0, -1), Vector3f(0, 0, -1), Vector3f(1, 0, -1),
    Vector3f(-1, 1, -1), Vector3f(0, 1, -1), Vector3f(1, 1, -1)
    };

  float gScaleFactor = 1.0;
  bool gAddOnBase = false;
  bool gShowCT = true;
  
  double gWindowingMin = 0.0;
  double gWindowingMax = 1.0;
}

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
 mConvolutionShader(),
 mActiveShader(),
 mDefaultShader(),
 mColorizeShader(),
 mCtTexture(),
 mFilterType(fNone),
 mFilter(9)
{
  setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::drawSlice()
{
  glBegin(GL_QUADS);
  glTexCoord3dv(gTexCoord0.getPtr());
  glVertex3d(0.0, 0.0, 0.0);
  glTexCoord3dv(gTexCoord1.getPtr());
  glVertex3d(0.0, 1.0, 0.0);
  glTexCoord3dv(gTexCoord2.getPtr());
  glVertex3d(1.0, 1.0, 0.0);
  glTexCoord3dv(gTexCoord3.getPtr());
  glVertex3d(1.0, 0.0, 0.0);
  glEnd();
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  QString fragmentSource, vertexSource;
  
  //--- Convolution shader
  QFile vert(":/convolution.vert");
  vert.open(QIODevice::ReadOnly);
  vertexSource = vert.readAll();
  vert.close();

  QFile frag(":/convolution.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  frag.close();

  mConvolutionShader.addVertexShaderSource(vertexSource);
  mConvolutionShader.addFragmentShaderSource(fragmentSource);
  
  frag.setFileName(":/windowing.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  frag.close();
  mConvolutionShader.addFragmentShaderSource(fragmentSource);
  
  mConvolutionShader.link();
  
  //default shader
  frag.setFileName(":/windowing.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  frag.close();
  mDefaultShader.addFragmentShaderSource(fragmentSource);
  
  QString customMain = " \
  uniform sampler3D texture;\
  vec4 applyWindowing(in sampler3D iTex, in vec3 iPos);\
  void main()\
  {\
    gl_FragColor = applyWindowing(texture, gl_TexCoord[0].xyz);\
  }";
  mDefaultShader.addFragmentShaderSource(customMain);
  mDefaultShader.link();
  
  //colorize shader
  frag.setFileName(":/colorize.frag");
  frag.open(QIODevice::ReadOnly);
  fragmentSource = frag.readAll();
  frag.close();
  mColorizeShader.addFragmentShaderSource(fragmentSource);
  mColorizeShader.link();
  
  //CT
  QFile f("../Resources/73_512_512");
  if(f.open(QIODevice::ReadOnly))
  {
    QByteArray a = f.readAll();
    mCtTexture.set(a.data(), Vector3i(73, 512, 512), GL_LUMINANCE,
      GL_UNSIGNED_SHORT, GL_LINEAR);
    f.close();
  }
  
  //ultrasound
  //f.setFileName("../Resources/ultrasound_340_431_364");
  //f.setFileName("../Resources/bad_breast_scan_281_254_245");
  f.setFileName("../Resources/nice_breast_scan_277_219_247");
  if(f.open(QIODevice::ReadOnly))
  {
    QByteArray a = f.readAll();
    mUltraSoundTexture.set(a.data(), Vector3i(277, 219, 247), GL_LUMINANCE,
      GL_UNSIGNED_BYTE, GL_LINEAR);
    f.close();
  }
  
  setFilterType(fNone);
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipEvent)
{
  switch (ipEvent->key()) 
  {
    default: break;
  }
  update();
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();

  glPushMatrix();
  	
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
  glDisable(GL_LIGHTING);
  
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_3D);
  if(gShowCT)
    glBindTexture(GL_TEXTURE_3D, mCtTexture.getTextureId());
  else
    glBindTexture(GL_TEXTURE_3D, mUltraSoundTexture.getTextureId());
  
  pushShader(mActiveShader);
  glScaled(gScale.getX(), gScale.getY(), gScale.getZ());
  drawSlice();
  popShader();
 
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0xBBBB);
  drawSlice();

  glPopAttrib();
  glPopMatrix();

  showFps();
}

//-----------------------------------------------------------------------------
void Viewer::setFilterType(filterType iType)
{
	mFilterType = iType;
  
  switch (getFilterType()) 
  {
    case fNone:
      mActiveShader = mDefaultShader;
      pushShader(mActiveShader);
      mActiveShader.setUniform("texture", 0);
      mActiveShader.setUniform("windowingMin", gWindowingMin);
      mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fMean:
      mFilter = ConvolutionFilter(9);
      mFilter.mKernelValues[0] = 0.111;
      mFilter.mKernelValues[1] = 0.111;
      mFilter.mKernelValues[2] = 0.111;
      mFilter.mKernelValues[3] = 0.111;
      mFilter.mKernelValues[4] = 0.111;
      mFilter.mKernelValues[5] = 0.111;
      mFilter.mKernelValues[6] = 0.111;
      mFilter.mKernelValues[7] = 0.111;
      mFilter.mKernelValues[8] = 0.111;
      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 9, &gOffset9[0]);
  			mActiveShader.setUniform("kernelSize", 9);
  			mActiveShader.setUniform("kernelValue", 9, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fGausian:
      mFilter = ConvolutionFilter(25);
      mFilter.mKernelValues[0] = 1 / 273.0;
      mFilter.mKernelValues[1] = 4 / 273.0;
      mFilter.mKernelValues[2] = 7 / 273.0;
      mFilter.mKernelValues[3] = 4 / 273.0;
      mFilter.mKernelValues[4] = 1 / 273.0;
      
      mFilter.mKernelValues[5] = 4 / 273.0;
      mFilter.mKernelValues[6] = 16 / 273.0;
      mFilter.mKernelValues[7] = 26 / 273.0;
      mFilter.mKernelValues[8] = 16 / 273.0;
      mFilter.mKernelValues[9] = 4 / 273.0;
      
      mFilter.mKernelValues[10] = 7 / 273.0;
      mFilter.mKernelValues[11] = 26 / 273.0;
      mFilter.mKernelValues[12] = 41 / 273.0;
      mFilter.mKernelValues[13] = 26 / 273.0;
      mFilter.mKernelValues[14] = 7 / 273.0;
      
      mFilter.mKernelValues[15] = 4 / 273.0;
      mFilter.mKernelValues[16] = 16 / 273.0;
      mFilter.mKernelValues[17] = 26 / 273.0;
      mFilter.mKernelValues[18] = 16 / 273.0;
      mFilter.mKernelValues[19] = 4 / 273.0;
      
      mFilter.mKernelValues[20] = 1 / 273.0;
      mFilter.mKernelValues[21] = 4 / 273.0;
      mFilter.mKernelValues[22] = 7 / 273.0;
      mFilter.mKernelValues[23] = 4 / 273.0;
      mFilter.mKernelValues[24] = 1 / 273.0;

      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 25, &gOffset25[0]);
  			mActiveShader.setUniform("kernelSize", 25);
  			mActiveShader.setUniform("kernelValue", 25, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fEdgeDetection:
      mFilter = ConvolutionFilter(9);
      mFilter.mKernelValues[0] = 0.0;
      mFilter.mKernelValues[1] = 1.0;
      mFilter.mKernelValues[2] = 0.0;
      mFilter.mKernelValues[3] = 1.0;
      mFilter.mKernelValues[4] = -4.0;
      mFilter.mKernelValues[5] = 1.0;
      mFilter.mKernelValues[6] = 0.0;
      mFilter.mKernelValues[7] = 1.0;
      mFilter.mKernelValues[8] = 0.0;
      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 9, &gOffset9[0]);
  			mActiveShader.setUniform("kernelSize", 9);
  			mActiveShader.setUniform("kernelValue", 9, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fSharpening:
      mFilter = ConvolutionFilter(9);
      mFilter.mKernelValues[0] = 0.0;
      mFilter.mKernelValues[1] = -1.0;
      mFilter.mKernelValues[2] = 0.0;
      mFilter.mKernelValues[3] = -1.0;
      mFilter.mKernelValues[4] = 4.0;
      mFilter.mKernelValues[5] = -1.0;
      mFilter.mKernelValues[6] = 0.0;
      mFilter.mKernelValues[7] = -1.0;
      mFilter.mKernelValues[8] = 0.0;
      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 9, &gOffset9[0]);
  			mActiveShader.setUniform("kernelSize", 9);
  			mActiveShader.setUniform("kernelValue", 9, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fEdgeDetection3d:
      mFilter = ConvolutionFilter(27);
      mFilter.mKernelValues[0] = 0.0;
      mFilter.mKernelValues[1] = 0.0;
      mFilter.mKernelValues[2] = 0.0;
      mFilter.mKernelValues[3] = 0.0;
      mFilter.mKernelValues[4] = 1;
      mFilter.mKernelValues[5] = 0.0;
      mFilter.mKernelValues[6] = 0.0;
      mFilter.mKernelValues[7] = 0.0;
      mFilter.mKernelValues[8] = 0.0;
      mFilter.mKernelValues[9] = 0.0;
      mFilter.mKernelValues[10] = 1;
      mFilter.mKernelValues[11] = 0.0;
      mFilter.mKernelValues[12] = 1;
      mFilter.mKernelValues[13] = -6;
      mFilter.mKernelValues[14] = 1;
      mFilter.mKernelValues[15] = 0.0;
      mFilter.mKernelValues[16] = 1;
      mFilter.mKernelValues[17] = 0.0;
      mFilter.mKernelValues[18] = 0.0;
      mFilter.mKernelValues[19] = 0.0;
      mFilter.mKernelValues[20] = 0.0;
      mFilter.mKernelValues[21] = 0.0;
      mFilter.mKernelValues[22] = 1;
      mFilter.mKernelValues[23] = 0.0;
      mFilter.mKernelValues[24] = 0.0;
      mFilter.mKernelValues[25] = 0.0;
      mFilter.mKernelValues[26] = 0.0;
      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 27, &gOffset27[0]);
  			mActiveShader.setUniform("kernelSize", 27);
  			mActiveShader.setUniform("kernelValue", 27, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fSharpening3d:
      mFilter = ConvolutionFilter(27);
      mFilter.mKernelValues[0] = 0.0;
      mFilter.mKernelValues[1] = 0.0;
      mFilter.mKernelValues[2] = 0.0;
      mFilter.mKernelValues[3] = 0.0;
      mFilter.mKernelValues[4] = -1;
      mFilter.mKernelValues[5] = 0.0;
      mFilter.mKernelValues[6] = 0.0;
      mFilter.mKernelValues[7] = 0.0;
      mFilter.mKernelValues[8] = 0.0;
      mFilter.mKernelValues[9] = 0.0;
      mFilter.mKernelValues[10] = -1;
      mFilter.mKernelValues[11] = 0.0;
      mFilter.mKernelValues[12] = -1;
      mFilter.mKernelValues[13] = 6;
      mFilter.mKernelValues[14] = -1;
      mFilter.mKernelValues[15] = 0.0;
      mFilter.mKernelValues[16] = -1;
      mFilter.mKernelValues[17] = 0.0;
      mFilter.mKernelValues[18] = 0.0;
      mFilter.mKernelValues[19] = 0.0;
      mFilter.mKernelValues[20] = 0.0;
      mFilter.mKernelValues[21] = 0.0;
      mFilter.mKernelValues[22] = -1;
      mFilter.mKernelValues[23] = 0.0;
      mFilter.mKernelValues[24] = 0.0;
      mFilter.mKernelValues[25] = 0.0;
      mFilter.mKernelValues[26] = 0.0;
      mActiveShader = mConvolutionShader;
      pushShader(mActiveShader);
        mActiveShader.setUniform("texture", 0);
 			  mActiveShader.setUniform("offset", 27, &gOffset27[0]);
  			mActiveShader.setUniform("kernelSize", 27);
  			mActiveShader.setUniform("kernelValue", 27, mFilter.mKernelValues);
        mActiveShader.setUniform("scaleFactor", gScaleFactor);
        mActiveShader.setUniform("addOnBase", gAddOnBase);
        mActiveShader.setUniform("volumeSize", gVolumeSize);
        mActiveShader.setUniform("windowingMin", gWindowingMin);
        mActiveShader.setUniform("windowingMax", gWindowingMax);
      popShader();
      break;
    case fColorize:
      mActiveShader = mColorizeShader;
      pushShader(mActiveShader);
      mActiveShader.setUniform("texture", 0);
      mActiveShader.setUniform("base", gWindowingMin);
      mActiveShader.setUniform("treshold", gWindowingMax);
      popShader();
      break;
    default:
      break;
  }
  update();
}

//-----------------------------------------------------------------------------
Viewer::ConvolutionFilter::ConvolutionFilter(int iFilterSize) : 
  mFilterSize(iFilterSize)
{
  mKernelValues = new float[mFilterSize];
}

//-----------------------------------------------------------------------------
Viewer::ConvolutionFilter::~ConvolutionFilter()
{ delete[] mKernelValues; }

//-----------------------------------------------------------------------------
// MainDialog
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QDialog(),
  mpViewer(0)
{
  resize(800, 600);
  
  QVBoxLayout* pLyt = new QVBoxLayout(this);
  pLyt->setMargin(5);
  
  //ajout des boutons pour choisir la vue
  QHBoxLayout* pViewLyt = new QHBoxLayout(this);
  QButtonGroup* pViewGroup = new QButtonGroup(this);
  QRadioButton* pAxial = new QRadioButton(this);
  QRadioButton* pSagittal = new QRadioButton(this);
  QRadioButton* pCoronal = new QRadioButton(this);
  pAxial->setText("Axial");
  pSagittal->setText("Sagittal");
  pCoronal->setText("Coronal");
  pViewGroup->addButton(pAxial, vAxial);
  pViewGroup->addButton(pSagittal, vSagittal);
  pViewGroup->addButton(pCoronal, vCoronal);
  connect(pViewGroup, SIGNAL(buttonClicked(int)), 
    this, SLOT(viewChanged(int)));
  pViewLyt->addWidget(pAxial);
  pViewLyt->addWidget(pSagittal);
  pViewLyt->addWidget(pCoronal);
  pViewLyt->addStretch(1);
  pLyt->addLayout(pViewLyt);
  
  //ajout du visualiseur
  mpViewer = new Viewer(this);
  mpViewer->setCameraMode(Camera::ORTHOGONAL);
  mpViewer->setCameraOrientation(Camera::XY);
  pLyt->addWidget(mpViewer, 1);
  
  
  QHBoxLayout* pFilterLayout = new QHBoxLayout(this);
  //ajout d'un checkBox pour choisir le CT ou l'ultrason
  QCheckBox* pShowCtCB = new QCheckBox(this);
  pShowCtCB->setText("Show CT/US");
  pShowCtCB->setCheckState(Qt::Checked);
  connect(pShowCtCB, SIGNAL(stateChanged(int)), 
    this, SLOT(showCtChanged(int)));
  
  //ajout d'un drop box pour choisir le filtre
  QComboBox* pCombo = new QComboBox(this);
  pCombo->insertItem(Viewer::fNone, "aucun");
  pCombo->insertItem(Viewer::fMean, "moyenne");
  pCombo->insertItem(Viewer::fGausian, "gausien");
  pCombo->insertItem(Viewer::fEdgeDetection, "edge detection");
  pCombo->insertItem(Viewer::fSharpening, "sharpening");
  pCombo->insertItem(Viewer::fEdgeDetection3d, "edge detection 3d");
  pCombo->insertItem(Viewer::fSharpening3d, "sharpening 3d");
  pCombo->insertItem(Viewer::fColorize, "Colorize");
  connect(pCombo, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(filterTypeChanged(int)));
    
  //ajout du scale factor (0 - 10)
  QSlider* pSpinBox = new QSlider(Qt::Horizontal, this);
  pSpinBox->setMinimum(0);
  pSpinBox->setMaximum(100);
  connect(pSpinBox, SIGNAL(valueChanged(int)),
    this, SLOT(scaleFactorChanged(int)));
    
  //ajout du add on base
  QCheckBox* pCheckBox = new QCheckBox(this);
  pCheckBox->setText("Superposer");
  connect(pCheckBox, SIGNAL(stateChanged(int)), 
    this, SLOT(addOnBaseChanged(int)));
  
  pFilterLayout->addWidget(pShowCtCB);
  pFilterLayout->addWidget(pCombo, 1);
  pFilterLayout->addWidget(pSpinBox);
  pFilterLayout->addWidget(pCheckBox);
  pLyt->addLayout(pFilterLayout);
  
  //ajout du slider pour controler la tranche
  QSlider* pSlider = new QSlider(Qt::Horizontal, this);
  pSlider->setMinimum(0);
  pSlider->setMaximum(1000);
  connect(pSlider, SIGNAL(valueChanged(int)),
    this, SLOT(sliceChanged(int)));
  pLyt->addWidget(pSlider);
  
  //ajout de 2 sliders pour controller le windowing
  QGroupBox* pWindowingBox = new QGroupBox("Windowing", this);
  QVBoxLayout* pWindowingLyt = new QVBoxLayout(pWindowingBox);
  QSlider* pWindowingMin = new QSlider(Qt::Horizontal, pWindowingBox);
  pWindowingMin->setRange(0, 10000);
  QSlider* pWindowingMax = new QSlider(Qt::Horizontal, pWindowingBox);
  pWindowingMax->setRange(0, 10000);
  pWindowingMax->setValue(10000);
  connect(pWindowingMin, SIGNAL(valueChanged(int)),
    this, SLOT(windowingMinChanged(int)));
  connect(pWindowingMax, SIGNAL(valueChanged(int)),
    this, SLOT(windowingMaxChanged(int)));
  pWindowingLyt->addWidget(pWindowingMin);
  pWindowingLyt->addWidget(pWindowingMax);
  pLyt->addWidget(pWindowingBox);
  
  //déplace la camera
  Camera c = mpViewer->getCamera();
  Matrix4d m;
  m.setTranslation(Point3d(0.5, 0.5, 0.5));
  c.setTransformationToGlobal(m);
  mpViewer->setCamera(c, false);
}

//-----------------------------------------------------------------------------
void MainDialog::addOnBaseChanged(int iS)
{
  gAddOnBase = iS == Qt::Checked ? true : false;
  mpViewer->setFilterType(mpViewer->getFilterType());
}

//-----------------------------------------------------------------------------
void MainDialog::filterTypeChanged(int iType)
{ 
  mpViewer->setFilterType((Viewer::filterType)iType);
}

//-----------------------------------------------------------------------------
void MainDialog::scaleFactorChanged(int iValue)
{ 
  gScaleFactor = (float)iValue / 50.0;
  mpViewer->setFilterType(mpViewer->getFilterType());
}

//-----------------------------------------------------------------------------
void MainDialog::showCtChanged(int iS)
{
  gShowCT = iS == Qt::Checked ? true : false;
  if(gShowCT)
  {
    gVolumeSize.setXYZ(73, 512, 512);
    gSpacing.setXYZ(3.0, 0.91, 0.91);
  }
  else
  {
    gVolumeSize.setXYZ(277, 219, 247);
    gSpacing.setXYZ(1.0, 1.0, 1.0);
  }
  mpViewer->setFilterType(mpViewer->getFilterType());
}

//-----------------------------------------------------------------------------
void MainDialog::refreshTexCoord()
{
	switch (gView)
  {
    case vAxial:
      gTexCoord0 = Vector3d(gTexValue, 0.0, 0.0);
      gTexCoord1 = Vector3d(gTexValue, 0.0, 1.0);
      gTexCoord2 = Vector3d(gTexValue, 1.0, 1.0);
      gTexCoord3 = Vector3d(gTexValue, 1.0, 0.0);
      break;
    case vSagittal:
      gTexCoord0 = Vector3d(0.0, gTexValue, 0.0);
      gTexCoord1 = Vector3d(0.0, gTexValue, 1.0);
      gTexCoord2 = Vector3d(1.0, gTexValue, 1.0);
      gTexCoord3 = Vector3d(1.0, gTexValue, 0.0);
      break;
    case vCoronal:
      gTexCoord0 = Vector3d(0.0, 0.0, gTexValue);
      gTexCoord1 = Vector3d(0.0, 1.0, gTexValue);
      gTexCoord2 = Vector3d(1.0, 1.0, gTexValue);
      gTexCoord3 = Vector3d(1.0, 0.0, gTexValue);
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
void MainDialog::sliceChanged(int iValue)
{ 
  gTexValue = iValue / 1000.0;
  refreshTexCoord();
  mpViewer->update();
}

//-----------------------------------------------------------------------------
void MainDialog::viewChanged(int iId)
{
  gView = (view)iId;
	refreshTexCoord();
  
  Matrix4d rotation;
  switch (gView) 
  {
    case vAxial:
      gScale.setXYZ(gVolumeSize.getY() * gSpacing.getY(), gVolumeSize.getZ() * gSpacing.getZ(), 0.0);
      gOffset9[0] = Vector3f(0, -1, 1);
      gOffset9[1] = Vector3f(0, 0, 1);
      gOffset9[2] = Vector3f(0, 1, 1);
      gOffset9[3] = Vector3f(0, -1, 0);
      gOffset9[4] = Vector3f(0, 0, 0);
      gOffset9[5] = Vector3f(0, 1, 0);
      gOffset9[6] = Vector3f(0, -1, -1);
      gOffset9[7] = Vector3f(0, 0, -1);
      gOffset9[8] = Vector3f(0, 1, -1);
      break;
    case vSagittal:
      gScale.setXYZ(gVolumeSize.getX() * gSpacing.getX(), gVolumeSize.getZ() * gSpacing.getZ(), 0.0);
      gOffset9[0] = Vector3f(-1, 0, 1);
      gOffset9[1] = Vector3f(0, 0, 1);
      gOffset9[2] = Vector3f(1, 0, 1);
      gOffset9[3] = Vector3f(-1, 0, 0);
      gOffset9[4] = Vector3f(0, 0, 0);
      gOffset9[5] = Vector3f(1, 0, 0);
      gOffset9[6] = Vector3f(-1, 0, -1);
      gOffset9[7] = Vector3f(0, 0, -1);
      gOffset9[8] = Vector3f(1, 0, -1);
      break;
    case vCoronal:
      gScale.setXYZ(gVolumeSize.getX() * gSpacing.getX(), gVolumeSize.getY() * gSpacing.getY(), 0.0);
      gOffset9[0] = Vector3f(-1, 1, 0);
      gOffset9[1] = Vector3f(0, 1, 0);
      gOffset9[2] = Vector3f(1, 1, 0);
      gOffset9[3] = Vector3f(-1, 0, 0);
      gOffset9[4] = Vector3f(0, 0, 0);
      gOffset9[5] = Vector3f(1, 0, 0);
      gOffset9[6] = Vector3f(-1, -1, 0);
      gOffset9[7] = Vector3f(0, -1, 0);
      gOffset9[8] = Vector3f(1, -1, 0);
      break;
    default:
      break;
  }
  
  mpViewer->setFilterType(mpViewer->getFilterType());
}

//-----------------------------------------------------------------------------
void MainDialog::windowingMinChanged(int iValue)
{
	gWindowingMin = iValue / 10000.0;
  mpViewer->setFilterType(mpViewer->getFilterType());
}

//-----------------------------------------------------------------------------
void MainDialog::windowingMaxChanged(int iValue)
{
  gWindowingMax = iValue / 10000.0;
  mpViewer->setFilterType(mpViewer->getFilterType());
}
