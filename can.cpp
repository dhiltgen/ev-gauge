//——————————————————————————————————————————————————————————————————————————————
//  ACAN2515Tiny Tiny Demo in loopback mode
//——————————————————————————————————————————————————————————————————————————————

#ifdef __AVR__
// TODO - once abstracted, create an else block...

#include <ACAN2515Tiny.h>
#include "can.h"
//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 connections:
//    - standard SPI pins for SCK, MOSI and MISO
//    - a digital output for CS
//    - interrupt input pin for INT
//——————————————————————————————————————————————————————————————————————————————
// If you use CAN-BUS shield (http://wiki.seeedstudio.com/CAN-BUS_Shield_V2.0/) with Arduino Uno,
// use B connections for MISO, MOSI, SCK, #9 or #10 for CS (as you want),
// #2 or #3 for INT (as you want).
//——————————————————————————————————————————————————————————————————————————————

static const byte MCP2515_CS  = 10 ; // CS input of MCP2515 (adapt to your design)
static const byte MCP2515_INT =  3 ; // INT output of MCP2515 (adapt to your design)

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2515Tiny can (MCP2515_CS, SPI, MCP2515_INT) ;

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Quartz: adapt to your design
//——————————————————————————————————————————————————————————————————————————————

static const uint32_t QUARTZ_FREQUENCY = 16UL * 1000UL * 1000UL ; // 16 MHz

//——————————————————————————————————————————————————————————————————————————————
//   SETUP
//——————————————————————————————————————————————————————————————————————————————

void can_setup () {
//--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
  Serial.begin (38400) ;
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
// TODO may need to call various SPI settings to use specific pins here..
//--- Define alternate pins for SPI0 (see https://www.pjrc.com/teensy/td_libs_SPI.html)
//   SPI.setMOSI (MCP2515_SI) ;
//   SPI.setMISO (MCP2515_SO) ;
//   SPI.setSCK (MCP2515_SCK) ;

//--- Begin SPI
  SPI.begin () ;
//--- Configure ACAN2515
  Serial.println ("Configure ACAN2515") ;
  ACAN2515TinySettings settings (QUARTZ_FREQUENCY, 125UL * 1000UL) ; // CAN bit rate 125 kb/s
  // settings.mRequestedMode = ACAN2515TinySettings::ListenOnlyMode ;
  const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Propagation Segment: ") ;
    Serial.println (settings.mPropagationSegment) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW: ") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Triple Sampling: ") ;
    Serial.println (settings.mTripleSampling ? "yes" : "no") ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
    switch (errorCode) {
        case ACAN2515Tiny::kNoMCP2515:
            Serial.println("no MCP");
            break;
        case ACAN2515Tiny::kTooFarFromDesiredBitRate:
            Serial.println("too far from dedired bitrate");
            break;
        case ACAN2515Tiny::kInconsistentBitRateSettings:
            Serial.println("inconsistent bitrate settings");
            break;
        case ACAN2515Tiny::kINTPinIsNotAnInterrupt:
            Serial.println("intp is not an interrupt");
            break;
        case ACAN2515Tiny::kISRIsNull:
            Serial.println("isri is null");
            break;
        case ACAN2515Tiny::kRequestedModeTimeOut:
            Serial.println("requested mode timeout");
            break;
        case ACAN2515Tiny::kAcceptanceFilterArrayIsNULL:
            Serial.println("acceptange filter array is null");
            break;
        case ACAN2515Tiny::kOneFilterMaskRequiresOneOrTwoAcceptanceFilters:
            Serial.println("one or two filters required");
            break;
        case ACAN2515Tiny::kTwoFilterMasksRequireThreeToSixAcceptanceFilters:
            Serial.println("three to six filters required");
            break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

//——————————————————————————————————————————————————————————————————————————————

void can_loop () {
  CANMessage frame ;
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    // const bool ok = can.tryToSend (frame) ;
    // if (ok) {
    //   gSentFrameCount += 1 ;
    //   Serial.print ("Sent: ") ;
    //   Serial.println (gSentFrameCount) ;
    // }else{
    //   Serial.println ("Send failure") ;
    // }
  }
  if (can.available ()) {
    can.receive (frame) ;
    gReceivedFrameCount ++ ;
    Serial.print ("Received: ") ;
    Serial.print (gReceivedFrameCount) ;
    Serial.print (" 0x") ;
    Serial.println (frame.id, HEX) ;
  }
  /*
  class CANMessage {
  public : uint32_t id = 0 ;  // Frame identifier
  public : bool ext = false ; // false -> standard frame, true -> extended frame
  public : bool rtr = false ; // false -> data frame, true -> remote frame
  public : uint8_t idx = 0 ;  // This field is used by the driver
  public : uint8_t len = 0 ;  // Length of data (0 ... 8)
  public : union {
    uint64_t data64        ; // Caution: subject to endianness
    int64_t  data_s64      ; // Caution: subject to endianness
    uint32_t data32    [2] ; // Caution: subject to endianness
    int32_t  data_s32  [2] ; // Caution: subject to endianness
    float    dataFloat [2] ; // Caution: subject to endianness
    uint16_t data16    [4] ; // Caution: subject to endianness
    int16_t  data_s16  [4] ; // Caution: subject to endianness
    int8_t   data_s8   [8] ;
    uint8_t  data      [8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
  } ;
} ;
*/
}

//——————————————————————————————————————————————————————————————————————————————

#endif