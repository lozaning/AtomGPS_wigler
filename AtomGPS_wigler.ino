#include "M5Atom.h"
#include "GPSAnalyse.h"
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include <WiFi.h>

GPSAnalyse GPS;
String fileName;

void setup() {
  M5.begin(true, false, true);
  SPI.begin(23, 33, 19, -1); // Adjust for your SPI pins

  // Check if SD Card is present
  if (!SD.begin(-1, SPI, 40000000)) {
    M5.dis.drawpix(0, 0xff0000); // Flash Red if TF card is missing
    delay(1000);
    return;
  }

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Initialize GPS
  Serial1.begin(9600, SERIAL_8N1, 22, -1); // GPS Serial
  GPS.setSerialPtr(Serial1);
  GPS.start();

  // Find a new file name and create CSV header if the file is new
  int fileNumber = 0;
  bool isNewFile = false;
  do {
    fileName = "/wifi-scans" + String(fileNumber) + ".csv"; // Ensure path starts with '/'
    isNewFile = !SD.exists(fileName);
    fileNumber++;
  } while (!isNewFile);
  
  if (isNewFile) {
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("MAC,SSID,RSSI,Lat,Lon,UTC");
      dataFile.close();
    }
  }
  Serial.println("Using file name: " + fileName);
}

void loop() {
  // GPS Update and Check Lock Status
  GPS.upDate();
  if (GPS.isGpsFixed()) {
    M5.dis.drawpix(0, 0x00ff00); // Pulse Green when GPS is locked
  } else {
    M5.dis.clear(); // Clear LED if GPS not locked
  }

  float lat = GPS.s_GNRMC.Latitude;
  float lon = GPS.s_GNRMC.Longitude;
  String utc = GPS.s_GNRMC.Utc;

  int numNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numNetworks; i++) {
    String dataString = WiFi.BSSIDstr(i) + "," + WiFi.SSID(i) + "," +
                        String(WiFi.RSSI(i)) + "," + String(lat, 6) + "," + 
                        String(lon, 6) + "," + utc;

    File dataFile = SD.open(fileName, FILE_APPEND);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      M5.dis.drawpix(0, 0x0000ff); // Flash Blue for each new Wi-Fi network
      delay(200);
      M5.dis.clear(); // Clear the LED after flashing
    } else {
      Serial.println("Error opening " + fileName);
    }
  }
  delay(5000);
}
