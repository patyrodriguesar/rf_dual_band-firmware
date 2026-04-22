# RF Firmware — Dual-Band RF Cyber Threat Detection Device

Bare-metal C firmware for a dual-band RF detector operating at **13.56 MHz** (NFC/RFID)
and **433 MHz** (ISM band), designed for spectrum monitoring and cybersecurity threat detection.

Developed as part of a Master's research at **UNIFEI** (Universidade Federal de Itajubá),
affiliated with the **Cybersecurity and Critical Systems (CS)² group**.

## Hardware

| Component        | Details                                     |
|------------------|---------------------------------------------|
| Microcontroller  | STM32F103C8T6 (ARM Cortex-M3, 72 MHz)       |
| Display          | OLED 128×64 SSD1306 (I2C)                   |
| RF Input         | PB0 — digital RF signal (floating input)    |
| Buzzer           | PA1 — push-pull output (jamming alert)      |
| Power Button     | PA0 — input pull-up                         |
| Communication    | I2C1 via PB6 (SCL) / PB7 (SDA) at 100 kHz   |
| Programming      | Pure C — direct register access (no HAL)    |
| Reference Manual | RM0008 STM32F10x Reference Manual           |

## Pin Mapping

| Pin  | Function          | Mode                    |
|------|-------------------|-------------------------|
| PA0  | POWER button      | Input pull-up           |
| PA1  | Buzzer            | Output push-pull 2 MHz  |
| PB0  | RF signal input   | Input floating          |
| PB6  | SCL I2C1          | AF open-drain 50 MHz    |
| PB7  | SDA I2C1          | AF open-drain 50 MHz    |

## System Architecture

### Timekeeping
SysTick is configured for **1 ms resolution** at 72 MHz:
```
LOAD = 72000 - 1 = 71999
```
The `SysTick_Handler` increments `ms_tick` on every interrupt, used by `delay()`.

### I2C Configuration
| Parameter    | Value                                  |
|-------------|----------------------------------------|
| Clock source | APB1 at 36 MHz                        |
| SCL speed    | 100 kHz (standard mode)               |
| CCR          | 180  → 36 MHz / (2 × 100 kHz)        |
| TRISE        | 37   → (36 × 1000 ns) + 1 ticks      |

## RF Signal Classification (FSM)

The firmware reads the RF signal bit-by-bit from **PB0** into a **sliding window of 16 bytes (128 bits)**.
Every time the window fills, it is classified by `classify()`:

| Condition          | State     | Meaning                     |
|-------------------|-----------|-----------------------------|
| All 128 bits = 1   | `JAMMING` | Continuous interference     |
| All 128 bits = 0   | `FAULT`   | No signal / device error    |
| Mixed bits         | `NORMAL`  | Valid RF signal detected    |

```
rf_state values:
  NORMAL  = 0
  JAMMING = 1
  FAULT   = 2
  initial = -1  (not yet classified)
```

## OLED Display

Driver implemented from scratch over I2C for the **SSD1306** controller.

| Screen        | Content                                       |
|--------------|-----------------------------------------------|
| Boot screen   | UNIFEI + UoM logos, project title, 2s delay  |
| JAMMING state | "JAMMING! / RF Interference"                 |
| FAULT state   | "NO Signal / Error device"                   |
| NORMAL state  | "Normal / RF Signal OK"                      |
| System OFF    | "System OFF / Power to turn ON"              |

Includes a **5×8 pixel bitmap font** (ASCII 32–122) and pixel-art logos for UNIFEI and
the University of Manchester rendered directly via I2C data writes.

## Buzzer Logic

The buzzer on **PA1** activates only when `powered == 1` and `rf_state == JAMMING`.
All other states keep PA1 low (buzzer off).

## Power Button

Debounced in software with a **50 ms delay** (double-read pattern).
Toggling the button:
- **ON →** resets window, state, and re-runs boot sequence
- **OFF →** silences buzzer, clears display, shows "System OFF" screen

## Repository Structure

```
rf-firmware-stm32/
├── dual_band_RF.c     # Full firmware: init, I2C, OLED, RF FSM, buzzer, main loop
├── .gitignore         # Excludes Keil build artifacts (Objects/, Listings/, *.axf, etc.)
└── README.md
```

## Build

**Toolchain:** Keil µVision (ARM Cortex-M3 target)  
**Flash:** ST-Link Utility

> No external libraries or HAL. All peripheral access is done via direct register writes
> using the `REG(addr)` macro defined as `(*((volatile unsigned int*)(addr)))`.

## Research Context

| Field        | Details                                             |
|-------------|------------------------------------------------------|
| Institution  | Federal University of Itajuba (UNIFEI)              |
| Group        | Cybersecurity and Critical Systems (CS)²            |
| Supervisors  | Prof. PhD. Otávio Gomes · Prof. PhD. Décio Rennó    |
| Researcher   | Patrícia Rodrigues                                  |
| Created      | April 9, 2026                                       |

> The formal verification model (ESBMC) derived from this firmware logic
> is maintained in a separate private repository — available upon request.

## License

Private repository. All rights reserved.
