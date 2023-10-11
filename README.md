# LJN Video Art cartridge dumper
In 1987, toy company LJN made a home game console called the "[Video Art](https://en.wikipedia.org/wiki/LJN_Video_Art)." It has a handful of games that come on cartridges and is 6805-based.

This repo contains the source for a hardware/software combination to dump these cartridges to ROM files, which can be used in a hypothetical future emulator or clone device.

This device was developed as part of [a Leaded Solder blog](https://www.leadedsolder.com) entry. It cost money and time to develop; if you want to support things like this in the future, please contribute to the Patreon.

## Hardware
### Requirements
You will need:
 - An Arduino Uno or compatible. I used the "Elegoo Uno R3" from Amazon for development and testing;
 - A USB cable;
 - 2x DIP 74HCT595 (74HC595 is probably fine);
 - 1x DIP 74HCT165 (74HC165 is probably fine);
 - The cartridge slot for the LJN cartridge, I used a [TE AMP 5530843-3](https://www.te.com/usa-en/product-5530843-3.html);
 - A length of component leg (e.g. leftover resistor) or some wire;
 - Arduino shield stacking headers (I got mine off Amazon)

### Assembly Instructions
Assembly is a bit tricky, so go shortest to tallest component as you are populating the board. The stacking connectors should go last.

The cartridge connector should be tilted back slightly to make sure there is clearance with the stacking connectors.

Stick a wire or a bent resistor leg through the "0Ω" resistor footprint on the board.

There is no need to populate the two test points TP1 or TP2; these are meant for a beaded test hook, in order to debug the board using a logic probe.

It is challenging to make the stacking connectors go in straight; if you have a better way, please let me know. I used sticky tack/poster putty and bent the pins to mate with the Uno base board.

## Software
### Build instructions
 1. Install Arduino IDE. I developed this with version 2.2.1, but I assume any Arduino editor should work for this purpose.
 2. Open the `.ino` file.
 3. Connect your Arduino Uno (or compatible) to the USB port and upload the firmware to the board. This might require some configuration; check your board vendor for more details.

### Usage instructions
For now, there is no software client for this dumper. Instead, you'll want to use the Serial Monitor from the Arduino IDE.

When the Uno starts up, it should display a message like this:

```
LJN VideoArt ROM Dumper
OE is on pin 10
```

Type `dump` into the text input, press return, and wait for the dump to complete. The monitor should slowly fill with hexadecimal values from the ROM, followed by the message `done.`

Open the `interpreter.py` Python script in your favourite text editor.

Copy and paste the hex values from the Arduino Serial Monitor output into the string `DUMP_STRING`. Make sure none of the old string is there, and make sure it makes a valid Python string (i.e. that it is surrounded by single quotes and is all one line.)

Run the `interpreter.py` Python script. It should generate a file called `rom.bin` that is exactly 16,384 bytes long.
