**RS485 BUS**  

Can bus has a lot of overhead and requires dedicated IC, but has wide adoption in automotive.  
No dominant protocol exist for the RS485 bus.  
RS485 has been choosen for the slaves. Communication protocol is to be choosen at a later stage of the design.  

![RS485 Schematics and initial placement](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-24%20OrangeHat%20RS485.PNG)  
  
**Display**  
The display is side mounted. This solution is elegant as it frees the top of the board and let the display be seen from the side.  
The display is meant top show power and status information of the OrangeHat.  
![OrangeHat Display Mockup](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-25%20OrangeHat%20LCD%20Mockup.jpg)  
Schematics. Pin assigment may change to make routing easier.  
![OrangeHat Display Schematics](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-25%20LCD%20Schematics.PNG)  
Initial Placement. Gives an idea of possible routing and area use.  
![OrangeHat Display Initial Placement](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-25%20LCD%20Placement.PNG)  

**Servo**
Schematics for the Servo section of the board  
- 5.8V 3.0A dedicated power line  
- Power cut from microcontroller  
- 8 servo channels  
- Voltage monitoring  
- Current Monitoring  

Schematics:  
![OrangeHat Servo Schematics](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-25b%20OrangeHat%20Servo%20Schematics.PNG)

Initial placement:  
![OrangeHat Servo Initial Placement](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-07-25b%20OrangeHat%20Servo%20Placement.PNG)

Complete Autorute:  
![](https://raw.githubusercontent.com/OrsoEric/OrangeHat/main/Hardware/OrangeHat/Media/2021-08-10c%20autoroute%20success.PNG)
