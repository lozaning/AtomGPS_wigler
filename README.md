# AtomGPS Wigler

## Overview
AtomGPS_wigler is a wardriving tool initially **created by @lozaning** using the M5Stack Atom GPS kit. It's designed for Wi-Fi network scanning with LED status indicators.

## Features - v1.0-1.1
- Deduplication avoids logging multiple entries for the same MAC. 
- LED status indicators for wardriving:

  • Blink Red: SD card missing/failed. 

  • Blink Purple: Waiting for GPS fix.

  • Steady Green: GPS locked, begin scan.

  • Flash Blue: New network found.
  
- Log to [Wigle](https://wigle.net) compatible .CSV

  ## Features - v1.2 - (less verbose, more networks)

  - Hidden network scanning
  - Faster channel hopping
  - LED differences:
     • No blue flashes on network found
     • Red on card write error
  
## Prerequisites
- M5Stack Atom GPS Kit
- TF Card
- ESPtool or Arduino IDE

## Installation
Clone the repository:
```bash
git clone https://github.com/lukeswitz/AtomGPS_wigler.git
```

## Flashing Instructions
Identify your device's port:
- On Linux/macOS:
  ```bash
  ls /dev/ttyUSB* /dev/cu.*
  ```
- On Windows, check COM port in Device Manager.

Flash the firmware .bin version of your choice
```bash
esptool.py --chip auto --port [PORT] erase_flash write_flash -z 0x1000 [FIRMWARE_FILE]
```
Replace `[PORT]` with your device's port and `[FIRMWARE_FILE]` with the firmware file.

## Usage
After flashing, the device starts scanning for Wi-Fi networks, indicating the status through LEDs and saving found networks to a Wigle.net compatible CSV file. 
