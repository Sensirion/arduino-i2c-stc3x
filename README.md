# Sensirion I²C STC3X Arduino Library

This is the Sensirion STC3X library for Arduino allowing you to
communicate with a sensor of the STC3X family over I²C.

<img src="images/stc3x.png" width="300px">

Click [here](https://sensirion.com/products/catalog/SEK-STC31) to learn more about the Sensirion STC3X sensor family.


The measured gas mixture depends on the STC3x product and configured binary gas.
Please refer to the datasheet and API documentation to get a list of supported
binary gases.



## Supported sensor types

| Sensor name   | I²C Addresses  |
| ------------- | -------------- |
|[STC31-C](https://sensirion.com/products/catalog/STC31)| **0x29**|
|[STC31](https://sensirion.com/products/catalog/STC31)| **0x29**|

The following instructions and examples use a *STC31-C*.



## Installation of the library

This library can be installed using the Arduino Library manager:
Start the [Arduino IDE](http://www.arduino.cc/en/main/software) and open
the Library Manager via

`Sketch` ➔ `Include Library` ➔ `Manage Libraries...`

Search for the `Sensirion I2C STC3X` library in the `Filter
your search...` field and install it by clicking the `install` button.

If you cannot find it in the library manager, download the latest release as .zip file
and add it to your [Arduino IDE](http://www.arduino.cc/en/main/software) via

`Sketch` ➔ `Include Library` ➔ `Add .ZIP Library...`

Don't forget to **install the dependencies** listed below the same way via library
manager or `Add .ZIP Library`

#### Dependencies
* [Sensirion Core](https://github.com/Sensirion/arduino-core)

## Sensor wiring

Use the following pin description to connect your STC3X to the standard I²C bus of your Arduino board:

<img src="images/stc3x-pinout.png" width="300px">

| *Pin* | *Cable Color* | *Name* | *Description*  | *Comments* |
|-------|---------------|:------:|----------------|------------|
| 1 | red | VDD | Supply Voltage | 2.7V to 5.5V
| 2 | yellow | SCL | I2C: Serial clock input |
| 3 | black | GND | Ground |
| 4 | green | SDA | I2C: Serial data input / output |




The recommended voltage is 3.3V.

### Board specific wiring
You will find pinout schematics for recommended board models below:



<details><summary>Arduino Uno</summary>
<p>

| *STC3X* | *STC3X Pin* | *Cable Color* | *Board Pin* |
| :---: | --- | --- | --- |
| VDD | 1 | red | 3.3V |
| SCL | 2 | yellow | D19/SCL |
| GND | 3 | black | GND |
| SDA | 4 | green | D18/SDA |



<img src="images/Arduino-Uno-Rev3-i2c-pinout-3.3V.png" width="600px">
</p>
</details>




<details><summary>Arduino Nano</summary>
<p>

| *STC3X* | *STC3X Pin* | *Cable Color* | *Board Pin* |
| :---: | --- | --- | --- |
| VDD | 1 | red | 3.3V |
| SCL | 2 | yellow | A5 |
| GND | 3 | black | GND |
| SDA | 4 | green | A4 |



<img src="images/Arduino-Nano-i2c-pinout-3.3V.png" width="600px">
</p>
</details>




<details><summary>Arduino Micro</summary>
<p>

| *STC3X* | *STC3X Pin* | *Cable Color* | *Board Pin* |
| :---: | --- | --- | --- |
| VDD | 1 | red | 3.3V |
| SCL | 2 | yellow | ~D3/SCL |
| GND | 3 | black | GND |
| SDA | 4 | green | D2/SDA |



<img src="images/Arduino-Micro-i2c-pinout-3.3V.png" width="600px">
</p>
</details>




<details><summary>Arduino Mega 2560</summary>
<p>

| *STC3X* | *STC3X Pin* | *Cable Color* | *Board Pin* |
| :---: | --- | --- | --- |
| VDD | 1 | red | 3.3V |
| SCL | 2 | yellow | D21/SCL |
| GND | 3 | black | GND |
| SDA | 4 | green | D20/SDA |



<img src="images/Arduino-Mega-2560-Rev3-i2c-pinout-3.3V.png" width="600px">
</p>
</details>




<details><summary>ESP32 DevKitC</summary>
<p>

| *STC3X* | *STC3X Pin* | *Cable Color* | *Board Pin* |
| :---: | --- | --- | --- |
| VDD | 1 | red | 3V3 |
| SCL | 2 | yellow | GPIO 22 |
| GND | 3 | black | GND |
| SDA | 4 | green | GPIO 21 |



<img src="images/esp32-devkitc-i2c-pinout-3.3V.png" width="600px">
</p>
</details>



## Quick Start

1. Install the libraries and dependencies according to [Installation of the library](#installation-of-the-library)

2. Connect the STC3X sensor to your Arduino as explained in [Sensor wiring](#sensor-wiring)

3. Open the `exampleUsage` sample project within the Arduino IDE:

   `File` ➔ `Examples` ➔ `Sensirion I2C STC3X` ➔ `exampleUsage`


   The provided example is working with a STC31-C, I²C address 0x29.
   In order to use the code with another product or I²C address you need to change it in the code of `examples/exampleUsage`.
   You find the list with pre-defined addresses in `src/SensirionI2CSTC3X.h`.


5. Click the `Upload` button in the Arduino IDE or `Sketch` ➔ `Upload`

4. When the upload process has finished, open the `Serial Monitor` or `Serial
   Plotter` via the `Tools` menu to observe the measurement values. Note that
   the `Baud Rate` in the used tool has to be set to `115200 baud`.

## Contributing

**Contributions are welcome!**

We develop and test this driver using our company internal tools (version
control, continuous integration, code review etc.) and automatically
synchronize the master branch with GitHub. But this doesn't mean that we don't
respond to issues or don't accept pull requests on GitHub. In fact, you're very
welcome to open issues or create pull requests :)

This Sensirion library uses
[`clang-format`](https://releases.llvm.org/download.html) to standardize the
formatting of all our `.cpp` and `.h` files. Make sure your contributions are
formatted accordingly:

The `-i` flag will apply the format changes to the files listed.

```bash
clang-format -i src/*.cpp src/*.h
```

Note that differences from this formatting will result in a failed build until
they are fixed.
:

## License

See [LICENSE](LICENSE).