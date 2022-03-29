# Sorting-Controller-ChibiOS

## Prerequisites

* Install [VSCode](https://code.visualstudio.com/)
* Install the `PlatformIO` extension from the VSCode extension marketplace as described [here](https://docs.platformio.org/en/latest//integration/ide/vscode.html#installation)

## Setup

* Clone this git repository: `git clone https://github.com/aminedj/Sorting-Controller-ChibiOS`
* Select the correct project environment [PlatformIO toolbar](https://docs.platformio.org/en/latest/integration/ide/vscode.html#platformio-toolbar) for your Teensy board, e.g. `teensy41`
* Build project: use `Build` button on the [PlatformIO toolbar](https://docs.platformio.org/en/latest/integration/ide/vscode.html#platformio-toolbar) or shortcut (`ctrl/cmd+alt+b`)
* Upload firmware image to Teensy
  * Connect USB cable to teensy board
    * Use `Upload` button on the [PlatformIO toolbar](https://docs.platformio.org/en/latest/integration/ide/vscode.html#platformio-toolbar) or shortcut (`ctrl/cmd+alt+t`) and select "PlatformIO: Upload"
* Use a terminal program (e.g. [CoolTerm](https://freeware.the-meiers.org/) to connect to the USB serial device

## MAX7300 port expander library usage

The MAX7300 port expander library is contained in the two files `MAX7300.cpp` & `MAX7300.h`

* User `MAX7300()` in your `setup()` section to initialize and detect all port expanders connected
* To set high or low a specific port, use the functions `setHigh(uint8_t port)`/`setLow(uint8_t port)`. The argument `port` is the port number to be set high/low. Keep in mind this port number is proper to the port expander and is not the same as in the pogo pins board
* In a similar manner to set high or low all the available ports, use the function `setAll(uint8_t state)`. The argument `state` is either `1` for high or `0` for low.
* To set high or low all the port of a specific port expander use the function `setAllPortofDevice(uint8_t device, uint8_t state)`. parameter `device` is the HEX address of the port expander to use and `state` is either `1` for high or `0` for low.

## `main.cpp` description and usage

* The main controller code runs using the excellent port of [ChibiOS to the Teensy paltform](https://github.com/greiman/ChRt).
* The code controller is made of 2 user defined threads `sortingTask` and `triggerTask`. Both are statically declared with the same priority (note that priority `NORMALPRIO + 1` is important in this case to run these two threads so they have a higher priority than the `chSetup()` thread) and use a cooperative schedule to yield execution.
