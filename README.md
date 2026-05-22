# RF Firmware — Dual-Band RF Cyber Threat Detection Device

Bare-metal C firmware for a dual-band RF detector operating at **13.56 MHz** and **433 MHz** (ISM band), designed for spectrum monitoring and cybersecurity threat detection.

Developed as part of a research at **UNIFEI** (Federal University of Itajubá), in collaboration with Clavis Segurança da Informação, affiliated with the **Cybersecurity and Critical Systems (CS)² group**.

dual_band_RF.c — Main Dual-Band Detection Firmware

## Hardware

| Component           | Details                                     |
|---------------------|---------------------------------------------|
| Microcontroller     | STM32F103C8T6 (ARM Cortex-M3, 72 MHz)       |
| Display             | OLED 128×64 SSD1306 (I2C1)                  |
| RF Input 13.56 MHz  | PA6 — digital RF signal (input pull-up)     |
| RF Input 433 MHz    | PB0 — digital RF signal (input pull-up)     | 
| Buzzer              | PA1 — push-pull output (jamming alert)      |
| Power Button        | PA0 — input pull-up                         |
| Communication       | I2C1 via PB6 (SCL) / PB7 (SDA) at 100 kHz   |
| Programming         | Pure bare-metal C — direct register access  |
| Reference Manual    | RM0008 STM32F10x Reference Manual           |

## Pin Mapping

| Pin  | Function              | Mode                    |
|------|-----------------------|-------------------------|
| PA0  | POWER button          | Input pull-up           |
| PA1  | Buzzer                | Output push-pull 2 MHz  |
| PA6  | RF input — 13.56 MHz  | Input pull-up           |
| PB0  | RF input — 433 MHz    | Input pull-up           |
| PB6  | SCL I2C1              | AF open-drain 50 MHz    |
| PB7  | SDA I2C1              | AF open-drain 50 MHz    |

### Clock Configuration

| Parameter     | Value                              |
|---------------|------------------------------------|
| Clock source  | HSE (external crystal)             |
| PLL config    | HSE × 9 → 72 MHz system clock      |
| APB1 (PCLK1)  | 36 MHz (÷2)                        |
| Flash latency | 2 wait states + prefetch enabled   |

### Timekeeping

SysTick configured for **1 ms resolution** at 72 MHz:
```
LOAD = 72000 - 1 = 71999
```

### I2C Configuration

| Parameter    | Value                             |
|--------------|-----------------------------------|
| Clock source | APB1 at 36 MHz                    |
| SCL speed    | 100 kHz (standard mode)           |
| CCR          | 180 → 36 MHz / (2 × 100 kHz)      |
| TRISE        | 37  → (36 × 1000 ns) + 1 ticks    |

### RF Sampling — TIM3 Interrupt-Driven

Both bands are sampled simultaneously by the **TIM3 interrupt at 100 kHz**:

| Parameter          | Value                                     |
|--------------------|-------------------------------------------|
| Sampling rate      | 100 kHz (TIM3 update interrupt)           |
| Window size        | 1024 samples per classification cycle     |
| Jamming threshold  | > 80% high samples → `RF_JAMMING`         |
| Low threshold      | < 20% high samples → `RF_FAULT`           |
| Confirmation       | 3 consecutive windows before state change |

Inside `TIM3_IRQHandler`, each interrupt reads PA6 (13.56 MHz) and PB0 (433 MHz), increments the respective `high_count`, and sets the `ready` flag when 1024 samples are accumulated. The main loop then calls `rf_process()` for each band independently.

### RF State Machine

Each band has its own independent `rf_detector_t` instance: `g_rf[BAND_1356]` and `g_rf[BAND_433]`.

| State         | Value | Condition                                  |
|---------------|-------|--------------------------------------------|
| `RF_UNKNOWN`  |   0   | Initial state, not yet classified          |
| `RF_NORMAL`   |   1   | Mixed samples — valid RF signal            |
| `RF_JAMMING`  |   2   | > 95% high samples — interference detected |
| `RF_FAULT`    |   3   | < 5% high samples — no signal / fault      |

### OLED Display

The display shows both bands simultaneously:

```
[UNIFEI logo]
13.56 MHz:  [state]
  433 MHz:  [state]
UNIFEI and Clavis
```

| State         | Text displayed  |
|---------------|-----------------|
| `RF_JAMMING`  | `JAMMING!`      |
| `RF_FAULT`    | `FAULT`         |
| `RF_NORMAL`   | `NORMAL`        |
| `RF_UNKNOWN`  | `Waiting...`    |

### Buzzer

PA1 activates if **either band** detects `RF_JAMMING` while `g_powered == 1`. GPIO writes use the atomic **BSRR register** (no read-modify-write).

### Power Button

Debounced with a **30 ms** edge-detection pattern using timestamps from `millis()`. 
- **ON (standby exit) →** resets both detectors, re-enables TIM3 sampling
- **OFF (standby enter) →** disables sampling, silences buzzer, shows "Standby" screen

### Watchdog (IWDG)

Independent Watchdog enabled at startup:

| Parameter   | Value                            |
|-------------|----------------------------------|
| Prescaler   | ÷32                              |
| Reload      | 1250 → ~1.25 s timeout           |
| Kick        | `IWDG_KICK()` in every main loop |

### Fault Flags

`g_system_faults` is a bitmask accumulating hardware errors:

| Flag                    | Bit | Meaning                     |
|-------------------------|-----|-----------------------------|
| `FAULT_CLOCK_HSE`       |  0  | HSE oscillator timeout      |
| `FAULT_CLOCK_PLL`       |  1  | PLL lock timeout            |
| `FAULT_CLOCK_SWITCH`    |  2  | Clock switch timeout        |
| `FAULT_I2C`             |  3  | I2C bus error               |
| `FAULT_OLED`            |  4  | OLED init failure           |
| `FAULT_RF_OVERRUN_1356` |  5  | 13.56 MHz sample overrun    |
| `FAULT_RF_OVERRUN_433`  |  6  | 433 MHz sample overrun      |
| `FAULT_IWDG`            |  7  | IWDG configuration timeout  |

## `generator_test.c` — RF Signal Generator Test Utility

Single-band test firmware used to validate the RF signal generation stage, developed in collaboration with **UNIFEI and Clavis**.

> Shares the same architecture as `dual_band_RF.c` (PLL 72 MHz, TIM3 at 100 kHz, 1024-sample window, 80%/20% thresholds, 3-window confirmation, IWDG, BSRR, WFI) but operates on a **single RF channel** (PB0 only) without the dual-band structure.

## Build

**Toolchain:** Keil µVision (ARM Cortex-M3 target)
**Flash:** ST-Link Utility

> All peripheral access is done via direct register writes using the `REG(addr)` macro: `(*(volatile uint32_t *)(uintptr_t)(addr))`.

## Research Context

| Field        | Details                                             |
|--------------|----------------------------------------------- -----|
| Institution  | Federal University of Itajuba (UNIFEI)              |
| Partner      | Clavis Information Security                         |
| Group        | Cybersecurity and Critical Systems (CS)²            |
| Supervisors  | Prof. PhD. Otávio Gomes · Prof. PhD. Décio Rennó    |
| Researcher   | Patrícia Rodrigues                                  |
| Created      | January 15, 2026                                    |

> The formal verification model (ESBMC) derived from this firmware logic is maintained in a separate private repository — available upon request.

## License

Private repository. All rights reserved.
