/**/

#include <3d/Camera.h>
#include <3d/Utilities.h>
#include <math/Point.h>
#include <math/Vect.h>
#include "MainDialog.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>


using namespace realisim;
using namespace math;
using namespace treeD;

Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
mBloomActivated(true),
mIntensityCutoff(0.5),
mHalfKernelSize(5)
{
	setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

void Viewer::draw()
{
	Widget3d::draw();

	if (mBloomActivated)
	{
		renderWithBloom();
	}
	else
		renderWithoutBloom();
}

void Viewer::initializeGL()
{
	Widget3d::initializeGL();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//open texture
	const QString cwd = QCoreApplication::applicationDirPath();
	QFileInfo fi(cwd+"/../assets/dusk.png");
	if( fi.exists() )
	{
		QImage i(fi.absoluteFilePath());
		mTexture.set(i);
	}

	//init mFbo
	mFbo.addColorAttachment(true);
	mFbo.addColorAttachment(true);
	mFbo.addColorAttachment(true);
	mFbo.getTexture(0).setMinificationFilter(GL_LINEAR);
	mFbo.getTexture(1).setMinificationFilter(GL_LINEAR);
	mFbo.getTexture(2).setMinificationFilter(GL_LINEAR);

	loadShaders();
}

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key())
	{
	case Qt::Key_F5: loadShaders(); break;
	default: break;
	}
	update();
}

//-----------------------------------------------------------------------------
void Viewer::loadShaders()
{
	const QString cwd = QCoreApplication::applicationDirPath();
  printf("cwd: %s\n", cwd.toStdString().c_str());
	//init shaders
	//intensity
	{
        mHighIntensityFilter.clear();
		QFile f(cwd + "/../assets/highIntensityFilter.frag");
		f.open(QIODevice::ReadOnly);
		QString content = f.readAll();
		mHighIntensityFilter.addFragmentSource(content);
	}

	//blur
	{
		mBlur.clear();
		QFile f(cwd + "/../assets/blur.frag");
		f.open(QIODevice::ReadOnly);
		QString content = f.readAll();
		mBlur.addFragmentSource(content);
	}

	//Combine
	{
		mCombine.clear();
		QFile f(cwd + "/../assets/combine.frag");
		f.open(QIODevice::ReadOnly);
		QString content = f.readAll();
		mCombine.addFragmentSource(content);
	}
}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
	Widget3d::paintGL();
}

//-----------------------------------------------------------------------------
void Viewer::renderWithBloom()
{
	const Camera& c = getCamera();
	const Vector2i viewportSize = c.getViewport().getSize();
	treeD::ScreenSpaceProjection ssp(viewportSize);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	mFbo.begin();
	{
		//perform high intensity cutoff
		mFbo.drawTo(0);
		glClear(GL_COLOR_BUFFER_BIT);

		pushShader(mHighIntensityFilter);
		{
			mHighIntensityFilter.setUniform("uTexture", 0);
			mHighIntensityFilter.setUniform("uCutoff", mIntensityCutoff);

			glBindTexture(GL_TEXTURE_2D, mTexture.getId());
			drawRectangle(Point2d(0.0, 0.0), Vector2d(viewportSize));
		}
		popShader();
		
		//perform blur
		pushShader(mBlur);
		{
			////perform blurX
			mFbo.drawTo(1);
			glClear(GL_COLOR_BUFFER_BIT);

			Texture t0 = mFbo.getTexture(0);
			mBlur.setUniform("uTexture", 0);
			mBlur.setUniform("uTextureSize", Vector2i(t0.size()[0], t0.size()[1]));
			mBlur.setUniform("uHorizontal", 1);
			mBlur.setUniform("uVertical", 0);
			mBlur.setUniform("uHalfKernel", mHalfKernelSize);

			glBindTexture(GL_TEXTURE_2D, t0.getId());
			drawRectangle(Point2d(0.0, 0.0), Vector2d(viewportSize));


			////perform blurY
			mFbo.drawTo(2);
			glClear(GL_COLOR_BUFFER_BIT);

			Texture t1 = mFbo.getTexture(1);
			mBlur.setUniform("uHorizontal", 0);
			mBlur.setUniform("uVertical", 1);

			glBindTexture(GL_TEXTURE_2D, t1.getId());
			drawRectangle(Point2d(0.0, 0.0), Vector2d(viewportSize));
		}
		popShader();
	}
	mFbo.end();

	//blit final result to screen and combine with bloom
	//const Texture& finalTexture = mFbo.getTexture(0);
	pushShader(mCombine);
	{
		glBindTexture(GL_TEXTURE_2D, mTexture.getId());

		Texture t2 = mFbo.getTexture(2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, t2.getId());

		mCombine.setUniform("uTexture0", 0);
		mCombine.setUniform("uTexture1", 1);
		drawRectangle( Point2d(0.0, 0.0), Vector2d(viewportSize));

		glBindTexture(GL_TEXTURE_2D, 0);
		
		glActiveTexture(GL_TEXTURE0);
		
	}
	popShader();

	//display little vignettes of fbo contents
	{
		int cx = 0, cy = 0;
		const Vector2i vignetteSize = c.getViewport().getSize() / 4;
		for (int i = 0; i < mFbo.getNumColorAttachment(); ++i)
		{
			glBindTexture(GL_TEXTURE_2D, mFbo.getTexture(i).getId());

			drawRectangle(Point2d(cx, cy), vignetteSize);
			cx += vignetteSize.x() + 2;
		}
	}
}

