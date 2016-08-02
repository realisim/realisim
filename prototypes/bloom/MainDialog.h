/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"

#include <QCheckbox>
#include <QKeyEvent>
#include <QSlider>
#include <QMainWindow>
#include <QpushButton>
#include <vector>


class Viewer : public realisim::treeD::Widget3d
{
	friend class MainDialog;

public:
	Viewer(QWidget*);
	~Viewer();

private:
	virtual void draw() override;
	virtual void keyPressEvent(QKeyEvent*);
	virtual void loadShaders();
	virtual void initializeGL() override;
	virtual void paintGL() override;
	void renderWithBloom();
	void renderWithoutBloom();
	virtual void resizeGL(int, int) override;

	realisim::treeD::Texture mTexture;
	realisim::treeD::FrameBufferObject mFbo;
	realisim::treeD::Shader mHighIntensityFilter;
	realisim::treeD::Shader mBlur;
	realisim::treeD::Shader mCombine;

	bool mBloomActivated;
	double mIntensityCutoff;
	int mHalfKernelSize;
};


class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog() {};

protected slots:
	void activateBloomClicked();
	void halfKernelChanged(int);
	void intensityCutoffChanged(int);
	void nextImageClicked();

protected:
	void updateUi();

	//--- ui
	Viewer* mpViewer;
	QCheckBox* mpActivateBloom;
	QSlider *mpIntensityCutoffSlider;
	QSlider *mpHalfKernelSlider;
	QPushButton *mpNextImage;

	//--- data
	int mCurrentImageIndex;
	std::vector<QString> mImagesFilePath;
};

#endif
