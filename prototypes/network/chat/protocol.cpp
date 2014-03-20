#include "protocol.h"
#include <qbytearray.h>
#include <QDataStream>
#include <QFile>
#include <QTcpSocket>

using namespace std;

namespace realisim
{
namespace prototypes
{
//-----------------------------------------------------------------------------
chatPeer from( const QByteArray& iBa )
{
	chatPeer r;
  quint16 protocol;
  QDataStream in( iBa );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  QByteArray t;
  in >> t;
  r = readPeerPacket( t ); //from
  return r;
}
//-----------------------------------------------------------------------------
QByteArray makeFilePacket( QFile& iF, const chatPeer& iFrom, 
	const chatPeer& iTo )
{
  QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint16)cpFile;
  out << makePeerPacket( iFrom );
  out << makePeerPacket( iTo );
  out << iF.fileName().section( "/", -1, -1 );
  out << iF.readAll();
  return r;
}

//-----------------------------------------------------------------------------
QByteArray makePeerListPacket( const vector< chatPeer >& iPeers )
{
	QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint16)cpPeerList;
  out << (quint32)iPeers.size();
  vector< chatPeer >::const_iterator it = iPeers.begin();
  for( ; it != iPeers.end(); ++it )
  {
  	out << it->getName();
    out << it->getAddress();
  }  
  return r;
}
//-----------------------------------------------------------------------------
QByteArray makePeerNamePacket( const QString& iS )
{
	QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint16)cpPeerName;
  out << iS.toUtf8();
  return r;
}
//-----------------------------------------------------------------------------
QByteArray makePeerPacket( const chatPeer& iP )
{
	QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << iP.getName().toUtf8();
  out << iP.getAddress().toUtf8();
  return r;
}

//-----------------------------------------------------------------------------
QByteArray makeTextPacket( const QString& iS, const chatPeer& iFrom,
  const chatPeer& iTo )
{
	QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint16)cpText;
  out << makePeerPacket( iFrom );
  out << makePeerPacket( iTo );
  out << iS.toUtf8();
  return r;
}
//-----------------------------------------------------------------------------
//retourne le nom du fichier qui vient d'Etre reçu
QByteArray readFilePacket( const QByteArray& iBa, QString& oFileName )
{
  QByteArray r;
  quint16 protocol;
  QDataStream in( iBa );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  if( (chatProtocol)protocol == cpFile )
  {
    QByteArray t;
    in >> t; //from
    in >> t; //to
    in >> oFileName; //filename
    in >> r; //contenu du fichier
  }
  return r;
}
//-----------------------------------------------------------------------------
vector< chatPeer > readPeerListPacket( const QByteArray& iA )
{
	vector< chatPeer > r;
  quint16 protocol;
  quint32 numberOfPeers;
  QDataStream in( iA );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  if( (chatProtocol)protocol == cpPeerList )
  {
    in >> numberOfPeers;
    QString name, address;
    for( unsigned int i = 0; i < numberOfPeers; ++i )
    {
    	in >> name;
      in >> address;
      r.push_back( chatPeer( name, address ) );
    }
  }
  return r;
}
//-----------------------------------------------------------------------------
QString readPeerNamePacket( const QByteArray& iA )
{
	QString r;
  quint16 protocol;
  QDataStream in( iA );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  if( (chatProtocol)protocol == cpPeerName )
  {
    QByteArray t;
    in >> t;
  	r = QString::fromUtf8( t, t.size() );
  }
  return r;
}
//-----------------------------------------------------------------------------
chatPeer readPeerPacket( const QByteArray& iBa )
{
	chatPeer r;
  QByteArray ba;
  QDataStream in( iBa );
  in.setVersion(QDataStream::Qt_4_7);
	in >> ba;
  r.setName( QString::fromUtf8( ba, ba.size() ) );
  in >> ba;
  r.setAddress( QString::fromUtf8( ba, ba.size() ) );
  return r;
}

//-----------------------------------------------------------------------------
chatProtocol readProtocol( const QByteArray& iBa )
{
	chatProtocol cp = cpUnknown;
  quint16 protocol;
  QDataStream in( iBa );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  cp = (chatProtocol)protocol;
	return cp;
}
//-----------------------------------------------------------------------------
QString readTextPacket( const QByteArray& iA )
{
  QString r;
  quint16 protocol;
  QDataStream in( iA );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  if( (chatProtocol)protocol == cpText )
  {
    QByteArray t;
    in >> t; // from
    in >> t; // to
    in >> t; // message
    r = QString::fromUtf8( t, t.size() );
  }
  return r;
}
//-----------------------------------------------------------------------------
chatPeer to( const QByteArray& iBa )
{
	chatPeer r;
  quint16 protocol;
  QDataStream in( iBa );
  in.setVersion(QDataStream::Qt_4_7);
  in >> protocol;
  QByteArray t;
  in >> t; //from
  in >> t; //to
  r = readPeerPacket( t );
  return r;
}

//-----------------------------------------------------------------------------
//--- chatPeer
//-----------------------------------------------------------------------------
chatPeer::chatPeer( QString iName, QString iAddress ) :
  mName( iName ),
  mAddress( iAddress )
{}

chatPeer::~chatPeer()
{}

}
}