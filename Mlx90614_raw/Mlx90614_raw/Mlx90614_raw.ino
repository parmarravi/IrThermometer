/*
 All the adresses you might need
 
#define MLX90614_I2CADDR 0x5A
// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x0E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F
#define MLX90614_SLEEP 0xFF 
*/
#include "Wire.h"
#define MLX90614_I2CADDR 0x5A
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_EMISS 0x24

float newEmissivity = 0.98;//for human

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Serial.println("MLX90614_Sensor test");  
  setEmissivity(newEmissivity);
  Serial.println("Emissivity: " + String(readEmissive()));
}
void loop()
{
  Serial.print("Ambient = "); Serial.print(readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(readObjectTempF()); Serial.println("*F");
  Serial.println();
  delay(500);
}

float readEmissive(void) {
  return (readEmiss(MLX90614_EMISS)/65535.0);
}

double readObjectTempF(void) {
  return (readTemp(MLX90614_TOBJ1) * 9 / 5) + 32;
}


double readAmbientTempF(void) {
  return (readTemp(MLX90614_TA) * 9 / 5) + 32;
}

double readObjectTempC(void) {
  return readTemp(MLX90614_TOBJ1);
}


double readAmbientTempC(void) {
  return readTemp(MLX90614_TA);
}

double readTemp(uint8_t reg) 
{
  float temp;
  
 temp = read16(reg);
 temp *= .02;
 temp  -= 273.15;
 return temp;
}

double readEmiss(uint8_t reg) 
{
  float emis;
  
  emis= read16(reg);
  
  return emis;
}

uint8_t setEmissivity(float emis)
{
  // Make sure emissivity is between 0.1 and 1.0
  if ((emis > 1.0) || (emis < 0.1))
    return 0; // Return fail if not
  // Calculate the raw 16-bit value:
  uint16_t ke = uint16_t(65535.0 * emis);
  ke = constrain(ke, 0x2000, 0xFFFF);

  // Write that value to the ke register
  return writeEEPROM(MLX90614_EMISS, (int16_t)ke);
}

uint8_t writeEEPROM(byte reg, int16_t data)
{  
  // Clear out EEPROM first:
  if (I2CWriteWord(reg, 0) != 0)
    return 0; // If the write failed, return 0
  delay(5); // Delay tErase
  
  uint8_t i2cRet = I2CWriteWord(reg, data);
  delay(5); // Delay tWrite
  
  if (i2cRet == 0)
    return 1;
  else
    return 0; 
}
uint8_t I2CWriteWord(byte reg, int16_t data)
{
  uint8_t crc;
  uint8_t lsb = data & 0x00FF;
  uint8_t msb = (data >> 8);
  
  crc = crc8(0, (MLX90614_I2CADDR << 1));
  crc = crc8(crc, reg);
  crc = crc8(crc, lsb);
  crc = crc8(crc, msb);
  
  Wire.beginTransmission(MLX90614_I2CADDR);
  Wire.write(reg);
  Wire.write(lsb);
  Wire.write(msb);
  Wire.write(crc);
  return Wire.endTransmission(true);
}
uint8_t crc8 (uint8_t inCrc, uint8_t inData)
{
  uint8_t i;
  uint8_t data;
  data = inCrc ^ inData;
  for ( i = 0; i < 8; i++ )
  {
    if (( data & 0x80 ) != 0 )
    {
      data <<= 1;
      data ^= 0x07;
    }
    else
    {
      data <<= 1;
    }
  }
  return data;
}

double read16(uint8_t a)
{
  uint16_t ret;
  uint8_t addr =MLX90614_I2CADDR;
  Wire.beginTransmission(addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(false); // end transmission
  
  Wire.requestFrom(addr, (uint8_t)3);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret |= Wire.read() << 8; // receive DATA

  uint8_t pec = Wire.read();

  return ret;
}

