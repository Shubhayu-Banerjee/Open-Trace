# OpenTrace — RP2040-Based USB Logic Analyzer

OpenTrace is a custom and CHEAP hardware logic-analyzer project built around the Raspberry Pi RP2040. The goal is to capture digital signals from external circuits and inspect their logic levels and timing on a connected computer.

> **Project status:** PCB design in progress. This README describes the current intended design; it is not a claim that the hardware has been fabricated, tested, or validated.

## Goals

- Capture up to **8 digital input channels**.
- Connect to a computer over **USB 2.0 Full-Speed**.
- Use the RP2040's PIO and DMA-capable peripherals for deterministic digital sampling and buffered capture.
- Provide a local display and rotary-encoder controls, subject to final GPIO and PCB-space validation.
- Keep the design affordable and practical to assemble.

## Planned Hardware

| Subsystem | Current design direction | Notes |
|---|---|---|
| MCU | Raspberry Pi RP2040 | Core capture and USB interface |
| External flash | Winbond W25Q16JV | 16 Mbit (2 MB) QSPI flash; verify exact suffix and footprint |
| Clock | 12 MHz crystal | Confirm crystal specifications and load-capacitor values against the RP2040 reference design |
| USB | USB-C receptacle, USB 2.0 D+/D− | CC1 and CC2 each use a 5.1 kΩ pull-down to GND |
| USB ESD | USBLC6-2SC6 | Intended for USB D+/D− protection; verify pinout and PCB layout against the datasheet |
| Input buffer | SN74LVC245A-family, currently SN74LVC245APW | 8-bit buffer/transceiver; intended direction A→B with DIR low and active-low `/OE` low |
| Probe connector | 8 signal inputs | Add/retain a clear ground-reference connection for external probes |
| Power | USB VBUS to 3.3 V regulator | Current schematic includes an AMS1117-3.3 candidate; validate regulator stability, capacitor requirements, current capacity, and thermal performance |
| UI | TFT display and three rotary encoders | Final GPIO assignments and interfaces still need to be locked down |

## Input Interface

The planned signal path is:

```text
External digital probes
        │
        ▼
8-channel probe connector (+ ground reference)
        │
        ▼
SN74LVC245A input buffer (A1–A8)
        │  A→B, DIR = LOW, /OE = LOW
        ▼
RP2040 GPIO inputs (B1–B8)
        │
        ▼
PIO-based sampling and capture firmware
```

The current prototype plan omits a dedicated ESD array on the probe inputs because suitable parts and verified availability have not yet been settled. **The probe inputs are therefore not ESD-protected in the current plan.** This is a known limitation, not a validated production choice.

### Input safety and electrical limits

- Intended for ordinary low-voltage digital logic only.
- Do **not** connect mains, unknown voltages, or signals outside the validated input range.
- Confirm the exact SN74LVC245A manufacturer's datasheet, supply voltage, input-voltage limits, and 5 V-tolerance conditions before use.
- A buffer is not a substitute for overvoltage protection, current limiting, or ESD protection.
- Connect the circuit under test and OpenTrace to a suitable common ground reference.
- Use short probe leads where practical and handle the unprotected inputs carefully to reduce ESD risk.
- If ESD protection is added later, choose it based on standoff voltage, capacitance, transient/clamping behavior, footprint, availability, and the buffer's absolute-maximum ratings—not ESD rating alone.

## USB and Power Notes

- USB-C CC1 and CC2 should each have their own 5.1 kΩ pull-down to GND for a USB 2.0 device/sink configuration.
- Tie the receptacle's USB 2.0 D+ contacts together and D− contacts together as required by the connector pinout, then route through the USB ESD protector before the RP2040.
- Keep the USB ESD device close to the connector with a short ground return.
- USB VBUS is nominally 5 V; do not connect it directly to the RP2040's 3.3 V supply rail.
- Validate the AMS1117-3.3 implementation against the exact regulator datasheet, including required input/output capacitors, dissipation, and available USB current.
- Verify all RP2040 supply pins, the VREG_VIN/VREG_VOUT capacitor connections, decoupling, RUN pull-up/reset circuit, BOOTSEL arrangement, and SWD access against the official hardware design guide.

## PCB and Schematic Checklist

### Core schematic
- [ ] Verify every RP2040 power pin and regulator net against the official reference design.
- [ ] Confirm VREG_VIN and VREG_VOUT capacitor values and connections.
- [ ] Confirm 12 MHz crystal part specifications and reference circuit.
- [x] Add RUN pull-up/reset circuit (verify final schematic).
- [ ] Verify BOOTSEL/flash chip-select startup arrangement.
- [ ] Verify flash part number, pinout, voltage, and footprint.
- [ ] Verify USB-C CC resistors, D+/D− mapping, VBUS, GND, shield, and ESD pinout.
- [ ] Validate 3.3 V regulator stability, heat, and current budget.
- [ ] Finalize the probe connector ground-reference pin/connector.
- [ ] Confirm buffer DIR and `/OE` are not floating.
- [ ] Finalize RP2040 GPIO assignments for the buffer, display, and encoders.
- [ ] Run ERC and resolve all meaningful errors/warnings.

### PCB
- [ ] Assign and verify all footprints against manufacturer package drawings.
- [ ] Place RP2040 decoupling capacitors close to their supply pins.
- [ ] Place crystal and its components close to XIN/XOUT; keep traces short.
- [ ] Place USB ESD protection near the USB-C receptacle.
- [ ] Keep USB D+/D− traces short and routed as a differential pair where practical.
- [ ] Place the input buffer close to the probe connector.
- [ ] Ensure a continuous ground return and suitable ground connection for probes.
- [ ] Run DRC and resolve all manufacturing-critical issues.
- [ ] Review copper clearances, board outline, silkscreen, connector orientation, and mounting holes.
- [ ] Generate and inspect Gerbers, drill files, BOM, and pick-and-place data required by the assembler.

## Firmware Direction

The intended firmware direction is to use RP2040 PIO for predictable input sampling, with buffering/DMA as appropriate, and expose captures to a host computer over USB. Capture rate, maximum sample depth, trigger features, and host software are not yet specified here and should be documented once implemented and tested.

## Official References

- [RP2040 Hardware Design with RP2040](https://datasheets.raspberrypi.com/rp2040/hardware_design_with_rp2040.pdf)
- [RP2040 documentation portal](https://pip.raspberrypi.com/categories/814-rp2040)
- [TI SN74LVC245A product/datasheet](https://www.ti.com/product/SN74LVC245A)
- [ST USBLC6-2SC6 product/datasheet](https://www.st.com/en/protections-and-emi-filters/usbllc6-2.html)

## License

License not yet specified. Add a license before publishing or accepting contributions.
