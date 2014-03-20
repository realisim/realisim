
#ifndef REALISIM_PROTOTYPE_CHAT_PROTOCOLE_H
#define REALISIM_PROTOTYPE_CHAT_PROTOCOLE_H

#include <map>
class QByteArray;
class QFile;
#include <QString>
#include <vector>
#ifdef _WIN32
  #include <time.h>
#endif

namespace realisim 
{ 
namespace prototypes
{

enum chatProtocol{ cpUnknown, cpPeerList, cpText, cpRequestToSendFile,
	cpAcceptFile, cpFile, cpPeerName };

class chatPeer
{
	public:
  	chatPeer() : mName( QString("n/a") ), mAddress( QString("n/a") ) {}
  	chatPeer( QString, QString );
    chatPeer( const chatPeer& iP) : mName( iP.getName() ),
    	mAddress( iP.getAddress() ) {}
    chatPeer& operator=( const chatPeer& iP ) { mName = iP.getName(); 
    	mAddress = iP.getAddress(); return *this; }
    bool operator==( const chatPeer& iP ) const { return mName == iP.getName() &&
    	mAddress == iP.getAddress(); }
    
    virtual ~chatPeer();
    
    virtual QString getAddress() const { return mAddress; }
    virtual QString getName() const { return mName; }
    virtual void setName( const QString& iS ) { mName = iS; }
    virtual void setAddress( const QString& iA ) { mAddress = iA; }
    
  protected:
    QString mName;
    QString mAddress;
};

chatPeer from( const QByteArray& );
QByteArray makeFilePacket( QFile&, const chatPeer&, const chatPeer& );
QByteArray makePeerPacket( const chatPeer& );
QByteArray makePeerListPacket( const std::vector< chatPeer >& );
QByteArray makePeerNamePacket( const QString& );
QByteArray makeTextPacket( const QString&, const chatPeer&, const chatPeer& );
QByteArray readFilePacket( const QByteArray& iBa, QString& oFileName );
std::vector< chatPeer > readPeerListPacket( const QByteArray& );
QString readPeerNamePacket( const QByteArray& );
chatPeer readPeerPacket( const QByteArray& );
chatProtocol readProtocol( const QByteArray& );
QString readTextPacket( const QByteArray& );
chatPeer to( const QByteArray& );

}
}

#endif