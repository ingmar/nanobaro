#include <SFE_BMP180.h>
#include <Wire.h>

#define NMEA_TALKER_ID "WI" // Weather Instruments
#define NMEA_DELAY 10 // Send data every 10 seconds

// meters above mean sea level
#define ALTITUDE 1
// How many samples to take per iteration
#define PRESS_OVERSAMPLING 3


SFE_BMP180 pressure;

int init_pressure() {
  if (pressure.begin()) return true;
}

void setup() {
  Serial.begin(4800);

  if (init_pressure() != true) {
    nmea_txt("BMP180 init fail");
    exit(1);
  }
  nmea_txt("Nanobaro ready.");
}

int get_temperature(double &temp) {
  int status;

  status = pressure.startTemperature();
  if (status) {
    // Wait for the measurement to complete:
    delay(status); // Typically 5ms
    status = pressure.getTemperature(temp);
    if (status) return true;
  }
  return false;
}

int get_pressure(double &press, double &temp) {
  int status;

  status = pressure.startPressure(PRESS_OVERSAMPLING);
  if (status) {
    // Wait for the measurement to complete:
    delay(status); // Typically 26 at oversampling 3 in startPressure
    status = pressure.getPressure(press, temp);
    if (status) return true;
  }
  return false;
}

const byte buff_len = 90;
char CRCbuffer[buff_len];

byte nmea_crc(String msg) {
  // NMEA CRC: XOR each byte with previous for all chars between '$' and '*'
  char c;
  int i;
  byte crc = 0;
  for (i = 0; i < buff_len; i++){
    crc ^= msg.charAt(i);  // XOR
  }
  return crc;
}

void nmea_send(String sentence, String params) {
  String msg = String(NMEA_TALKER_ID) + sentence + params;

  msg.toCharArray(CRCbuffer, sizeof(CRCbuffer)); // put complete string into CRCbuffer
  int crc = nmea_crc(CRCbuffer);

  if (crc < 16) msg += "0"; // pad with leading 0
  String hexcrc = String(crc, HEX);
  hexcrc.toUpperCase();
  Serial.println("$" + msg + "*" + hexcrc);
}

void nmea_txt(String text) {
  nmea_send("TXT", ",01,01,01," + text);
}

String s;
double temp, press, seapress;

void loop() {
  if (!get_temperature(temp)) {
    nmea_txt("Temperature read failure");
    temp = -1000.0;
  }

  if (temp > -1000.0) {
    if (get_pressure(press, temp)) {
      seapress = pressure.sealevel(press, ALTITUDE);
      s = "";
      s = ",C," + String(temp)  + ",C,TEMP";
      s += ",P," + String(press / 1000.0, 5) + ",B,BARO"; // OpenCPN doesn't seem to grok Pascal (unit "P"), only Bar ("B")
      nmea_send("XDR", s);
      // These NMEA 0183 sentences are deprecated:
      //nmea_send("MTA", "," + String(temp) + ",C");
      //nmea_send("MMB", ",0.0,I,1.5,B");
    } else {
      nmea_txt("Pressure read failure");
      seapress = 0.0;
    }
  }

  delay(NMEA_DELAY*1000);
}
