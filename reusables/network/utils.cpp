/* Created by Pierre-Olivier Beaudoin on 10-02-13. */

#include "network/utils.h"
#include <QDataStream>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>

using namespace std;

namespace realisim 
{
namespace reusables 
{
namespace network
{

//------------------------------------------------------------------------------
QString asString(QAbstractSocket::SocketError iError)
{
  QString r("Undefined network error occured.");
  
  switch (iError) 
  {
    case QAbstractSocket::ConnectionRefusedError :
    {r = "The connection was refused by the peer (or timed out)."; break;}
    case QAbstractSocket::RemoteHostClosedError :
    {r = "The remote host closed the connection."; break;}
		case QAbstractSocket::HostNotFoundError :
    {r = "The host address was not found."; break;}
    case QAbstractSocket::SocketAccessError :
    {r = "The socket operation failed because the application lacked the required privileges."; break;}
    case QAbstractSocket::SocketResourceError :
    {r = "The local system ran out of resources (e.g., too many sockets)."; break;}
    case QAbstractSocket::DatagramTooLargeError :
    {r = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)."; break;}
    case QAbstractSocket::NetworkError :
    {r = "An error occurred with the network (e.g., the network cable was accidentally plugged out)."; break;}
    case QAbstractSocket::AddressInUseError :
    {r = "The address specified to QUdpSocket::bind() is already in use and was set to be exclusive."; break;}
    case QAbstractSocket::SocketAddressNotAvailableError :
    {r = "The address specified to QUdpSocket::bind() does not belong to the host."; break;}
    case QAbstractSocket::UnsupportedSocketOperationError :
    {r = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)."; break;}
    case QAbstractSocket::ProxyAuthenticationRequiredError :
    {r = "The socket is using a proxy, and the proxy requires authentication."; break;}
    case QAbstractSocket::SslHandshakeFailedError :
    {r = "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)"; break;}
    case QAbstractSocket::UnfinishedSocketOperationError :
    {r = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)."; break;}
    case QAbstractSocket::ProxyConnectionRefusedError :
    {r = "Could not contact the proxy server because the connection to that server was denied"; break;}
    case QAbstractSocket::ProxyConnectionClosedError :
    {r = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)"; break;}
    case QAbstractSocket::ProxyConnectionTimeoutError :
    {r = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase."; break;}
    case QAbstractSocket::ProxyNotFoundError :
    {r = "The proxy address set with setProxy() (or the application proxy) was not found."; break;}
    case QAbstractSocket::ProxyProtocolError :
    {r = "The connection negotiation with the proxy server because the response from the proxy server could not be understood."; break;}
    case QAbstractSocket::UnknownSocketError :
    {r = "An unidentified error occurred."; break;}

    default: r = "Unknown unidentified out of no where error occured..."; break;
  }
  return r;
}

//------------------------------------------------------------------------------
QStringList getLocalIpAddresses()
{
  QStringList result;
  QList<QHostAddress> l = QNetworkInterface::allAddresses();
  QList<QHostAddress>::iterator it = l.begin();
  for(; it != l.end(); ++it)
  {
    result.push_back((*it).toString());
  }
    
  return result;
}

//------------------------------------------------------------------------------
QByteArray makePacket( const QByteArray& iPayload )
{
	QByteArray packet;
  QDataStream out(&packet, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint32)0;
  out << iPayload;
  out.device()->seek(0);
  out << (quint32)(packet.size() - sizeof(quint32));
	return packet;
}

//------------------------------------------------------------------------------
QByteArray makeUploadHeader( const QByteArray& iUpload )
{
	QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_7);
  out << (quint32)0xABAB0506; //magic header
  out << (quint16)1; //version
  out << (quint32)iUpload.size();
  return r;
}

//------------------------------------------------------------------------------
void printAsHex( const QByteArray& iA )
{
  for( int i = 0; i < iA.size(); ++i )
    printf("%02X", (uchar)iA.at( i ) );
  printf("\n");
}

//------------------------------------------------------------------------------
QByteArray readPacket( QTcpSocket* iS )
{
	QByteArray r;
  if( iS->bytesAvailable() > (int) sizeof( quint32 ) )
  {
  	bool read = false;
    quint32 packetSize;
    QDataStream in(iS);
    in.setVersion(QDataStream::Qt_4_7);
    in >> packetSize;
    while( !read )
    {
    	if( iS->bytesAvailable() < packetSize )
      	iS->waitForReadyRead( 1000 );
      else
      { in >> r; read = true; printf( "read packet size: %d\n", packetSize ); }
    }
  }
  return r;
}

//------------------------------------------------------------------------------
Transfer readUploadHeader( const QByteArray& iA )
{
	Transfer r;
  QDataStream in( iA );
  in.setVersion(QDataStream::Qt_4_7);
  quint32 magicNumber;
  in >> magicNumber;
  if( magicNumber == 0xABAB0506 )
  {
  	in >> r.mVersion;
    in >> r.mTotalSize;
    r.mIsValid = true;
  }
  return r;
}

//------------------------------------------------------------------------------
//--- Transfer
//------------------------------------------------------------------------------
Transfer::Transfer() :
	mIsValid( false ),
	mVersion( 1 ),
  mTotalSize( 0 ),
  mPayload()
{}

Transfer::~Transfer()
{}


} //network
} //reusables
} //realisim
