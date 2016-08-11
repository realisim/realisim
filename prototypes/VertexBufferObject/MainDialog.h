/*
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <3d/Shader.h>
#include <3d/Texture.h>
#include <3d/VertexBufferObject.h>
#include <3d/Widget3d.h>

#include <QKeyEvent>
#include <QMainWindow>
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
	virtual void loadModels();
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int, int) override;

	realisim::treeD::Texture mTexture;
	realisim::treeD::FrameBufferObject mFbo;
  realisim::treeD::VertexBufferObject mVbo;
};


class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog() {};

protected slots:

protected:
	void updateUi();

	//--- ui
	Viewer* mpViewer;

	//--- data
};

#endif
