# StirlingEngine

Repository for a low-cost demonstration of a modified desktop stirling engine. An arduino micro is used to measure the engines internal pressure and flywheel angle. This data is then imported into matlab to perform a thermodynamic analysis of the engine to calculate the heat and work.

Front view:

![IMG_6112](https://github.com/clnbtlr/StirlingEngine/assets/125999934/b0d3bbcf-f4f2-4d93-a137-077eb4bfe103)
 
Top view:

<img src="https://github.com/clnbtlr/StirlingEngine/assets/125999934/71b0a3bb-8168-4625-a069-147d9969ebae" width="533">

## Bill of Materials

| Part                                            | Supplier     | Cost each (€) |
| ----------------------------------------------- | ------------ |---------------|
| Stirling Engine                                 | Amazon       | 40            |
| Arduino micro                                   | RS           | 26            |
| Mikro-2755 Angle 3 Breakout board               | RS           | 14            |
| MPXV7002DPT1 Pressure Sensor                    | Farnell      | 18            |
| Perma-Proto small mint tin size breadboard      | Adafruit     | 8             |
| Pnuematic tube adapter M5 Male to Push In 4 mm  | RS           | 3             |
| Diametrically magnetised ring magnet Ø 10/5 mm  | supermagnete | 1             |

An Arduino micro was used as it operates at 5V which is required for the pressure and angle sensors but this could also be substituted for an Arduino Uno.

As well as the above, jumper wires of various lengths, male hearder pins, 4 x M4 screws, and a small length (~ 7 cm) of Ø 4 mm pneumatic tubing are also required.

In addition, some small 3d-printed PLA parts were used. The stl files are included in the .\stlfiles directory.

| 3d part name             | Qty required |
| ------------------------ | ------------ |
| MagnetHolder.stl         | 1            |
| PermaProtoSmallClip.stl  | 2            |

## Assembly
The pressure sensor was soldered to the perma-proto board such that pin 2 (Vs) and pin 3 (GND) are along the positive and negative power rails. 7 male header pins were soldered in row 3, columns 0-6. Jumper wires were then soldered in place to connect the pressure sensor pins and angle 3 breakout board to the header pins, as shown in the front view photo.

The Pinout for the connections to the Arduino micro is follows: 

| Arduino micro            | Angle 3 click/AK7451 | MPXV7002DPT1 | Perma-Proto pin |
| ------------------------ | -------------------- | ------------ | --------------- |
| 5V   | 5V  | Vs   | 2 |
| GND  | GND | GND  | 1 |
| SCK  | SCK | -    | 5 |
| MI   | SDO | -    | 4 |
| MO   | SDI | -    | 3 |
| SS   | CS  | -    | 6 |
| A5   | -   | Vout | 0 |


Zero position needs to be set correctly so during operation, the measured angle increases.
