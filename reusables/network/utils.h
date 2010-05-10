/* Created by Pierre-Olivier Beaudoin on 10-02-13. */
 
#ifndef REALISIM_REUSABLE_NETWORK_UTILS_HH
#define REALISIM_REUSABLE_NETWORK_UTILS_HH

#include <QAbstractSocket>
#include <qhostaddress.h>
#include <qlist.h>
#include <QNetworkInterface> 
#include <QStringList>

/*
*/
namespace realisim 
{
namespace reusables 
{
namespace network
{
  QStringList getLocalIpAddresses();
  QString asString(QAbstractSocket::SocketError);
  
  const int kProtocolVersion = 1;
  enum Protocol
  {
    pPeersListChanged = 0,
    pProtocolEnd
  };
}//network
}//reusable
}//realisim

#endif

 

