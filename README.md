# StirlingEngine

Repository for a low-cost demonstration of a modified desktop stirling engine. An arduino micro is used to measure the engines internal pressure and flywheel angle. This data is then imported into matlab to perform a thermodynamic analysis of the engine to calculate the heat and work.

Front view:

![IMG_6112](https://github.com/clnbtlr/StirlingEngine/assets/125999934/b0d3bbcf-f4f2-4d93-a137-077eb4bfe103)
 
Top view:

<img src="https://github.com/clnbtlr/StirlingEngine/assets/125999934/71b0a3bb-8168-4625-a069-147d9969ebae" width="533">

## Bill of Materials

| Part                                            | Supplier     | Cost, each (€)|
| ----------------------------------------------- | ------------ |---------------|
| Stirling Engine                                 | Amazon       | 40            |
| Arduino micro                                   | RS           | 26            |
| Mikro-2755 Angle 3 Breakout board               | RS           | 14            |
| MPXV7002DPT1 Pressure Sensor                    | Farnell      | 18            |
| Perma-Proto small mint tin size breadboard      | Adafruit     | 8             |
| Pnuematic tube adapter M5 Male to Push In 4 mm  | RS           | 3             |
| Diametrically magnetised ring magnet Ø 10/5 mm  | supermagnete | 1             |

An Arduino micro was used as it operates at 5V which is required for the pressure and angle sensors but this could also be substituted for an Arduino Uno.

As well as the above, jumper wires of various lengths, male hearder pins, 4 x M3 screws, and a small length (~ 7 cm) of Ø 4 mm pneumatic tubing are also required.

In addition, some small 3d-printed PLA parts were used. The stl files are included in the .\stlFiles directory.

| 3d part name             | Qty required |
| ------------------------ | ------------ |
| MagnetHolder.stl         | 1            |
| PermaProtoSmallClip.stl  | 2            |

## Assembly

The pressure sensor was soldered to the perma-proto board such that pin 2 (Vs) and pin 3 (GND) are along the positive and negative power rails. 7 male header pins were soldered in row 3, columns 0-6. Jumper wires were then soldered in place to connect the pressure sensor pins and angle 3 breakout board to the header pins, as shown in the front view photo.

The pinout for the connections is as follows: 

| Arduino micro            | Angle 3 click/AK7451 | MPXV7002DPT1 | Perma-Proto pin |
| ------------------------ | -------------------- | ------------ | --------------- |
| 5V   | 5V  | Vs   | 2 |
| GND  | GND | GND  | 1 |
| SCK  | SCK | -    | 5 |
| MI   | SDO | -    | 4 |
| MO   | SDI | -    | 3 |
| SS   | CS  | -    | 6 |
| A5   | -   | Vout | 0 |

A M5 hole was drilled and taped into the top plate of the engine and the pneumatic adapter with gasket was screwed in place.

The MagnetHolder 3d-printed part acts as a bushing between the ring magnet and the shaft of the flywheel. The magnet was glued onto the 3d-printed part before being pushed onto the shaft up to the cam. See top view photo. No glue was used as the PLA part is intended to be a tight fit.

The 2 x PermaProtoSmallClip 3d-printed parts are used with the 4 x M3 screws to secure the soldered breadboard to the metal vertical bracket. The PLA parts are designed to be self-taped by the M3 screws. Ensure that the AK7451 chip on the Angle 3 click board is lined up with the flywheel axis of rotation, and tighten the M3 screws sufficiently so everything doesn't move.

Connect the Ø 4 mm pneumatic tubing to the MPXV7002DPT1 pressure sensor port closest to its top surface (port P1 in its datasheet) and the pneumatic adapter in the top plate.

## Arduino Code

The zero position of the angle sensor needs to be set correctly so that during operation, the measured angle increases. The zero position is the angle reading corresponding to the highest point of piston in degrees. Using the Arduino IDE, uncomment the Serial.print(angle) line in the stirlingEngine.ino sketch and upload it to the Arduino micro.

Slowly rotate the flywheel by hand until the piston is at its maximum height and read the angle value in the Serial monitor and then update the zeroPos variable. The Serial.print(angle) line can be commented out now again. Update the geometric variables in the script if necessary for a different model of stirling engine. Upload the updated sketch to the Arduino micro again.

The real-time values for the pressure and volume can now be monitored in the Serial Plotter window.

## Running a test

Fill a mug with boiling water from a kettle. Measure its temperature (i.e. the hot reservoir temperature) using a thermometer/thermistor/thermocouple. Measure ambient temperature (i.e. the cold reservoir temperature). Place the stirling engine on top of the mug and wait a few minutes for the hot plate to heat up. Then by hand, spin the flywheel and it should start running freely. Direction of rotation is clockwise in the photos above.

The almost sinusodial change in pressure and volume over time should be observed in the Ardunio IDE serial plotter window. The pressure signal leads the volume signal.

## Data analysis with Matlab

In the Matlab script readStirlingData.m (tested on r2019b and r2023a), the measurements from the arduino can be streamed using serial port communication using the Instrument Control Toolbox (in the absence of this toolbox, the data from the Arduino serial port could also be recorded to a txt file using PuTTY and then imported into Matlab). The gauge pressure and volume change data for ~12 seconds or ~30 cycles are saved in a .mat file for loading. Two .mat files of data from the arduino are provided in this repository:

| .mat file name           | Description                               |
| ------------------------ | ----------------------------------------- |
| arduinoData.mat          | P and V data from MPXV7002 sensor         |
| arduinoData1.mat         | P and V data from PSE533 reference sensor |

The values of pressure were sligtly different between the two sensors, most likely as a result of sensor accuracy and precision. Either set of data can be used in the analysis.

The matlab script first section plots the time series data from the arduino similar to the Arduino IDE serial plotter. 

It then presents this data as a P-V diagram, with the values for pressure converted from gauge to absolute pressure, and the volume converted from dV to specific volume.
