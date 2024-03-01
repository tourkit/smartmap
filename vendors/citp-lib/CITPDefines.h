#pragma once

#define CITP_RGB8       0x38424752 // RGB8


#define CITP_MULTICAST_IP   "224.0.0.180"
#define CITP_MULTICAST_PORT 4809

#include <cstring>
#include <array>

struct CITP_Header { 

  CITP_Header(uint32_t ContentType) : ContentType(ContentType) {}

  uint32_t      Cookie            = 0x50544943; // CITP    
  uint8_t       VersionMajor      = 0x01;       
  uint8_t       VersionMinor      = 0x00;                    
  uint8_t       Reserved[2]       = {0x00,0x00};       
  uint32_t      MessageSize       = 20;           // The size of the entire message, including this header.      
  uint16_t      MessagePartCount  = 0x01;         // Number of message fragments.      
  uint16_t      MessagePart       = 0x00;         // Index of this message fragment (0-based). 
  uint32_t      ContentType;                      // Cookie identifying the type of contents

}; 

struct CITP_PINF_Header { 

  CITP_Header   CITPHeader; 
  uint32_t      ContentType;                      // Cookie identifying the type of contents

  CITP_PINF_Header(uint32_t ContentType) : ContentType(ContentType), CITPHeader(0x464e4950) { CITPHeader.MessageSize += 4; }

}; 

struct CITP_PINF_PLoc {

  CITP_PINF_Header  CITPPINFHeader; 

  uint16_t          ListeningTCPPort;   
                                        
  char type_name_state[120];      
  size_t pos = 0;

  CITP_PINF_PLoc(const char *Type, const char *Name, const char *State) : CITPPINFHeader(0x636f4c50) {

    memset(&type_name_state[0],0,120);

    strcpy(&type_name_state[pos], Type);
    pos+=strlen(Type)+1;

    strcpy(&type_name_state[pos], Name);
    pos+=strlen(Name)+1;

    strcpy(&type_name_state[pos], State);
    pos+=strlen(State)+1;

    CITPPINFHeader.CITPHeader.MessageSize += pos+2;

  }
                                                                             
};

struct CITP_MSEX_Header {  

  CITP_Header CITPHeader;

  uint8_t VersionMajor = 0x01;
  uint8_t VersionMinor = 0x01;
  uint32_t ContentType;

  CITP_MSEX_Header(uint32_t ContentType) : ContentType(ContentType), CITPHeader(0x5845534d) { CITPHeader.MessageSize += 6; }

};

template<size_t SIZE>
struct CITP_MSEX_StFr { 

  CITP_MSEX_Header CITPMSEXHeader;

  uint16_t SourceIdentifier;
  uint32_t FrameFormat;
  uint16_t FrameWidth;
  uint16_t FrameHeight;
  uint16_t FrameBufferSize; 
  std::array<uint8_t, SIZE> FrameBuffer;

  CITP_MSEX_StFr(uint16_t SourceIdentifier, uint32_t FrameFormat, uint16_t FrameWidth, uint16_t FrameHeight) 
  
    : CITPMSEXHeader(0x72467453), SourceIdentifier(SourceIdentifier), FrameFormat(FrameFormat), FrameWidth(FrameWidth), FrameHeight(FrameHeight), FrameBufferSize(SIZE+1) {

      CITPMSEXHeader.CITPHeader.MessageSize += 12+FrameBufferSize;

      memset(&FrameBuffer[0],0,FrameBufferSize);

      if (SIZE == 12) {

        std::vector<uint8_t> TestFrameBuffer = {  

          255,0,0,
          0,255,0,
          0,0,255,
          255,255,255

        };

        memcpy(&FrameBuffer[0],&TestFrameBuffer[0],12);

      }

  }

};