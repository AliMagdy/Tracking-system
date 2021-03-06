#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <NMEAGPS.h>

#include <sms.h>
SMSGSM sms;

static NMEAGPS gps;
SoftwareSerial ss(4, 3);
SoftwareSerial SIM900(7, 8);

static uint8_t number_valid_locations = 0; // consecutive good data

void doSomeWork()
{
  char text[60];
  char *ptr = &text[0];

  strcpy( ptr, "My Location is \nlatitude: " );
  ptr = &ptr[ strlen(ptr) ]; // step to the end
  dtostrf( gps.fix().latitude(), 8, 6, ptr );
  ptr = &ptr[ strlen(ptr) ];
  strcpy( ptr, "\nlongitude: " );
  ptr = &ptr[ strlen(ptr) ];
  dtostrf( gps.fix().longitude(), 8, 6, ptr );

  Serial.println( text );

  gsm.begin(9600); // not listening to GPS any more!
  if (sms.SendSMS("+xxxxxxxxxx", text)) {
    Serial.println("\nSMS sent OK.");
  } else {
    Serial.println("\nError sending SMS.");
  }

  do {} while (1);
}

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
}

void loop()
{
  while (ss.available()) {

    if (gps.decode( ss.read() ) == NMEAGPS::DECODE_COMPLETED) {

      if (gps.nmeaMessage == NMEAGPS::NMEA_RMC) {

        if (gps.fix().valid.location) {
          number_valid_locations++;

          if (number_valid_locations >= 4) {
            doSomeWork();
            number_valid_locations = 0; // reset counter
          }

        } else // no valid location, reset counter
          number_valid_locations = 0;

      }
    }
  }
}
