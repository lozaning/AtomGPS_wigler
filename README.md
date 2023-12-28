# AtomGPS Wigler

## Overview
AtomGPS_wigler is a wardriving tool initially **created by @lozaning** using the M5Stack Atom GPS kit. It's designed for Wi-Fi network scanning with LED status indicators.

## Features
- Deduplication avoids logging multiple entries for the same MAC. 
- LED status indicators for wardriving:

  • Blink Purple: Waiting for GPS fix.

  • Steady Green: GPS locked, begin scan.

  • Flash Blue: New network found.
  
- Log to [Wigle](https://wigle.net) compatible .CSV 

## Prerequisites
- M5Stack Atom GPS Kit
- TF Card
- ESPtool

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

Flash the firmware:
```bash
esptool.py --chip esp32 --port [PORT] erase_flash write_flash -z 0x1000 [FIRMWARE_FILE]
```
Replace `[PORT]` with your device's port and `[FIRMWARE_FILE]` with the firmware file.

## Usage
After flashing, the device starts scanning for Wi-Fi networks, indicating the status through LEDs. Purple for waiting on GPS fix, Green when acquired and Blue flashes for new networks. 
