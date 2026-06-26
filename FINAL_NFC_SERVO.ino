#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
#include <Servo.h>

SoftwareSerial SWSerial(3, 2); // RX, TX

PN532_SWHSU pn532swhsu(SWSerial);
PN532 nfc(pn532swhsu);

Servo doorServo;

String requiredTag = "142.66.169.128";
String tagId = "None";

byte nuidPICC[4];

const int servoPin = 9;
const int closedPosition = 90;
const int openPosition = 0 ;

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Hello Maker!");

  doorServo.attach(servoPin);
  doorServo.write(closedPosition);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (!versiondata)
  {
    Serial.println("Didn't Find PN53x Module");
    while (1);
  }

  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);

  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print(".");
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();

  Serial.println("Waiting for NFC tag...");
}

void loop()
{
  if (correctTagDetected())
  {
    Serial.println("Correct tag detected. Opening servo.");

    doorServo.write(openPosition);
    delay(10000);

    Serial.println("Closing servo.");
    doorServo.write(closedPosition);

    delay(1000);
  }
}

bool correctTagDetected()
{
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  for (byte i = 0; i < 4; i++)
  {
    nuidPICC[i] = 0;
  }

  boolean success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (!success)
  {
    return false;
  }

  Serial.print("UID Value: ");

  for (uint8_t i = 0; i < uidLength; i++)
  {
    if (i < 4)
    {
      nuidPICC[i] = uid[i];
    }

    Serial.print(" ");
    Serial.print(uid[i], DEC);
  }

  Serial.println();

  tagId = tagToString(nuidPICC);

  Serial.print("tagId is: ");
  Serial.println(tagId);

  if (tagId == requiredTag)
  {
    return true;
  }

  Serial.println("Wrong tag.");
  return false;
}

String tagToString(byte id[4])
{
  String tagId = "";

  for (byte i = 0; i < 4; i++)
  {
    if (i < 3)
    {
      tagId += String(id[i]) + ".";
    }
    else
    {
      tagId += String(id[i]);
    }
  }

  return tagId;
}