// Copied from http://linuxgazette.net/issue74/tougher.html
// (only slightly modified)


// Definition of the Socket class


#ifndef Socket_class
#define Socket_class

#include "stdafx.h"

#include <sys/types.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <unistd.h>
#include <string>
//#include <arpa/inet.h>
#include <winsock2.h>

#include "socketexception.h"


const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
 public:
  Socket( std::string host, int port );
  virtual ~Socket();

  // Server initialization
  bool create();
  bool bind ( const int port );
  bool listen() const;
  bool accept ( Socket& ) const;

  // Client initialization
  bool connect ( const std::string host, const int port );

  // Data Transimission
  bool send ( char *buf, int length ) const;
  bool send ( const std::string ) const;
  int recv ( std::string& ) const;
  int recv ( char *buf, int maxlen ) const;


  void set_non_blocking ( const bool );

  bool is_valid() const { return m_sock != -1; }

 private:

  int m_sock;
  sockaddr_in m_addr;


};


#endif
