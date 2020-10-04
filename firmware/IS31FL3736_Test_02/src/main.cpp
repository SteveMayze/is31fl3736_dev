#include <Arduino.h>

#include<Wire.h>
#include<avr/pgmspace.h>
#define Addr_GND_GND 0xa0//AD1 open (R7 pull-low), ADDR2=GND
#define Addr_GND_VCC 0xa6//AD1 = VIO, ADDR2=GND
#define Addr_GND_SCL 0xa2//AD1 = SCL, ADDR2=GND
#define Addr_GND_SDA 0xa4//AD1 = SDA, ADDR2=GND


#define IS31FL3736_LED_RED 1
#define IS31FL3736_LED_GREEN 2
#define IS31FL3736_LED_BLUE 3

static const int heartbeat = 13;

/*
 Calculates the SW register address from the row number
 */
uint8_t IS31FL3736_row_address(int8_t row, uint8_t colour) {
  return (uint8_t) 3 * (row - 1) + colour;
}

uint8_t IS31FL3736_column_bit() {
  return -1;
}

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
  for(i=0;i<0x18;i++) {
    IS_IIC_WriteByte(Addr_GND_GND,i,0xFF);// Enable all LEDs
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
  IS_IIC_WriteByte(Addr_GND_GND,0x01,0x1f);//global current
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
  uint8_t cs[5] = {0x00, 0x01, 0x04, 0x10, 0x40 };

  uint8_t i = 0;
  for ( uint8_t led = 0; led < 0x05; led++) {

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//Unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x00);// Turn to page 0: control registers

    for(i=0; i<0x18; i = i+2) {
      IS_IIC_WriteByte(Addr_GND_GND, i, cs[led]);//open  LED
    }

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    for(i=0; i<192; i++) {
      IS_IIC_WriteByte(Addr_GND_GND, i, 0xA0);//Set PWM data to 0
    }

    digitalWrite( heartbeat, !digitalRead( heartbeat) );
    delay(1000); // wait for a second

  }
}

#define ROWS 12
#define COLUMNS 8
void IS31FL3736_Test_mode4(void)
{

Serial.println("TEST 4 START");
  uint16_t reg[12];
  for ( uint8_t sw=0; sw < ROWS; sw++) {
    reg[sw] = 0x0000;
  }

  IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
  for(uint8_t i=0; i<192; i++) {
    IS_IIC_WriteByte(Addr_GND_GND, i, 0xA0);//Set PWM data to 0
  }

  for ( uint8_t sw =0; sw < ROWS; sw++) {
    reg[sw] = 0x0001;

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5); //Unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x00); // Turn to page 0: control registers

    for ( uint8_t col=0; col < COLUMNS; col++) {
      // reg[sw] = 1 << col*2 | 1 >> (15);
      reg[sw] = 1 << col*2;
      uint8_t lhs = reg[sw] & 0xFF;
      uint8_t rhs = (reg[sw] >> 8) & 0xFF;

      IS_IIC_WriteByte(Addr_GND_GND, sw*2, lhs);
      IS_IIC_WriteByte(Addr_GND_GND, sw*2+1, rhs);

      delay(100);

    }
    IS_IIC_WriteByte(Addr_GND_GND, sw*2, 0x00);
    IS_IIC_WriteByte(Addr_GND_GND, sw*2+1, 0x00);
    reg[sw] = 0x0000;
  }
}



