# Ohmmeter2.0

## 3D RGB Resistor

First the cables are soldered to the LEDs and the LED is glued on the piece inside the band. Each of the bands are mounted and glued together to build the resistance.

In the space between the bands, the white reference LEDs are soldered in parallel.

All cables are passed from one to another so that they all exit through the last.

Finally the two ends are glued and the resistance is finished.

## Assembling the Box

After printing all the pieces, we passed the resistance wires through the hole and soldered the terminations and the wires of the LEDs and the potentiometer.

The pcb is soldered according to the diagram and we connect all the wires.

## Programming

A state machine has been implemented to differentiate the modes.

### 'A' mode - Value to color
A value is entered by keyboard, it goes through an algorithm that returns its commercial value and displays it in the 3D resistor.

The program remains on active standby until a keyboard event arrives and if it is different from 'D' it shows the value on the screen and saves it in an array.
When pressing 'D', the array with the resistor value is passed to the "valor_a_comercial" function that converts the value into a commercial value and then, this array is passed to the "mostrar_resistencia" function that turns on the RGB LEDs of the 3D resistor with the right colors.

### 'B' mode - Color to value
The colors are entered and the value is displayed on the screen and the colors in the 3D resistor.

A legend is shown on the screen with the values and the corresponding number. The program saves the colors coded with numbers in an array and when pressing 'D' it shows the resistor value. While the colors are being entered, the RGB led of the corresponding band lights up.

### 'C' mode - Ohmmeter
This mode reads through the terminals and shows the value on the screen and the colors in the 3D resistor.

In order to measure various ranges, a potentiometer has been used. In a loop, the value of the potentiometer is read and this value is used as a reference to measure the connected resistor.
This process is repeated every programmable time (600 ms).

## Supplies:
- Arduino Mega

- 4 RGB Leds

- 3 Red Leds

- 3 White Leds

- 4 x 4 Membrane Switch Keypad

- OLED Display I2C 128 x 64 pixels and 0.96 Inch

- 2 connection terminals for resistors

- Potentiometer 50kΩ

- Resistors

    - 1kΩ x1
    - 220Ω x3
    - 22kΩ x1
    - 68Ω x1
- Pre-drilled plate

- 4 screws
