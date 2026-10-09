# OpenTrace

OpenTrace is a custom RP2040-based USB logic analyzer designed to capture digital signals from external circuits for inspection on a computer. This repository documents the hardware design completed so far.

<img width="1203" height="889" alt="OpenTrace v1" src="https://github.com/user-attachments/assets/4716ec3f-9eee-4d49-916e-666cc19518c7" />


## What has been designed

### RP2040 core

- Selected the **Raspberry Pi RP2040** as the main MCU.
- Designed the core around an external **W25Q16JV QSPI flash** (16 Mbit / 2 MB).
- Added the 12 MHz crystal oscillator circuit, using the RP2040 reference topology as the design basis.
- Added local supply decoupling capacitors and the RP2040's 3.3 V / 1.1 V regulator-related connections.
- Tied `TESTEN` low.
- Added a `RUN` reset circuit using a 10 kΩ pull-up to +3V3 and a normally-open momentary switch to GND.

### USB-C interface

- Added a USB-C receptacle for USB 2.0 device connectivity.
- Added separate **5.1 kΩ pull-down resistors from CC1 and CC2 to GND**.
- Combined the receptacle's duplicate USB 2.0 contacts correctly: A6/B6 for D+ and A7/B7 for D−.
- Added a **USBLC6-2SC6** for USB D+/D− ESD protection.
- Added an **AMS1117-3.3** regulator stage to derive 3.3 V from USB VBUS in the current schematic.

### Eight-channel input stage

- Selected the **SN74LVC245A-family octal buffer**, with the current schematic using `SN74LVC245APW`.
- Set `DIR` low for A-to-B signal flow.
- Set active-low `/OE` low to enable the outputs.
- Added a 100 nF bypass capacitor at the buffer's supply.
- Added an eight-signal probe connector (`J2`) and wired its eight signals to buffer inputs A1–A8.
- Wired the corresponding B1–B8 outputs to RP2040 GPIOs **GPIO25 through GPIO18**, respectively:

| Probe / buffer channel | RP2040 GPIO |
|---|---:|
| Channel 1 (A1 → B1) | GPIO18 |
| Channel 2 (A2 → B2) | GPIO19 |
| Channel 3 (A3 → B3) | GPIO20 |
| Channel 4 (A4 → B4) | GPIO21 |
| Channel 5 (A5 → B5) | GPIO22 |
| Channel 6 (A6 → B6) | GPIO23 |
| Channel 7 (A7 → B7) | GPIO24 |
| Channel 8 (A8 → B8) | GPIO25 |

The mapping above records the schematic mapping discussed so far; it must still be checked against the final display/encoder pin assignments.

### Probe-input ESD decision

- Evaluated multiple eight-channel and single-/multi-line ESD protection options, including the SP4065-08ATG, TPD4E05U06, DRTR5V0U4S-7, PESD5V0U1BA, PESD5V0L1BA, NUP4114, and PESD5V0C2BDFZ.
- No probe-side ESD protector was selected as a verified solution. The main blocker was finding a part that was available, had a practical footprint, and had transient/clamping characteristics appropriately evaluated against the SN74LVC245A input limits.
- **The current design decision is to omit the dedicated probe-input ESD array for this revision**, rather than add an unverified component. This leaves the exposed probe inputs without dedicated ESD protection and is a known limitation.
- The USBLC6-2SC6 remains assigned to USB D+/D−, not to the probe inputs.

## Important current design details

| Item | Current design |
|---|---|
| Main MCU | Raspberry Pi RP2040 |
| QSPI flash | W25Q16JV, 16 Mbit / 2 MB |
| Clock | 12 MHz crystal circuit |
| USB | USB-C, USB 2.0 |
| USB ESD | USBLC6-2SC6 on D+/D− |
| USB-to-3.3 V regulator | AMS1117-3.3 candidate |
| Probe buffer | SN74LVC245APW |
| Probe inputs | 8 digital channels |
| Probe-to-GPIO mapping | GPIO25, GPIO24, GPIO23, GPIO22, GPIO21, GPIO20, GPIO19, GPIO18 |
| Probe ESD | Omitted for current revision; not protected by a dedicated ESD array |
| Reset | `RUN` with 10 kΩ pull-up and momentary switch to GND |
| Boot configuration | `TESTEN` tied low; flash/BOOTSEL arrangement needs final verification |

## Decisions and trade-offs made

- Chose **RP2040** rather than a more complex FPGA or another MCU for this revision, prioritizing a feasible custom board and PIO-based deterministic sampling.
- Kept the external QSPI flash and 12 MHz crystal as part of the bare-MCU design.
- Used a USB-C connector with the correct device-side CC pull-down approach.
- Added dedicated ESD protection to the USB data lines.
- Chose the SN74LVC245A-family device as the eight-channel input buffer.
- Deferred probe-input ESD protection because candidate parts were unavailable or could not be validated as a complete protection solution within the current design effort.
- Prioritized getting a coherent, manufacturable first PCB over adding a component whose electrical suitability was uncertain.

## Known limitations / items not yet verified

This repository describes the design work completed so far; it does not claim the PCB has been fabricated or tested.
- The probe inputs have no dedicated ESD array in the current revision. Treat them as unprotected inputs.

## References

- [RP2040 hardware design guide](https://datasheets.raspberrypi.com/rp2040/hardware_design_with_rp2040.pdf)
- [RP2040 documentation portal](https://pip.raspberrypi.com/categories/814-rp2040)
- [Texas Instruments SN74LVC245A](https://www.ti.com/product/SN74LVC245A)
- [ST USBLC6-2SC6](https://www.st.com/en/protections-and-emi-filters/usbllc6-2.html)

## License

No license has been specified yet.
