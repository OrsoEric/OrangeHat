**Previous Architecture**  

The previous architecture of Raspberry Pi based electronics and software stack was developed for the [Pi Wars 2020](https://fatherofmachines.blogspot.com/p/pi-wars-2020.html) and [Pi Wars 2021](https://orangebotpiwars2021.blogspot.com/)
Below an architectural overview:  
![OrangeBot Architecture](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Architecture/2021-06-26%20OrangeBot%20Architecture.PNG)  
Below the Pi Wars 2021 Robot:  
![OrangeBot Robot](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Architecture/2020-09-06%20OrangeBot.JPG)  

**OrangeHat Architecture**  

The scope of the OrangeHat project is to consolidate what worked in the previous robot electronics in a PCB and to fix the design flaws encountered.  
 -  Dual channel 3.0A Buck regulator TPS54386 (OK)
 -  Onboard microcontroller AT4809 (OK)
 -  Communication bus (+)
 -  Expansion slots (+)
 -  Servomotors (OK)
 -  Power Monitors (+)
 -  Display (+)

Two versions of the OrangeHat are going to be printed.  

***OrangeHat - Minimal***  

Features Servos and 485 Modbus communication on board. Fixed function to reduce dimensions.  Expandability is through the 485 Modbus 3.5mm Jack connectors.  
![OrangeHat Minimal](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Architecture/2021-07-31%20Architecture%20Minimal.PNG)  

***OrangeHat - MikroBUS***  

Features four MikroBUS expansion slots. It allows the communication expansion of choice, 485 Modbus/CAN. It allows for sensors, like IMU, GPS and actuators 5VDC motors, Servos and others. It's the Hat used to prototype adding/removing functionality through clickboards rather than communication bus.  
![OrangeHat MikroBUS](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Architecture/2021-07-31%20Architecture%20Quad%20MikroBUS.PNG)  


**Power**  

The TPS54386 is good. One 3.0A channel is dedicated to the Raspberry Pi 3B+, the other channel serves the servomotors and expansion boards.  
Power monitoring is to be included to monitor currents and voltages and protect the LIPO battery.  
Input voltage is to support 2S LIPO (6.4V discharged) to 4S LIPO (16.8V charged).  

**Microcontroller**  

Choice was between the AT4809 and the GD32VF103.  
After various considerations, the AT4809 is to be used for the hat, and the Longan Nano board for the slave boards.  
The microcontroller is to communicate with the Raspberry Pi using the Rosserial protocol.

**Communication Bus**  

A communication bus is needed for larger boards that can't fit the OrangeHat and may need to be far away phisically from the Raspberry Pi.  
Choice was between 485 Bus+Modbus or CAN bus. Target speed is 1Mb/s with about a dozen slaves supported.    
485+Modbus is wildly used, slaves are to use this bus.  

**Expansion Slots**  

A difficult choice is to include expansion slots on the OrangeHat or rely on communication bus and slave boards. Expansion boards can be custom, use the Arduino standard or the MikroBUS standard. After much consideration, including a number of MikroBUS expansion slots on the OrangeHat allows for much greater flexibility of the hat. One possible OrangeHat design includes four MikroBUS sockets, allowing the communication bus to be a Click Board too, for unmatched flexibility.  
![Four MikroBUS Sokets](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Architecture/2021-07-31%20IDEA%20Quad%20MikroBUS.PNG)

**ServoMotors**  

Servomotors are convenient. A version of the OrangeHat with fixed hardware includes onboard servomotors. A version of the OrangeHat with four MikroBUS expansion slots won't include Onboard Servos, but will have the beefed up pwoer delivery to support a Click Board with onboard servos.  

**Display**  

The Longan Nano GD32VF103 has an SPI display, but has too few pins to be used as an host on the orangeHat.  
The AT4809 will be connected to an onboard side mounted 16x2 4b display to show power and status information.  
