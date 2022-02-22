#include "DFMiniMp3.h"
#include <SoftwareSerial.h>

#define _DEBUG (1)

#define RX_PIN (10)
#define TX_PIN (11)
#define SHOT_PIN (12)
#define GUN_PIN (7)

#define PLAY_ENGINE_SOUND() (dfmp3.playFolderTrack16(1, 1))
#define PLAY_SHOT_SOUND() (dfmp3.playFolderTrack16(2, 1))

class Mp3Notify;
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
DfMp3 dfmp3(mySerial);

void setup()
{
#ifdef _DEBUG
  Serial.begin(115200);
  delay(1000 * 3);
  Serial.println("===== Start Setup =====");
#endif

  pinMode(GUN_PIN, OUTPUT);
  pinMode(SHOT_PIN, INPUT_PULLUP);

  dfmp3.begin(9600, 1000);
  dfmp3.reset();

  dfmp3.setVolume(20);
  waitMilliseconds(100);

  PLAY_ENGINE_SOUND();
  waitMilliseconds(1000 * 3);

#ifdef _DEBUG
  Serial.println("----- Setup end -----");
#endif
}

void loop()
{
// #ifdef _DEBUG
//   Serial.println("----- Loop start -----");
// #endif

  if (digitalRead(SHOT_PIN) == LOW) {
#ifdef _DEBUG
    Serial.println("Fire!");
#endif
    fire();
  }

// #ifdef _DEBUG
//   Serial.println("Wait...");
// #endif
  waitMilliseconds(100);

// #ifdef _DEBUG
//   Serial.println("----- Loop end -----");
// #endif
}

void fire() {
// #ifdef _DEBUG
//   Serial.println("Play 1");
// #endif
  PLAY_SHOT_SOUND();

  for(int i = 0; i < 10; i ++) {
    digitalWrite(GUN_PIN, HIGH);
    waitMilliseconds(50);
    digitalWrite(GUN_PIN, LOW);
    waitMilliseconds(50);
  }

// #ifdef _DEBUG
//   Serial.println("Play 2(Loop)");
// #endif
  PLAY_ENGINE_SOUND();
}

void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();

  while ((millis() - start) < msWait)
  {
    // if you have loops with delays, its important to
    // call dfmp3.loop() periodically so it allows for notifications
    // to be handled without interrupts
    dfmp3.loop();
    delay(1);
  }
}

//----------------------------------------------------------------------------------
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char *action)
  {
#ifdef _DEBUG
    if (source & DfMp3_PlaySources_Sd)
    {
      Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb)
    {
      Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash)
    {
      Serial.print("Flash, ");
    }
    Serial.println(action);
#endif
  }
  static void OnError(DfMp3 &mp3, uint16_t errorCode)
  {
#ifdef _DEBUG
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    switch (errorCode)
    {
    case DfMp3_Error_Busy:
      Serial.println("Busy");
      break;
    case DfMp3_Error_Sleeping:
      Serial.println("Sleeping");
      break;
    case DfMp3_Error_SerialWrongStack:
      Serial.println("Serial Wrong Stack");
      break;

    case DfMp3_Error_RxTimeout:
      Serial.println("Rx Timeout!!!");
      break;
    case DfMp3_Error_PacketSize:
      Serial.println("Wrong Packet Size!!!");
      break;
    case DfMp3_Error_PacketHeader:
      Serial.println("Wrong Packet Header!!!");
      break;
    case DfMp3_Error_PacketChecksum:
      Serial.println("Wrong Packet Checksum!!!");
      break;

    default:
      Serial.println(errorCode, HEX);
      break;
    }
#endif
  }
  static void OnPlayFinished(DfMp3 &mp3, DfMp3_PlaySources source, uint16_t track)
  {
#ifdef _DEBUG
    Serial.print("Play finished for #");
    Serial.println(track);
#endif
    if (track == 2) {
      PLAY_ENGINE_SOUND();
    }
  }
  static void OnPlaySourceOnline(DfMp3 &mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3 &mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3 &mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};