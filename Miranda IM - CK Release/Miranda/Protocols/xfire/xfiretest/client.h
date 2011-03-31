/*
 *  xfirelib - C++ Library for the xfire protocol.
 *  Copyright (C) 2006 by
 *          Beat Wolf <asraniel@fryx.ch> / http://gfire.sf.net
 *          Herbert Poul <herbert.poul@gmail.com> / http://goim.us
 *    http://xfirelib.sphene.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __CLIENT_H
#define __CLIENT_H

#include <string>
#include <pthread.h>
#include "packetreader.h"
#include "xfirepacketcontent.h"
#include "packetlistener.h"
#include "buddylist.h"
#include "xfiregameresolver.h"

namespace xfirelib {
  struct BuddyList;

class Client : public PacketListener {
 public:
  Client();
  ~Client();
  void connect(std::string username, std::string password);
  /**
   * Sends a XFirePacketContent (does NOT delete content)
   * Returns true if sent successfully.
   */
  bool send(XFirePacketContent *content);

  BuddyList *getBuddyList() { return buddyList; }
  void addPacketListener(PacketListener *packetListener);
  void disconnect();
  void sendMessage(string username, string message);
  void sendNickChange(string nick);
  XFireGameResolver *getGameResolver();
  void setGameResolver(XFireGameResolver *resolver) {
    delete this->gameResolver;
    this->gameResolver = resolver;
  }

  BOOL gotBudduyList;
  BOOL connected;

 protected:
  void receivedPacket( XFirePacket *packet );
  void startThreads();
  static void *startReadThread(void *ptr);
  static void *startSendPingThread(void *ptr);
 private:
  XFireGameResolver *gameResolver;
  PacketReader *packetReader;
  std::string *username;
  std::string *password;
  Socket *socket;
  BuddyList *buddyList;
  pthread_t readthread;
  pthread_t sendpingthread;
  int protocolVersion;
};

};

#endif