void IS31FL3736_Test_mode5(void)
{
  // uint8_t red[3] = {0xFF, 0x00, 0x00 };
  // uint8_t orange[3] = {0xFF, 0x7D, 0x00 };
  // uint8_t yellow[3] = {0xFF, 0xFF, 0x00 };
  // uint8_t springGreen [3] = {0x7D, 0xFF, 0x00 };
  // uint8_t green[3] = {0x00, 0xFF, 0x00 };
  // uint8_t turquoise[3] = {0x00, 0xFF, 0x7D };
  // uint8_t cyan[3] = {0x00, 0xFF, 0xFF };
  // uint8_t ocean[3] = {0x00, 0x7D, 0xFF };
  // uint8_t blue[3] = {0x00, 0x00, 0xFF };
  // uint8_t violet[3] = {0x7D, 0x00, 0xFF };
  // uint8_t magenta[3] = {0xFF, 0x00, 0xFF };
  // uint8_t raspberry[3] = {0x00, 0x00, 0x7D };

 //                          BLUE  GREEN RED
  uint8_t rainbow[12][3] = {{0x00, 0x00, 0xFF },  //Red
                            {0x00, 0x7D, 0xFF }, // Orange
                            {0x00, 0xFF, 0xFF }, // Yellow
                            {0x00, 0xFF, 0x7D }, // Spring Green
                            {0x00, 0xFF, 0x00 }, // Green
                            {0x7D, 0xFF, 0x00 }, // Turquoise
                            {0xFF, 0xFF, 0x00 }, // Cyan
                            {0xFF, 0x7D, 0x00 }, // Ocean
                            {0xFF, 0x00, 0x00 }, // Blue
                            {0xFF, 0x00, 0x7D }, // Violet
                            {0xFF, 0x00, 0xFF }, // Magenta
                            {0x7D, 0x00, 0xFF }  // Raspberry
                           };

  uint8_t pwm_reg[12][16];
  // for ( uint8_t i=0; i<12; i++) {
  //   for( uint8_t j=0; j<16; j++) {
  //     pwm_reg[i][j] = 0x00;
  //   }
  // }

  for (uint8_t colour = 0; colour<12; colour++){
    for ( uint8_t row = 0; row < 4; row++){
      for( uint8_t column = 0; column < 16; column=column+2){
        pwm_reg[row*3][column] = rainbow[colour][0];
        pwm_reg[row*3+1][column] = rainbow[colour][1];
        pwm_reg[row*3+2][column] = rainbow[colour][2];
      }
    }

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    uint8_t pwmAddr = 0;
    for ( uint8_t i=0; i<12; i++) {
      for( uint8_t j=0; j<16; j=j+2) {
        IS_IIC_WriteByte(Addr_GND_GND, pwmAddr, pwm_reg[i][j]);//Set PWM data to 0
        pwmAddr = pwmAddr+2;
      }
    }
    delay(100);
  }
  // delay(1000);
}

void IS31FL3736_Test_mode6(void)
{
  // uint8_t red[3] = {0xFF, 0x00, 0x00 };
  // uint8_t orange[3] = {0xFF, 0x7D, 0x00 };
  // uint8_t yellow[3] = {0xFF, 0xFF, 0x00 };
  // uint8_t springGreen [3] = {0x7D, 0xFF, 0x00 };
  // uint8_t green[3] = {0x00, 0xFF, 0x00 };
  // uint8_t turquoise[3] = {0x00, 0xFF, 0x7D };
  // uint8_t cyan[3] = {0x00, 0xFF, 0xFF };
  // uint8_t ocean[3] = {0x00, 0x7D, 0xFF };
  // uint8_t blue[3] = {0x00, 0x00, 0xFF };
  // uint8_t violet[3] = {0x7D, 0x00, 0xFF };
  // uint8_t magenta[3] = {0xFF, 0x00, 0xFF };
  // uint8_t raspberry[3] = {0x00, 0x00, 0x7D };

 //                          BLUE  GREEN RED
  uint8_t rainbow[12][3] = {{0x00, 0x00, 0xFF },  //Red
                            {0x00, 0x7D, 0xFF }, // Orange
                            {0x00, 0xFF, 0xFF }, // Yellow
                            {0x00, 0xFF, 0x7D }, // Spring Green
                            {0x00, 0xFF, 0x00 }, // Green
                            {0x7D, 0xFF, 0x00 }, // Turquoise
                            {0xFF, 0xFF, 0x00 }, // Cyan
                            {0xFF, 0x7D, 0x00 }, // Ocean
                            {0xFF, 0x00, 0x00 }, // Blue
                            {0xFF, 0x00, 0x7D }, // Violet
                            {0xFF, 0x00, 0xFF }, // Magenta
                            {0x7D, 0x00, 0xFF }  // Raspberry
                           };

  uint8_t pwm_reg[12][16];
  // for ( uint8_t i=0; i<12; i++) {
  //   for( uint8_t j=0; j<16; j++) {
  //     pwm_reg[i][j] = 0x00;
  //   }
  // }

  for (uint8_t colour = 0; colour<12; colour++) {
    for ( uint8_t row = 0; row < 4; row++) {
      for( uint8_t column = 0; column < 16; column=column+2){
        pwm_reg[row*3][column] = rainbow[colour][0];
        pwm_reg[row*3+1][column] = rainbow[colour][1];
        pwm_reg[row*3+2][column] = rainbow[colour][2];
      }
    }

    IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
    uint8_t pwmAddr = 0;
    for ( uint8_t i=0; i<12; i++) {
      for( uint8_t j=0; j<16; j=j+2) {
        IS_IIC_WriteByte(Addr_GND_GND, pwmAddr, pwm_reg[i][j]);//Set PWM data to 0
        pwmAddr = pwmAddr+2;
      }
    }
    delay(1000);
  }
  delay(1000);
}
void setup()
{
  Wire.begin();
  Wire.setClock(800000); //I2C 800kHz
  Serial.begin(9600);

  pinMode( heartbeat, OUTPUT);

  IS31FL3736_init();

}



void loop()
{
  Serial.println("Starting loop");
  IS31FL3736_Test_mode6();
}
