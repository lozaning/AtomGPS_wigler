#include "M5Atom.h"
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include <WiFi.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;
String fileName;

const int maxMACs = 75;
String macAddressArray[maxMACs];
int macArrayIndex = 0;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Starting...");

  M5.begin(true, false, true);
  SPI.begin(23, 33, 19, -1);  // Adjust for your SPI pins

  // Check if SD Card is present
  if (!SD.begin(-1, SPI, 40000000)) {
    Serial.println("SD Card initialization failed!");
    M5.dis.drawpix(0, 0xff0000);  // Flash Red if TF card is missing
    delay(1000);
    return;
  }
  Serial.println("SD Card initialized.");

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("WiFi initialized.");

  // Initialize GPS
  Serial1.begin(9600, SERIAL_8N1, 22, -1);  // GPS Serial
  Serial.println("GPS Serial initialized.");

  // Wait for GPS to get a fix
  Serial.println("Waiting for GPS fix...");
  bool gpsFixObtained = false;
  while (!gpsFixObtained) {
    while (Serial1.available() > 0) {
      if (gps.encode(Serial1.read()) && gps.location.isValid()) {
        gpsFixObtained = true;
        M5.dis.drawpix(0, 0x00ff00);  // Green LED to indicate GPS fix
        Serial.println("GPS fix obtained.");
        break;
      }
    }

    if (!gpsFixObtained) {
      M5.dis.drawpix(0, 0x800080);  // Purple LED while waiting for GPS fix
      delay(500);
      M5.dis.clear();
      delay(500);
    }
  }

  // Find a new file name and create CSV header if the file is new
  int fileNumber = 0;
  bool isNewFile = false;
  do {
    fileName = "/wifi-scans" + String(fileNumber) + ".csv";  // Ensure path starts with '/'
    isNewFile = !SD.exists(fileName);
    fileNumber++;
  } while (!isNewFile);

  if (isNewFile) {
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("MAC,SSID,RSSI,Lat,Lon,UTC");
      dataFile.close();
      Serial.println("New file created: " + fileName);
    }
  } else {
    Serial.println("Using existing file: " + fileName);
  }

  // Clear the MAC address array
  for (int i = 0; i < maxMACs; i++) {
    macAddressArray[i] = "";
  }
}

void loop() {
  // Process available GPS data
  while (Serial1.available() > 0) {
    if (gps.encode(Serial1.read())) {
      if (gps.location.isUpdated()) {
        M5.dis.drawpix(0, 0x00ff00);  // Green LED for updated location
        Serial.println("GPS Location Updated.");
      }
    }
  }

  // Check if GPS has a valid location
  if (gps.location.isValid()) {
    float lat = gps.location.lat();
    float lon = gps.location.lng();
    char utc[11];
    sprintf(utc, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.printf("Lat: %f, Lon: %f, UTC: %s\n", lat, lon, utc);

    int numNetworks = WiFi.scanNetworks();
    for (int i = 0; i < numNetworks; i++) {
      String currentMAC = WiFi.BSSIDstr(i);

      // Check if MAC is in the array
      bool macFound = false;
      for (int j = 0; j < maxMACs; j++) {
        if (macAddressArray[j] == currentMAC) {
          macFound = true;
          break;
        }
      }

      // Skip if MAC is found
      if (macFound) {
        continue;
      }

      // Add MAC to the top of the array and shift others down
      for (int j = maxMACs - 1; j > 0; j--) {
        macAddressArray[j] = macAddressArray[j - 1];
      }
      macAddressArray[0] = currentMAC;

      // Record data to SD card
      String dataString = currentMAC + "," + WiFi.SSID(i) + "," + String(WiFi.RSSI(i)) + "," + String(lat, 6) + "," + String(lon, 6) + "," + utc;

      File dataFile = SD.open(fileName, FILE_APPEND);
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        M5.dis.drawpix(0, 0x0000ff);  // Flash Blue for each new Wi-Fi network
        delay(200);
        M5.dis.clear();  // Clear the LED after flashing
        Serial.println("Data written: " + dataString);
      } else {
        Serial.println("Error opening " + fileName);
      }
    }
  } else {
    M5.dis.drawpix(0, 0x800080);  // Purple LED if waiting for GPS fix
  }
  delay(100);  // Short delay for loop iteration
}
