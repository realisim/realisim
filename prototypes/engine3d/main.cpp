
#include "Core.h"
#include "MainDialog.h"
#include <QApplication>
#include <QHBoxLayout>
#include "render/Renderer.h"


int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  engine3d::core::Core c;
  
  MainDialog m;
  m.show();
  
  //add the viewer to the main window
  QHBoxLayout *pLyt = new QHBoxLayout(m.centralWidget());
  pLyt->setMargin(0);
  
  engine3d::render::Renderer& v = c.getHub().getRenderer();
  pLyt->addWidget(&v);

  c.start();
	
  return app.exec();
}
