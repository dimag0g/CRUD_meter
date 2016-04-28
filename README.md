
CRUD meter for Arduino
======================

CRUD meter lets you perform basic measurements using Arduino. It uses UART
to connect to Serial Monitor, from which it gets commands to set measurement
mode and where it outputs measurement results.

'C', 'R', 'U' and 'D' are the measurement modes which can be selected
by sending the corresponding character, hence the name.

Available measurement modes include capacitance, resistance and voltage measurement with an additional mode
designed to measure voltage drop on diodes, including LEDs.

Schematic
---------

The hardware setup consists of an Arduino and 4 resistors with nominals in x10 increments.

    Arduino
    ────────┐
      PIN_1K├──┤ 1k ├──┐
            │          │
     PIN_10K├──┤ 10k├──┤
            │          │
    PIN_100K├──┤100k├──┤
            │          │
      PIN_1M├──┤ 1M ├──┤
            │          │  (+) (-)
      PIN_IN├──────────┴───Ø   Ø───┐
    ────────┘                     ─┴─
                                  GND

PIN_IN has to be an analog pin (A0 and up), other pins can be chosen freely.

Resistors don't have to have precise values: a deviation of 20% is possible.
For example, you can use anything between 0.8kOhm and 1.2kOhm as 1K resistor.

If you don't have a 1M resistor, you can use a second 100K resistor instead.
You'll get reduced measurement range though: you won't be able to measure
resistance above 1MOhm and capacitance below 10nF.

Calibration
-----------

Before you can use CRUD for actual measurements, you need to calibrate it.
Measure the exact values of the resistors you're about to use (preferably
before soldering if you're going to solder them), and set the calibration
constants to the exact value you have used:

- REF_1K   - to the actual resistance of 1kOhm resistor
- REF_10K  - to the actual resistance of 10kOhm resistor
- REF_100K - to the actual resistance of 100kOhm resistor
- REF_1M   - to the actual resistance of 1MOhm resistor

You will also need to measure the exact voltage on Arduino's VCC pin
and assign that value to the calibration constant REF_V.

Measurement
-----------

After a reset CRUD goes into 'U' mode, where it measures the voltage.
Sending a single letter via the UART will switch to the corresponding mode.

C - Capacitance
---------------

In this mode CRUD measures the RC time constant, trying different resistors from
1k to 1M, until the time constant comes in range. Corresponding pins are driven
HIGH and LOW to change and discharge the capacitor.

Capacitance values in range from approx. 1 nF to 250 uF can be measured.
Zero is displayed for values out of this range.

Note: ceramic caps may require a long time for the measurement to stabilize.
This is due to the fact that such caps are temperature-sensitive, and you have
likely been holding them in your hands just before the measurement.

Warning: polar capacitors (e.g. electrolytic) can be damaged if the polarity
is not respected. Negative terminal should be connected to ground.

R - Resistance
--------------

In this mode CRUD creates voltage dividers trying different resistors from
1k to 1M, until the measured voltage is in range. Corresponding pins are driven
HIGH during the measurement.

Resistance values in range from approx. 10 Ohm to 20 MOhm can be measured.
Zero is displayed for values out of this range.

U - Voltage
-----------

In this mode CRUD measures the voltage on PIN_IN, while load pins are not driven.

Voltages in range from 0 V to REF_V can be measured.

Warning: measured voltage should never be negative or exceed REF_V.
Doing so will permanently damage the controller.

D - Diode voltage drop
----------------------

In this mode CRUD measures the voltage on PIN_IN, while PIN_1K is driven HIGH.

If a diode is connected in forward direction (cathode to ground), a current
of 1-5 mA will flow through it, allowing to measure forward voltage drop of
rectifying diodes and most LEDs.

If a diode is connected in reverse direction, it can be checked for integrity.
For Zener diodes with breakdown voltage below REF_V, the breakdown voltage will
be measured.

Voltages in range from 0 V to 90% of REF_V can be measured.
Zero is displayed for values out of this range.
