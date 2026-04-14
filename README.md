# 🚀 Smart Motor Control System using STM32 & ESP32

---

## 📌 Overview

This project implements a **smart motor and conveyor control system** using STM32 and ESP32.
The system supports **dual control**:

* 🟢 Hardware push buttons
* 📱 WiFi control via mobile (ESP32 web interface)

It also provides **real-time monitoring** using:

* LCD display
* Webpage (auto-updating)

---

## 🎯 Objectives

* Implement sequential motor and conveyor operation
* Display real-time system status
* Enable wireless control using WiFi
* Achieve synchronization between hardware and IoT

---

## ⚙️ Features

* ⏱ 5-second delay before motor starts
* ⚡ Motor runs for 20 seconds
* 🔄 Conveyor runs for 8 seconds
* 📟 LCD shows live status and timing
* 🌐 Webpage displays real-time updates
* 🔁 Dual control (buttons + mobile)
* 🔌 UART communication between STM32 & ESP32

---

## 🧠 System Architecture

```
Mobile → ESP32 (WiFi Server) → UART → STM32 → Motor + LCD
                ↑
           Hardware Buttons
```

---

## 🔌 Hardware Components

* STM32 Microcontroller
* ESP32 WiFi Module
* 16x2 LCD (I2C)
* Push Buttons (Start/Stop)
* LEDs / Relay Module
* Breadboard & Power Supply

---

## 🔄 Working Principle

1. System starts in **Idle state**
2. On START:

   * Waits for 5 seconds
   * Motor turns ON (20 seconds)
   * Conveyor turns ON (8 seconds)
3. System returns to **Idle state**
4. Control possible via:

   * Buttons
   * Mobile webpage

---

## 📡 Communication Protocol

UART communication is used between STM32 and ESP32.

| Signal | Meaning                 |
| ------ | ----------------------- |
| Mx     | Motor ON (x seconds)    |
| Cx     | Conveyor ON (x seconds) |
| I      | System Idle             |

---

## 📱 Web Interface

* Hosted by ESP32
* Accessible via: `192.168.4.1`
* Auto-refresh every 1 second
* Provides:

  * START button
  * STOP button
  * Live system status

---

## 📂 Project Structure

```
smart-motor-control-system/
│
├── docs/          → Report & diagrams
├── stm32/         → STM32 source code
├── esp32/         → WiFi control code
├── logo_soft/     → PLC implementation
├── images/        → Project images
├── results/       → Observations & logs
├── video/         → Demo video
```

---

## 🧪 Results

* ✔ Accurate timing control achieved
* ✔ LCD displays stable output
* ✔ Webpage shows live updates
* ✔ Dual control works without conflict
* ✔ UART communication is reliable

---

## 📷 Output Screens

* 📟 LCD Display → `/images/lcd_display.jpg`
* 🌐 Webpage → `/images/webpage.jpg`
* 🔌 Setup → `/images/setup.jpg`

---

## 🎥 Demo

See:

```
/video/demo.mp4
```

---

## 🧾 PLC Implementation

* Implemented using **LOGO! Soft Comfort**
* File: `/logo_soft/program.lld`
* Includes:

  * Ladder Diagram
  * Simulation

---

## 🧠 Conclusion

This project successfully integrates **embedded systems with IoT**, enabling real-time monitoring and dual control.
It demonstrates efficient communication using UART and practical automation logic.

---

## 📚 Future Improvements

* Real-time web update without refresh (AJAX)
* Mobile app interface
* Sensor-based automation
* Cloud integration

---

## 👨‍💻 Author

Naraender

---
