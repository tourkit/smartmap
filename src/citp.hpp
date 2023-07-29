
#pragma once

#include "globals.hpp"
#include "include/vendor/citp-lib/CITPDefines.h"

#include <winsock2.h>
#include <ws2tcpip.h>

class MulticastSocket {
public:
  MulticastSocket(const char* ip, int port) {
    // Constructor initializes socket

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) std::cout << "error UDP 1" << std::endl; 
    
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd == INVALID_SOCKET) std::cout << "error UDP 2" << std::endl; 
    
    // Join multicast group
    mreq.imr_multiaddr.s_addr = inet_addr(ip);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR) std::cout << "error UDP 4" << std::endl; 

    // bind to NIC
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    bind(socketfd, (SOCKADDR*)&addr, sizeof(addr));
    // if (bind(socketfd, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) std::cout << "error UDP 3: " << WSAGetLastError() << std::endl; 
    
  }

  ~MulticastSocket() {
    
    closesocket(socketfd);
    WSACleanup();

  }

  void receive(char* buf, int len) {

    if (recvfrom(socketfd, buf, len, 0, (SOCKADDR*)&addr, &addrlen) == SOCKET_ERROR) std::cout << "error UDP 5" << std::endl; 

  }

  void send(const char* buf, int len) {

    if (sendto(socketfd, buf, len, 0, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) std::cout << "error UDP 6" << std::endl; 

  }

private:

  SOCKET socketfd;
  SOCKADDR_IN addr;
  ip_mreq mreq;
  int addrlen; 

};

struct CITP {

    CITP_MSEX_StFr<12> frame;

    CITP_PINF_PLoc ploc;

    MulticastSocket *socket;

    CITP(uint16_t width, uint16_t height) 
    
      : frame(0x01, CITP_RGB8, 0x02 ,0x02), ploc("Visualizer", "SmartMapCITP", "BeingAwesome") {
      // : frame(0x00, CITP_RGB8, 0x02 ,0x02), ploc("LightingConsole", "Sistom", "Moster") {

        socket = new MulticastSocket(CITP_MULTICAST_IP, CITP_MULTICAST_PORT);

        
    }

    void send() {

        socket->send((const char*)&ploc, ploc.CITPPINFHeader.CITPHeader.MessageSize);
        socket->send((const char*)&frame, frame.CITPMSEXHeader.CITPHeader.MessageSize);

    }

};

