

#ifndef realisim_sargam_updater_hh
#define realisim_sargam_updater_hh

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace realisim
{
namespace sargam
{
  
class Updater : public QObject
{
  Q_OBJECT
public:
  Updater(QObject* = 0);
  virtual ~Updater();
  
  void checkForUpdate();
  
protected slots:
  void replyFinished(QNetworkReply*);
  
protected:
  QNetworkAccessManager* mpAccess;
  
};

  
  
}
}


#endif
