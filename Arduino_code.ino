#include <SPI.h>
#include <MFRC522.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#if (defined(_AVR) || defined(ESP8266)) && !defined(AVR_ATmega2560_)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
// too much serialprint causes issues of memorry
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C mylcd(0x26, lcdColumns, lcdRows);
SoftwareSerial mySerial(8, 3); // RX, TX

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


/*
  rfid PinWiring to Arduino Uno

  SDA------------------------Digital 10

  SCK------------------------Digital 13

  MOSI----------------------Digital 11

  MISO----------------------Digital 12

  IRQ------------------------unconnected

  GND-----------------------GND

  RST------------------------Digital 9

  3.3V------------------------3.3V (DO NOT CONNECT TO 5V)  *
*/

//const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = A5, d7 = 2;
//LiquidCrystal mylcd(rs, en, d4, d5, d6, d7);
#define ss 10
#define rst 9
int finger_id = 1;
int enroll_btn = A1;
int esp = A2;
int buzz = A0;
int Slock = 4;
//int Slock = A4;
String uid = "000";
int id = 1;
int count = 0;
//int state = 0;

int randoms;
int progress = 3;
/* Custom characters */
byte START_DIV_0_OF_1[] = {
  B01111,
  B11000,
  B10000,
  B10000,
  B10000,
  B10000,
  B11000,
  B01111
}; // Char beginning 0 / 1

byte START_DIV_1_OF_1[] = {
  B01111,
  B11000,
  B10011,
  B10111,
  B10111,
  B10011,
  B11000,
  B01111
}; // Char beginning 1 / 1

byte DIV_0_OF_2[] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
}; // Char Middle 0 / 2

byte DIV_1_OF_2[] = {
  B11111,
  B00000,
  B11000,
  B11000,
  B11000,
  B11000,
  B00000,
  B11111
}; // Char Middle 1 / 2

byte DIV_2_OF_2[] = {
  B11111,
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B00000,
  B11111
}; // Char Middle 2 / 2

byte END_DIV_0_OF_1[] = {
  B11110,
  B00011,
  B00001,
  B00001,
  B00001,
  B00001,
  B00011,
  B11110
}; // Char End 0 / 1

byte END_DIV_1_OF_1[] = {
  B11110,
  B00011,
  B11001,
  B11101,
  B11101,
  B11001,
  B00011,
  B11110
}; // Char End 1 / 1



MFRC522 mfrc522(ss, rst);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  SPI.begin();         // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522 RFID reader
  mylcd.init();
  mylcd.backlight();
  mylcd.print("----------------");
  //  mylcd.createChar(0, Bell);
  mylcd.createChar(1, START_DIV_1_OF_1);//{|
  mylcd.createChar(2, DIV_0_OF_2);//=
  mylcd.createChar(3, DIV_1_OF_2);// |
  mylcd.createChar(4, DIV_2_OF_2);//  |
  mylcd.createChar(5, END_DIV_0_OF_1);//}
  mylcd.createChar(6, END_DIV_1_OF_1);// |}


  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    mylcd.print("SYSTEM OK!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    mylcd.print("FINGER SENSOR ERR");
    while (1) {
      delay(1);
    }
  }

  mylcd.clear();
  mylcd.setCursor(0, 0);
  mylcd.print("SYSTEM LOADING::");
  randoms = random(500, 1500);
  mylcd.setCursor(0, 1);
  mylcd.write(1);
  for (int i = 1; i < 10; i++) {
    int load = 0 + i;
    mylcd.setCursor(load, 1);
    mylcd.write(2);
  }
  mylcd.write(5);
  delay(randoms);

  Serial.println("LOADING");
  for (int i = 1; i < 11; i++) {
    int cursor = i;
    //    Serial.println(i);
    //progress++;
    randoms = random(100, 300);
    //  randoms = random(100, 500);
    Serial.print("#");
    delay(randoms);
    mylcd.setCursor(cursor, 1);
    mylcd.write(progress);
    delay(randoms);
    // Serial.print(randoms);
    if (progress == 3) {
      progress = 4;
    }
    else {
      progress = 3;
    }
    mylcd.setCursor(cursor, 1);
    mylcd.write(progress);

    progress = 3;

    mylcd.setCursor(12, 1);
    int status = map(i, 0, 10, 0, 100);
    mylcd.print(status);
    mylcd.print("%");
    if (i == 10) {
      mylcd.setCursor(cursor, 1);
      mylcd.write(6);
      delay(2000);
    }
  }

//finger.getParameters();
//  Serial.println(F("Reading sensor parameters"));
//  
//  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
//  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
//  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
//  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
//  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
//  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
//  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  pinMode(enroll_btn, INPUT);
  pinMode(esp, INPUT);
  pinMode(buzz, OUTPUT);
  pinMode(Slock, OUTPUT);
  digitalWrite(enroll_btn, HIGH );
  digitalWrite(Slock, LOW );
  Serial.println("Ready to read RFID tags...");
  digitalWrite(buzz, HIGH);
  mylcd.clear();
  mylcd.print("--SYSTEM READY--");
  //digitalWrite(led, LOW);
  delay(1000);
  digitalWrite(buzz, LOW);
  mylcd.clear();

}

