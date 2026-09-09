# HAVEN SENSE
## Privacy-Preserving Environmental and Behavioural Monitoring Prototype for Mental-Health Care

**Project Type:** Embedded Systems / IoT / Healthcare Technology  
**Controller:** ESP32  
**Development Environment:** Arduino IDE  
**Prototype Status:** Proof of Concept

---

## Abstract

HAVEN SENSE is a privacy-oriented embedded monitoring prototype intended for mental-health care environments where continuous physical observation by a nurse or caretaker may make a patient uncomfortable.

The central concept is to place a sensor node inside the patient's room and observe changes in the surrounding environment and non-invasive behavioural indicators rather than depending entirely on continuous direct observation. An ESP32 collects data from multiple sensors and performs simple local rule-based interpretation. Depending on the detected state, the prototype provides ambient visual feedback through an RGB NeoPixel strip and predefined audio feedback through a DFPlayer Mini.

The prototype includes sensing for room temperature and humidity, motion, pressure/activity, distance and sound level. An MLX90614 infrared temperature sensor and MPU6050 were also included in the hardware setup. A GSM module was considered for emergency SMS communication, but the available modules were faulty during development, so GSM/SMS transmission is not part of the current implementation.

The project demonstrates the feasibility of a low-cost, privacy-conscious monitoring architecture while also identifying the limitations that must be addressed before any real healthcare deployment.

---

## 1. Introduction

Mental-health patients may require observation for safety and wellbeing, but continuous presence of a nurse or caretaker can reduce privacy and may make some patients uncomfortable.

HAVEN SENSE explores whether an embedded sensing system can provide a degree of continuous environmental and activity monitoring without requiring a person to remain physically present in the room at all times.

The system is intentionally designed around **non-invasive room and behavioural sensing**. Instead of continuously collecting sensitive physiological information, the prototype looks for changes such as movement, pressure/activity, sound and environmental conditions.

The ESP32 acts as the central controller. It reads sensor values, applies a simple decision algorithm and controls local feedback devices.

---

## 2. Problem Statement

A conventional monitoring arrangement may require a caretaker to remain near a patient or repeatedly enter the patient's room to check their condition.

This creates two competing requirements:

1. **Safety:** abnormal activity or possible distress should not go unnoticed.
2. **Privacy and comfort:** the patient should not feel continuously watched.

HAVEN SENSE addresses this as an embedded-system design problem:

> Can a room-based sensor system detect meaningful changes in activity and environmental conditions and provide a local response, while reducing the need for continuous physical observation?

---

## 3. Objectives

The main objectives of the prototype are:

- To design an ESP32-based room monitoring system.
- To collect multiple environmental and behavioural sensor readings.
- To avoid relying solely on direct physiological measurement.
- To process sensor data locally.
- To classify basic prototype states using sensor thresholds.
- To provide non-invasive ambient feedback using RGB lighting.
- To provide predefined audio feedback.
- To create a foundation for future remote emergency notifications.
- To explore a privacy-conscious approach to healthcare monitoring.

---

## 4. System Architecture

The prototype follows this general architecture:

```text
        ENVIRONMENT / PATIENT ROOM
                    │
        ┌───────────┼─────────────┐
        │           │             │
      DHT22        PIR          FSR
        │           │             │
 Temp/Humidity   Motion      Pressure/
                              Activity
        │           │             │
        └───────────┼─────────────┘
                    │
             HC-SR04 / Mic
                    │
                    ▼
             ┌────────────┐
             │   ESP32    │
             │            │
             │ Acquisition│
             │ Processing │
             │ Decision   │
             └─────┬──────┘
                   │
          ┌────────┴────────┐
          ▼                 ▼
    NeoPixel LEDs       DFPlayer
    Visual feedback     Audio feedback

          Future communication
                   │
              Wi-Fi / GSM
                   │
                   ▼
           Emergency Contact
```

---

## 5. Hardware Components

### 5.1 ESP32

The ESP32 is the main controller. It handles sensor acquisition, local processing, LED control and serial communication. Its integrated Wi-Fi capability also provides a possible future route for remote notifications.

### 5.2 DHT22

The DHT22 is used to obtain:

- ambient temperature
- relative humidity

These values describe the room environment.

### 5.3 PIR Motion Sensor

The PIR sensor detects motion/activity in the monitored area.

### 5.4 FSR Pressure Sensor

The Force Sensitive Resistor is used as an analogue activity/pressure input. The current code reads one FSR through an ESP32 ADC pin.

Two FSR sensors were available for the prototype, but only one is currently read by the submitted sketch.

### 5.5 HC-SR04

The ultrasonic sensor measures approximate distance using the time taken for an ultrasonic pulse to return after reflection.

