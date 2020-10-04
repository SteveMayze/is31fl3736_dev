#include <Arduino.h>

#include<Wire.h>
#include<avr/pgmspace.h>
#define Addr_GND_GND 0xa0//AD1 open (R7 pull-low), ADDR2=GND
#define Addr_GND_VCC 0xa6//AD1 = VIO, ADDR2=GND
#define Addr_GND_SCL 0xa2//AD1 = SCL, ADDR2=GND
#define Addr_GND_SDA 0xa4//AD1 = SDA, ADDR2=GND

void IS_IIC_WriteByte(uint8_t Dev_Add, uint8_t Reg_Add, uint8_t Reg_Dat)
{
  Wire.beginTransmission(Dev_Add/2); // transmit to device address
  Wire.write(Reg_Add); // sends register address
  Wire.write(Reg_Dat); // sends register data
  Wire.endTransmission(); // stop transmitting
}


void IS31FL3736_init(void)//white LED
{
  uint8_t i;
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
  IS_IIC_WriteByte(Addr_GND_GND,0x00,0x00);//Enable software shutdown
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x00);// Turn to page 0: control registers
  for(i=0;i<0x18;i=i+1) {
    IS_IIC_WriteByte(Addr_GND_GND,i,0x00);//open all LED
  }
  //can use buffer write type as figure 7 in datasheet
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
  for(i=0;i<192;i++) {
    IS_IIC_WriteByte(Addr_GND_GND,i,0x00);//Set PWM data to 0
  }
  //can use buffer write type as figure 7 in datasheet
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
  IS_IIC_WriteByte(Addr_GND_GND,0x00,0x01);//Release software shutdown to normal operation
  IS_IIC_WriteByte(Addr_GND_GND,0x01,0xff);//global current
}


void IS31FL3736_Test_mode1(void)//white LED
{

  Serial.print("IS31FL3736_Test_mode1 Starting");

  uint8_t i;
  while(1)
  {
    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    for(i=0;i<192;i++)IS_IIC_WriteByte(Addr_GND_GND,i,0x10);//update all PWM with 0x10
    delay(1000); // wait for a second
    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    for(i=0;i<192;i++)IS_IIC_WriteByte(Addr_GND_GND,i,0x80); //update all PWM with 0x80
    delay(1000); // wait for a second
  }
}

void IS31FL3736_Test_mode2(void)
{
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
  for ( uint8_t pwm = 0; pwm < 0xFF; pwm++ ) {
    for(uint8_t i=0;i<192;i++) {
      IS_IIC_WriteByte(Addr_GND_GND,i, pwm);//update all PWM with 0x10
    }
    // delay(1000); // wait for a second
  }
}

void IS31FL3736_Test_mode3(void)
{
  uint8_t pin[16] = {0x00, 0x01, 0x04, 0x05, 0x10, 0x11, 0x14, 0x15, 0x40, 0x41, 0x44, 0x45, 0x50, 0x51, 0x54, 0x55 };

  uint8_t i = 0;
  for ( uint8_t led = 0; led < 0x10; led++) {
    // IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
    // IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
    // IS_IIC_WriteByte(Addr_GND_GND,0x00,0x00);//Enable software shutdown
    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x00);// Turn to page 0: control registers

    for(i=0; i<0x18; i++) {
      IS_IIC_WriteByte(Addr_GND_GND, i, pin[led]);//open  LED
    }

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    for(i=0; i<192; i++) {
      IS_IIC_WriteByte(Addr_GND_GND, i, 0xA0);//Set PWM data to 0
    }
    //can use buffer write type as figure 7 in datasheet
    // IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    // IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
    // IS_IIC_WriteByte(Addr_GND_GND,0x00,0x01);//Release software shutdown to normal operation
    // IS_IIC_WriteByte(Addr_GND_GND,0x01,0x7F);//global current
    delay(1000); // wait for a second

  }
}

void IS31FL3736_Test_mode4(void)
{
  // IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
  // IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
  // IS_IIC_WriteByte(Addr_GND_GND,0x00,0x00);//Enable software shutdown
  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x00);// Turn to page 0: control registers
  IS_IIC_WriteByte(Addr_GND_GND, 0x06, 0x01);// SW1, CS1

  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
  for(uint8_t i=0; i<192; i++) {
    IS_IIC_WriteByte(Addr_GND_GND, i, 0xA0);//Set PWM data to 0
  }
  // IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  // IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x03);//Turn to page 3: function registers
  // IS_IIC_WriteByte(Addr_GND_GND,0x00,0x01);//Release software shutdown to normal operation
  // IS_IIC_WriteByte(Addr_GND_GND,0x01,0x7F);//global current
  delay(10000); // wait for a second

}

void setup()
{
  Wire.begin();
  Wire.setClock(800000);//I2C 800kHz
  Serial.begin(9600);

  IS31FL3736_init();

}



void loop()
{
  IS31FL3736_Test_mode4();
}