//-----------------------------------------------------------------------------
void Viewer::renderWithoutBloom()
{
	const Camera& c = getCamera();
	const Vector2i viewportSize = c.getViewport().getSize();
	treeD::ScreenSpaceProjection ssp(viewportSize);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, mTexture.getId());
	drawRectangle(Point2d(0.0, 0.0), Vector2d(viewportSize));
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
mpViewer(0),
mCurrentImageIndex(0)
{
	//fill in images
	const QString cwd = QCoreApplication::applicationDirPath();
	mImagesFilePath.push_back(cwd + "/../assets/dusk.png");
	mImagesFilePath.push_back(cwd + "/../assets/dawn.png");
	mImagesFilePath.push_back(cwd + "/../assets/night.png");

	resize(800, 600);

	QWidget *pCentralWidget = new QWidget(this);
	setCentralWidget(pCentralWidget);

	QHBoxLayout* pLyt = new QHBoxLayout(pCentralWidget);
	pLyt->setMargin(5);
	{
		QVBoxLayout *pControlLyt = new QVBoxLayout();
		{
			mpActivateBloom = new QCheckBox("activate bloom", pCentralWidget);
			connect(mpActivateBloom, SIGNAL(clicked()), this, SLOT(activateBloomClicked()));

			//intensity cutoff
			QHBoxLayout *pLyt0 = new QHBoxLayout();
			{
				QLabel *l = new QLabel("intensity cutoff", pCentralWidget);
				mpIntensityCutoffSlider = new QSlider(pCentralWidget);
				mpIntensityCutoffSlider->setRange(0, 100);
				mpIntensityCutoffSlider->setTickInterval(1);
				mpIntensityCutoffSlider->setOrientation(Qt::Horizontal);
				connect(mpIntensityCutoffSlider, SIGNAL(valueChanged(int)),
					this, SLOT(intensityCutoffChanged(int)));

				pLyt0->addWidget(l);
				pLyt0->addWidget(mpIntensityCutoffSlider);
				pLyt0->addStretch(1);
			}

			//halfKernelSize
			QHBoxLayout *pLyt1 = new QHBoxLayout();
			{
				QLabel *l = new QLabel("half kernel size", pCentralWidget);
				mpHalfKernelSlider = new QSlider(pCentralWidget);
				mpHalfKernelSlider->setRange(0, 50);
				mpHalfKernelSlider->setTickInterval(1);
				mpHalfKernelSlider->setOrientation(Qt::Horizontal);
				connect(mpHalfKernelSlider, SIGNAL(valueChanged(int)),
					this, SLOT(halfKernelChanged(int)));

				pLyt1->addWidget(l);
				pLyt1->addWidget(mpHalfKernelSlider);
				pLyt1->addStretch(1);
			}

			//nextImage
			QHBoxLayout *pLyt2 = new QHBoxLayout();
			{
				mpNextImage = new QPushButton("next image", pCentralWidget);
				connect(mpNextImage, SIGNAL(clicked()), this, SLOT(nextImageClicked()));

				pLyt2->addWidget(mpNextImage);
				pLyt2->addStretch(1);
			}

			pControlLyt->addWidget(mpActivateBloom);
			pControlLyt->addLayout(pLyt0);
			pControlLyt->addLayout(pLyt1);
			pControlLyt->addLayout(pLyt2);
			pControlLyt->addStretch(1);
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
void MainDialog::activateBloomClicked()
{
	mpViewer->mBloomActivated = mpActivateBloom->isChecked();
	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::halfKernelChanged(int iV)
{
	mpViewer->mHalfKernelSize = iV;
	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::intensityCutoffChanged(int iV)
{
	mpViewer->mIntensityCutoff = iV / 100.0;
	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::nextImageClicked()
{
	mCurrentImageIndex = (mCurrentImageIndex + 1) % (int)mImagesFilePath.size();
	QFileInfo fi(mImagesFilePath[mCurrentImageIndex]);
	if (fi.exists())
	{
		QImage i(fi.absoluteFilePath());
		mpViewer->mTexture.set(i);
	}
	updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	mpActivateBloom->setChecked(mpViewer->mBloomActivated);
	mpIntensityCutoffSlider->setValue(mpViewer->mIntensityCutoff * 100);
	mpHalfKernelSlider->setValue(mpViewer->mHalfKernelSize);

	update();
	mpViewer->update();
}