void loop() {
  count++;
  getFingerprintID();

  // Check if a new RFID tag is detected
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Read the UID of the tag
    //  mylcd.clear();
    uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    // Print the UID on the serial monitor
    Serial.print("Scanned UID: ");
    Serial.println(uid);

    // mfrc522.PICC_HaltA();  // Halt the tag
  }


  if (uid == "03522007") {
    unlockdoor();
    delay(1000);
    Card();
  }
  else if (uid == "000") {
  }
  else {
    Alert();
    mylcd.clear();
    mylcd.setCursor(1, 0);
    mylcd.print("ACCESS DENIED");
    mylcd.setCursor(2, 1);
    mylcd.print("INVALID CARD");
    delay(2000);
    mylcd.clear();
    mylcd.setCursor(0, 0);
    mylcd.print("PLACE VALID CARD");
    mylcd.setCursor(3, 1);
    mylcd.print("TO UNLOCK");
    delay(3000);
    Card();
  }
  uid = "000";

  if (digitalRead(esp) == HIGH) {
    mylcd.setCursor(0, 0);
    mylcd.print("      FACE      ");
    mylcd.setCursor(0, 1);
    mylcd.print("   RECOGNIZED   ");
    delay(2000);
    unlockdoor();
    delay(1000);
    Card();
  }
  Card();
  delay(10);
   Serial.println(id);
//  return;
  
  if (digitalRead(enroll_btn) == 0) {
    id = 0;
    Serial.print("toggled");
  }
  if (id == 1) {// ID #0 not allowed, try again!
     return;
  }

//return;
  Serial.print("Enrolling ID #");
  Serial.println(id);
  while (!getFingerprintEnroll());
     id = 1;
}

void unlockdoor() {
  digitalWrite(Slock, HIGH);
  digitalWrite(buzz, HIGH);
  delay(1000);
  digitalWrite(buzz, LOW);
  mylcd.clear();
  mylcd.setCursor(0, 0);
  mylcd.print(" ACCESS GRANTED ");
  mylcd.setCursor(0, 1);
  mylcd.print("  DOOR OPENED   ");
  delay(2000);
  digitalWrite(Slock, LOW);
  mylcd.setCursor(1, 0);
  mylcd.print("  DOOR CLOSED   ");

}


void Alert() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzz, HIGH);
    Serial.println("on");
    delay(200);
    digitalWrite(buzz, LOW );
    Serial.println("off");
    delay(200);
  }
}
void Card() {
  if (count < 20) {
    mylcd.setCursor(0, 0);
    mylcd.print("PLACE YOUR CARD ");
    mylcd.setCursor(0, 1);
    mylcd.print("   TO UNLOCK    ");
  }
  else if (count >= 20 && count < 40) {

    mylcd.setCursor(0, 0);
    mylcd.print("  PLACE FINGER  ");
    mylcd.setCursor(0, 1);
    mylcd.print("   TO UNLOCK    ");
  }

  else if (count >= 40 && count < 60) {

    mylcd.setCursor(0, 0);
    mylcd.print("      SMART     ");
    mylcd.setCursor(0, 1);
    mylcd.print(" DOOR LOCK SYS. ");
  }

  if (count >= 60) {
      mylcd.clear();
    count = 0;
  }

}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    mylcd.setCursor(0, 0);
    mylcd.print("   FINGER NOT   ");
    mylcd.setCursor(0, 1);
    mylcd.print("   RECOGNIZED   ");
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    delay(200);
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    delay(200);
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  mylcd.clear();
  mylcd.setCursor(0, 0);
  mylcd.print("     FINGER     ");
  mylcd.setCursor(0, 1);
  mylcd.print("   RECOGNIZED   ");
  delay(1000);
  unlockdoor();
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  mylcd.clear();
  mylcd.setCursor(0, 0);
  mylcd.print("     FINGER     ");
  mylcd.setCursor(0, 1);
  mylcd.print("   RECOGNIZED   ");
  delay(1000);
  unlockdoor();

  return finger.fingerID;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  mylcd.setCursor(0, 0);
  mylcd.print("  MODE: ENROLL  ");
  mylcd.setCursor(0, 1);
  mylcd.print(" PLACE FINGER!  ");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }


  Serial.println("Remove finger");
  mylcd.setCursor(0, 0);
  mylcd.print(" REMOVE FINGER  ");
  mylcd.setCursor(0, 1);
  mylcd.print("                ");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  mylcd.setCursor(0, 0);
  mylcd.print("  PLACE FINGER  ");
  mylcd.setCursor(0, 1);
  mylcd.print("     AGAIN!     ");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    mylcd.setCursor(0, 0);
    mylcd.print(" FINGER PRINT  ");
    mylcd.setCursor(0, 1);
    mylcd.print("    MATCHED     ");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    mylcd.setCursor(0, 0);
    mylcd.print("FINGER MISMATCH ");
    mylcd.setCursor(0, 1);
    mylcd.print("   TRY AGAIN!   ");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    mylcd.setCursor(0, 0);
    mylcd.print("   FINGER ID    ");
    mylcd.setCursor(0, 1);
    mylcd.print("     STORED     ");
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
