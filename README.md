# 🦾 Accessible Elevator System For Disabled Person Using Voice Recognition and Eye Gesture Control
### STM32-Based Automated Elevator for Differently-Abled Persons

---

## 📌 Project Overview

This project is a **fully offline, embedded lift control system** designed to assist **differently-abled and disabled persons** who cannot operate conventional elevator buttons. The system allows users to control a multi-floor lift using:

- 👁️ **Eye Blink Gestures** — Blink 3, 4, or 5 times to select Ground, First, or Second floor
- 🎙️ **Offline Voice Recognition** — Speak floor commands without any internet connection
- 📡 **Serial Hex Commands** — External controller sends hex commands (0xA1, 0xA2, 0xA3) via UART

The entire system runs **offline on STM32 microcontroller** — no cloud, no Wi-Fi, no internet required. This makes it reliable even in power-sensitive or network-unavailable environments.

---

## 🎯 Problem Statement

Conventional elevators require physical button pressing, which is impossible or extremely difficult for:
- Persons with limb disabilities
- Paralysis patients
- Elderly individuals with motor impairments

This system provides a **hands-free, accessible, and affordable** alternative using eye gestures and voice commands.

---

## ⚙️ Hardware Used

| Component | Purpose |
|---|---|
| **STM32 Microcontroller** | Main processing unit |
| **Stepper Motor (28BYJ-48)** | Drives the lift mechanism |
| **IR/Optical Blink Sensor** | Detects eye blink gestures |
| **Floor Sensor (Analog)** | Detects ground floor position |
| **Buzzer** | Audio floor announcement feedback |
| **Offline Voice Recognition Module** | Recognizes spoken floor commands |
| **UART Serial Interface** | Receives hex commands from external controller |

---

## 🧠 How It Works

### 1. Eye Blink Control
```
Blink 3 times → Move to Ground Floor (Floor 0)
Blink 4 times → Move to First Floor  (Floor 1)
Blink 5 times → Move to Second Floor (Floor 2)
```
- Blink detection uses a **5-second window** to count blinks
- Invalid blink count triggers a **buzzer alert**

### 2. Offline Voice Recognition
- User speaks the floor name
- Onboard offline voice model processes the command **without internet**
- Sends serial hex command to STM32

### 3. Serial Hex Commands (UART)
```
Header: 0x20
0x20 0xA1 → Ground Floor
0x20 0xA2 → First Floor
0x20 0xA3 → Second Floor
```

### 4. Audio Feedback (Buzzer)
```
Ground Floor → 1 beep
First Floor  → 2 beeps
Second Floor → 3 beeps
```

---

## 📁 Repository Structure

```
├── firmware/
│   └── main.ino          # Main STM32 firmware (Arduino framework)
├── hardware/
│   └── circuit_diagram   # Wiring and connection diagrams
├── demo/
│   └── demo_video_link   # Link to project demo video
├── docs/
│   └── project_report    # Full project documentation
└── README.md
```

---

## 🔌 Pin Configuration

```cpp
#define IN1 PA3           // Stepper Motor Pin 1
#define IN2 PA2           // Stepper Motor Pin 2
#define IN3 PA1           // Stepper Motor Pin 3
#define IN4 PA0           // Stepper Motor Pin 4
#define SENSOR_PIN    PA4 // Floor detection sensor
#define BLINK_SENSOR_PIN PA5 // Eye blink sensor
#define BUZZER_PIN    PA6 // Buzzer for audio feedback
```

---

## 🚀 Getting Started

### Prerequisites
- STM32CubeIDE or Arduino IDE with STM32 board support
- STM32 board package installed
- USB-TTL converter for flashing

### Flashing the Firmware
1. Clone this repository
   ```bash
   git clone https://github.com/yourusername/accessible-lift-control.git
   ```
2. Open `firmware/main.ino` in Arduino IDE
3. Select your STM32 board from **Tools → Board**
4. Connect STM32 via USB and click **Upload**

---

## 📊 System Flow

```
Power ON
   ↓
Initialize → Move to Ground Floor (Auto-calibration)
   ↓
Wait for Input
   ↓
┌─────────────────────┬──────────────────────┬─────────────────────┐
│   Eye Blink Input   │  Voice Command Input  │   Serial Hex Input  │
│  (count in 5 sec)   │  (offline model)      │   (0x20 + command)  │
└─────────┬───────────┴──────────┬───────────┴──────────┬──────────┘
          ↓                      ↓                       ↓
     Select Floor           Select Floor            Select Floor
          ↓                      ↓                       ↓
          └──────────────────────┴───────────────────────┘
                                 ↓
                      Move Stepper Motor
                                 ↓
                      Play Buzzer Feedback
                                 ↓
                      Wait for Next Input
```

---

## 🌟 Key Features

- ✅ **Fully Offline** — Works without internet or cloud
- ✅ **Multi-Input** — Eye blink + Voice + Serial commands
- ✅ **Audio Feedback** — Buzzer announces each floor
- ✅ **Auto Calibration** — Auto moves to ground floor on power ON
- ✅ **Affordable** — Built with low-cost components
- ✅ **Real-time** — Instant response to user input

---

## 🎥 Demo Video

> 📹 **[[https://youtube.com/shorts/m80ZRAqRTQ8?feature=share](https://youtube.com/shorts/m80ZRAqRTQ8?feature=share)](#)** ← *(Click here to watch the project demo)*

---

## 👨‍💻 Author

**[Ramasamy.V]**
- 🎓 Final Year — [Electronics and Communication Engineering], [Ramco Institute of Technology]
- 📍 Virudhunagar, Tamil Nadu
- 💼 [https://www.linkedin.com/in/ramasamy-v-b4b074308/]
- 🐙 [https://github.com/Ramasamy15012004/]

---

## 🏷️ Tags

`STM32` `Embedded Systems` `IoT` `Accessibility` `Eye Gesture` `Voice Recognition` `Stepper Motor` `UART` `Offline AI` `Assistive Technology` `Final Year Project` `C++` `Arduino Framework` `Disabled Assistance`

---

## 📄 License

This project is open source under the [MIT License](LICENSE).

---

> *"Technology should be accessible to everyone — this project is one small step in that direction."*
