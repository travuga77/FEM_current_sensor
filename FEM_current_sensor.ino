// CAN Send Example
//

#define INPUT_PIN A2
#define CAN0_INT 2
#define GAIN     0.26096        // GAIN factors


// Calibrated valiables
#define X_ZERO 4
#define Y_ZERO -12
#define Z_ZERO 266

#include <mcp_can.h>
#include <SPI.h>
#include <SparkFun_ADXL345.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
long current_collect;
long x_collect, y_collect, z_collect;
int current_send;
int count;
int x_send = 0;
int y_send = 0;
int z_send = 0;
bool send_flag=false;

ADXL345 adxl = ADXL345();           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
MCP_CAN CAN0(10);     // Set CS to pin 10

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) 
    Serial.println("MCP2515 Initialized Successfully!");
  else 
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted

  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(2);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity
                                      
  adxl.setSpiBit(0);                // Configure the device: 4 wire SPI mode = '0' or 3 wire SPI mode = 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 

 /* adxl.readAccel(&a, &b, &c);

  long timer = 0;
  long a_collect = 0,
      b_collect = 0,
      c_collect = 0;
  
  timer=millis();
  while (millis()-timer <= 5000) {
    adxl.readAccel(&a, &b, &c);
    a_collect+=a;
    b_collect+=b;
    c_collect+=c;
    Serial.print("a= "); Serial.print(a_collect); 
    Serial.print(" b= "); Serial.print(b_collect); 
    Serial.print(" c= "); Serial.println(c_collect);
    count++;
  }
  a=a_collect/count;
  b=b_collect/count;
  c=c_collect/count;

  Serial.print("a= "); Serial.print(a); 
  Serial.print(" b= "); Serial.print(b); 
  Serial.print(" c= "); Serial.println(c);
  Serial.print(" count= "); Serial.println(count);
  count=0;
  timer=0; */
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{
  // Get the Accelerometer Readings
  int x,y,z;                          // init variables hold results
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store in variables x,y,z
  
  x_collect+=x;
  y_collect+=y;
  z_collect+=z;
  current_collect+=analogRead(INPUT_PIN);
  count++;
  //Serial.print(z_collect);
  //Serial.print(" ");
  //Serial.println(count);

  if (!digitalRead(CAN0_INT)) {                       // If CAN0_INT pin is low, read receive buffer
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s) 
  }
  switch (rxId) {
    case 0x80:
      // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
      current_send=current_collect/count;
      x_send=x_collect/count;
      y_send=y_collect/count;
      z_send=z_collect/count;
      x_send =-(x_send - X_ZERO)/GAIN;         // Calculating New Values for X, Y and Z
      y_send = (y_send - Y_ZERO)/GAIN;
      z_send = (z_send - Z_ZERO)/GAIN;
      data[0]=current_send%256;
      data[1]=current_send>>8;
      data[2]=x_send%256;
      data[3]=x_send>>8;
      data[4]=y_send%256; 
      data[5]=y_send>>8;
      data[6]=z_send%256;
      data[7]=z_send>>8;
      Serial.print("x= "); Serial.print(x_send); 
      Serial.print(" y= "); Serial.print(y_send); 
      Serial.print(" z= "); Serial.println(z_send);
      Serial.print(current_collect);
      Serial.print(" 1111 ");
      Serial.print(count);
      Serial.print(" OLOLOLOLOL ");
      Serial.println(current_send);
      rxId=count=current_collect=x_collect=y_collect=z_collect=0;
      send_flag=true;
      break;  
    default:
      break;
  }
  
  if (send_flag==true) {
      byte sndStat = CAN0.sendMsgBuf(0x230, 0, 8, data);
    if(sndStat == CAN_OK){
    } else {
      Serial.println("Error Sending Message...");
    }
    send_flag=false;
  }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