### 5.6 I2S Microphone

The I2S microphone provides digital audio samples to the ESP32. The current program uses a simplified sound-amplitude calculation rather than speech recognition or clinical voice analysis.

### 5.7 MLX90614

The MLX90614 is an infrared temperature sensor capable of non-contact temperature measurement. It is initialized in the current program and its object-temperature reading is displayed.

Because the project is intended to be privacy-conscious, this sensor should be treated as an optional/non-invasive sensing channel rather than as a requirement for continuously measuring the patient's body.

### 5.8 MPU6050

The MPU6050 contains an accelerometer and gyroscope. It was included in the hardware prototype and initialized in the current code, but its measurements are not yet used in the state-classification logic.

### 5.9 NeoPixel RGB Strip

The RGB strip provides visual feedback inside the room. Different colours represent the state selected by the current rule-based logic.

### 5.10 DFPlayer Mini

The DFPlayer Mini is used to play predefined audio tracks stored on a microSD card.

### 5.11 GSM/SIM Module

A GSM/SIM module was intended for emergency SMS communication. The available modules were faulty during development, so GSM communication could not be completed.

---

## 6. Sensor-to-Function Mapping

| Sensor / Module | Current role |
|---|---|
| DHT22 | Ambient temperature and humidity |
| PIR | Motion/activity |
| FSR | Pressure/activity |
| HC-SR04 | Distance/proximity |
| I2S microphone | Sound level |
| MLX90614 | IR object temperature |
| MPU6050 | Available for motion/IMU expansion; currently initialized only |
| NeoPixel | Local visual feedback |
| DFPlayer Mini | Local audio feedback |
| ESP32 Wi-Fi | Future remote communication |
| GSM module | Planned SMS communication; not operational in prototype |

---

## 7. Pin Configuration

The following pins are defined in the current ESP32 sketch:

| Signal | GPIO |
|---|---:|
| DHT22 | 4 |
| FSR | 34 |
| PIR | 14 |
| HC-SR04 TRIG | 32 |
| HC-SR04 ECHO | 33 |
| NeoPixel | 27 |
| I2S WS | 13 |
| I2S SD | 35 |
| I2S SCK | 12 |
| DFPlayer serial RX | 16 |
| DFPlayer serial TX | 17 |

I²C peripherals use the ESP32's default I²C interface as configured by `Wire.begin()`.

---

## 8. Software Methodology

The software performs the following sequence every cycle:

1. Read DHT22 temperature and humidity.
2. Read MLX90614 object temperature.
3. Read FSR analogue value.
4. Read PIR motion state.
5. Measure distance using HC-SR04.
6. Read one I2S microphone sample.
7. Convert the microphone sample into a simple sound-level value.
8. Apply threshold-based rules.
9. Select a prototype state.
10. Change the NeoPixel colour.
11. Play the corresponding DFPlayer track.
12. Print sensor readings and the selected state to the Serial Monitor.
13. Wait approximately three seconds and repeat.

---

## 9. Rule-Based State Detection

The current code uses the following priority order:

```text
IF soundLevel > 2000
    → ANXIETY

ELSE IF pressureValue > 2000
    → STRESS

ELSE IF motionDetected == 0
    → DISTRESS

ELSE
    → CALM
```

This is a simple proof-of-concept algorithm.

### Important interpretation

The labels **Calm, Stress, Anxiety and Distress** are not clinical diagnoses. They are names assigned to sensor-response states for demonstrating the prototype's behaviour.

For a real system, a much more robust approach would be required, including sensor calibration, temporal features, multiple-sensor fusion, validation and clinical testing.

---

## 10. Ambient Feedback

The prototype uses the RGB strip as a non-invasive local feedback mechanism.

| State | RGB output | Audio track |
|---|---|---|
| Calm | Blue | Track 1 |
| Stress | Yellow | Track 2 |
| Anxiety | Red | Track 3 |
| Distress | Purple | Track 4 |

The purpose of the ambient lighting is to provide an immediate visual indication and potentially create a calmer room environment without requiring a caretaker to enter the room for every small change.

The final choice of colours and audio should be validated with users and healthcare professionals rather than assumed to have a universal psychological effect.

---

## 11. Remote Alert Concept

The original project concept included sending an alert to an emergency contact when a significant change was detected.

Two communication approaches were considered:

### GSM/SMS

A GSM/SIM module could directly send an SMS to a configured emergency contact.

However, the GSM modules available during development were faulty. Therefore, this feature was not successfully implemented in the final prototype.

### ESP32 Wi-Fi

The ESP32 has built-in Wi-Fi and can be extended to communicate with a server, dashboard or messaging/API service.

A future implementation could follow:

```text
Sensor change
     ↓
ESP32 local processing
     ↓
Abnormal event detected
     ↓
Wi-Fi
     ↓
Cloud/API/server
     ↓
Emergency notification
```

The current submitted code does not yet contain this network-alert implementation.

---

## 12. Privacy-Centered Design

Privacy is a central motivation of the project.

The prototype attempts to reduce the need for continuous direct observation by placing sensing hardware in the environment.

Potential privacy advantages include:

- reduced requirement for continuous physical presence
- local processing of basic sensor values
- transmission of events rather than necessarily transmitting raw data
- reduced dependence on cameras for monitoring

A production system would still require strong access control, encryption, data minimization, informed consent and appropriate healthcare privacy safeguards.

---

## 13. Prototype Photographs

The prototype was assembled on a breadboard and also arranged as a simple room model.

The photographs show the ESP32-based electronics, sensor wiring and the room-model placement of several sensors and the RGB strip.

---

## 14. Results / Demonstration

The current prototype demonstrates:

- ESP32 sensor acquisition
- DHT22 environmental sensing
- PIR motion detection
- FSR analogue sensing
- HC-SR04 distance measurement
- I2S microphone sampling
- MLX90614 temperature reading
- local threshold-based state selection
- RGB ambient feedback
- DFPlayer audio playback
- Serial Monitor status reporting

The prototype also demonstrates that multiple sensing modalities can be integrated into a single ESP32-based embedded system.

---

## 15. Limitations

Several limitations were identified during development.

### Hardware limitations

- GSM modules available for the project were faulty.
- No dedicated O₂ sensor is present.
- No dedicated CO₂ sensor is present.
- The exact physical wiring should be documented in a formal schematic before production.
- Sensor readings require calibration.

### Software limitations

- Fixed thresholds may produce false positives/negatives.
- The current sound measurement is a basic amplitude calculation.
- MPU6050 data is not currently used for classification.
- Only one FSR is read in the current program.
- Wi-Fi alerting is not implemented in the current sketch.
- No long-term data logging is included.
- No machine-learning model is used.

### Healthcare limitations

The prototype is not a medical diagnostic device and cannot reliably determine a patient's psychological or medical condition from these sensor readings alone.

Any clinical deployment would require extensive validation and appropriate safety, privacy and regulatory processes.

---

## 16. Future Scope

### Multi-sensor fusion

Combine PIR, FSR, distance, sound, temperature and IMU data instead of using one threshold at a time.

### Time-window analysis

Instead of reacting to a single reading, analyse changes over several seconds or minutes.

### Adaptive baseline

Learn the normal environmental/activity pattern of a particular room and detect deviations from that baseline.

### O₂/CO₂ monitoring

Add dedicated, calibrated gas sensors if environmental oxygen and carbon-dioxide monitoring is required.

### Wi-Fi emergency notification

Implement secure API/server communication using the ESP32's Wi-Fi.

### Dashboard

Develop a dashboard for authorized caregivers showing only meaningful events and trends.

### Data privacy

Use local processing and event-level transmission where possible.

### Hardware refinement

Move from breadboard prototype to a proper PCB after the circuit is validated.

### Clinical validation

Evaluate the system with healthcare professionals and representative use cases before considering real-world deployment.

---

## 17. Conclusion

HAVEN SENSE demonstrates a practical embedded-system approach to privacy-conscious monitoring in mental-health care environments.

The prototype combines multiple low-cost sensors with an ESP32 to observe room conditions and behavioural indicators, processes these values locally and provides visual and audio feedback.

The most important contribution of the prototype is not clinical diagnosis; it is the exploration of how embedded sensing can reduce the need for continuous physical observation while maintaining a more comfortable environment for the patient.

The current implementation provides a foundation for future work in secure Wi-Fi communication, multi-sensor fusion, environmental gas sensing, adaptive anomaly detection, data logging and clinically validated healthcare monitoring.

---

## 18. Technologies Used

- ESP32
- Arduino IDE
- Embedded C/C++
- I²C
- I²S
- UART
- ADC
- DHT22
- PIR
- FSR
- HC-SR04
- MLX90614
- MPU6050
- NeoPixel
- DFPlayer Mini
- IoT / Wi-Fi communication concept
- Sensor fusion and rule-based decision logic

---

## 19. Project Deliverables

The repository contains:

- `README.md` — project overview and complete code reference
- `src/HavenSense.ino` — current ESP32 implementation
- `REPORT.md` — detailed project report
- `docs/connection-notes.md` — software pin mapping and wiring notes
- `images/` — prototype photographs

---

## 20. Author

**Sudeshna Patra**  
B.Tech, Electronics and Communication Engineering

**Project:** HAVEN SENSE
