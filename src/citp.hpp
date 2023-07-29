
#pragma once
#include <iostream>
#include "../include/vendor/citp-lib/CITPDefines.h"

#include <winsock2.h>
#include <ws2tcpip.h>


class MulticastSocket {
public:
  MulticastSocket(const char* ip, int port) {
    // Constructor initializes socket

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
      std::cout << "error UDP 1" << std::endl; 
    }

    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd == INVALID_SOCKET) {
      std::cout << "error UDP 2" << std::endl; 
    }

    // Bind to multicast address/port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("2.0.0.222");


    if (bind(socketfd, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
      std::cout << "error UDP 3: " << WSAGetLastError() << std::endl; 
    }

    // Join multicast group
    mreq.imr_multiaddr.s_addr = inet_addr(ip);
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
      (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR) {
        std::cout << "error UDP 4" << std::endl; 
    }
  }

  ~MulticastSocket() {
    // Destructor cleans up
    
    closesocket(socketfd);
    WSACleanup();
  }

  void receive(char* buf, int len) {
    // Receive multicast packets

    if (recvfrom(socketfd, buf, len, 0, (SOCKADDR*)&addr, &addrlen) == SOCKET_ERROR) {
      std::cout << "error UDP 5" << std::endl; 
    }
  }

  void send(const char* buf, int len) {
    // Send multicast packets 

    if (sendto(socketfd, buf, len, 0, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
      std::cout << "error UDP 6" << std::endl; 
    }
  }

private:
  SOCKET socketfd;
  SOCKADDR_IN addr;
  ip_mreq mreq;
  int addrlen; 


};


