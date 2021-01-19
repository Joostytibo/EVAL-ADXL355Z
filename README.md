# EVAL-ADXL355Z
Evalualtion board of ADXL355Z from Analog Digital.
This code is programmed in Arduino for EVAL-ADXL355Z from Analog Devices.
It's using the "Wire" library to make it work.
The device is on pull_up that makes the address 0x53, else it is using address 0x1D.

 ↓ Change address "0x53" to "0x1D" in the code if you want to use pull_down on the device ↓
 
    // ------------------------- Declare Global Variables ------------------------- // 
    int ADXL355 = 0x53;                     // ADXL355 I2C address - 0x53

change to:

    // ------------------------- Declare Global Variables ------------------------- // 
    int ADXL355 = 0x1D;                     // ADXL355 I2C address - 0x1D
    
The device also uses I2C to communicate, you need 2 wires for communication and 2 wires for power.

2 wires from SDA and SCL
  - SDA to Arduino pin A4
  - SCL to Arduino pin A5
  
2 wires from VDD and GND
  - VDD to Arduino pin 3.3V
  - GND to Arduino pin GND
      
Make MISO/SDA pin HIGH to establish a pull_up on the device
