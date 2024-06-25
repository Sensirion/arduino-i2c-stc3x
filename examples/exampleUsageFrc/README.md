# Forced Recalibration (FRC) Example

After soldering, during assembly, board-test or at a later stage, a forced recalibration (FRC)
must be performed to ensure performance within specifications.

The STC31-C sensor does not have any non-volatile memory. Therfore the sensor config (state) 
needs to be stored by the micro-controller in non-volatile memory. On a soft reset or power
cylce, those states need to be restored.

The post-soldering FRC procedure is documented in the paragraph 5 of the
[Desing guide](https://sensirion.com/resource/application_note/design_guide_STC31-C)

All used commands and formulas used in this exampel can be found in the
[Datasheet](https://sensirion.com/resource/datasheet/stc31-c)

## Post-soldering Forced Recalibration Process

The post soldering FRC process must be performed in a controlled environment with a
known CO2 concentration in vol%. In this example we use 0.0 vol% which is what we
measure for the atmospheric CO2 concentration. The atmospheric CO2 concentration is
0.04 vol% and as such below the STC31-C accuracy.

The FRC process is done in two loops. In the first loop, we apply the FRC value and
in the second loop we verify that the FRC was successful.

### Measurement Setup
For this example we use the STC31-C evaluation kit, with a STC31-C and SHT40 sensor
on a flex PCB.

* Measurement frequency: 1Hz
* Weak filter enabled (note: the filter configuration is not stored in the state and
  therefore needs to be set separately after each soft-reset or power-cylce).
* automatic self-calibration (ASC) enabled.
* Reference CO2 concentration: 0.0 vol% (atmospheric CO2 concentration)
* Measurement duration for stabilization phase: 20s
* Binary gas mode: 0x13 (19). CO2 concentration between 0 to 40% in air.

The configuration can be changed, by changing the various `#define` that can be found
at the beginning of the [exampleUsageFrc.ino](exampleUsageFrc.ino) file.


### 1st Loop: Forced Recalibration

Before the first loop is started, we send a I2C soft reset command to ensure that
the sensor is in a clean state.

The first loop is implemented in the function `fistFrcLoop()`. It consists of the
following steps.

1. Enable automatic self-calibration (ASC)
2. Set binary gas mode to 0x13
3. Enable weak filter (implemented in the function `configureFilter`)
4. Measure for 20s at 1Hz to stabilize the readings (section 4.1.1 in the Desing Guide).
   humidity and temperature compensation from SHT40 sensor are set before each measurement.
   If you have a pressure sensor on the board, you should read out the absolute pressure value
   and set it as well (commented out code in the `measureAndPrint` function)
5. Apply the FRC with a CO2 reference value of 0.0 vol% (raw value: 16384)
6. Get the sensor state and store it in the memory of the Arduino. This value should be stored
   on non-volatile memory in your product.
7. Trigger an I2C soft reset to initiate the 2nd loop (verify)


### 2nd Loop: Verify

The second loop is implemented in the function `secondFrcLoop()`.

1. Write and apply the sensor state we previously stored. This will restore all configurations
   except for the filters.
2. Enable weak filter (implemented in the function `configureFilter`)
3. Measure for 20s at 1Hz to stabilize the readings (See 1st loop step 4).
4. Take one more measurement and verify that the CO2 concentration is within specification (0.0 vol%).
   In this example we allow a margin of +- 1.0 vol%.

If the CO2 concentration is within specification, the FRC was successful and we can use the saved
state. If the value is out of specification, the FRC process must be repeated.
