// CAN Send Example
//

#define INPUT_PIN A0

#include <mcp_can.h>
#include <SPI.h>

int sensor_data;

MCP_CAN CAN0(10);     // Set CS to pin 10

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{

  sensor_data=analogRead(INPUT_PIN);
  data[0]=sensor_data%256;
  data[1]=sensor_data>>8;
  
  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x230, 0, 2, data);
  if(sndStat == CAN_OK){
    Serial.println(sensor_data);
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(30);   // send data per 100ms
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
