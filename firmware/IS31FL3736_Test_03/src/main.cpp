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


#define ROWS 4
#define COLUMNS 8

#define BLUE_LED 0
#define GREEN_LED 1
#define RED_LED 2


uint8_t mergeValue(uint8_t initialValue, uint8_t goalValue, uint8_t buffer) {
  uint8_t result = 0x00;
  if( initialValue == goalValue) {
    result = goalValue;
  }
  if( initialValue < goalValue) {
    result = buffer + 1;
  }
  if ( initialValue > goalValue ) {
    result = buffer - 1;
  }
  return result;
}

boolean mergeValues(uint8_t *initialValue, uint8_t *goalValue, uint8_t *buffer) {
  buffer[RED_LED] = mergeValue(initialValue[RED_LED], goalValue[RED_LED], buffer[RED_LED]);
  buffer[GREEN_LED] = mergeValue(initialValue[GREEN_LED], goalValue[GREEN_LED], buffer[GREEN_LED]);
  buffer[BLUE_LED] = mergeValue(initialValue[BLUE_LED], goalValue[BLUE_LED], buffer[BLUE_LED]);

  return buffer[RED_LED] == goalValue[RED_LED] &&
         buffer[GREEN_LED] == goalValue[GREEN_LED] &&
         buffer[BLUE_LED] == goalValue[BLUE_LED];
}

#define RED 0
#define ORANGE 1
#define YELLOW 2
#define SPRING_GREEN 3
#define GREEN 4
#define TURQUOISE 5
#define CYAN 6
#define OCEAN 7
#define BLUE 8
#define VIOLET 9
#define MAGENTA 10
#define RASPBERRY 11

void IS31FL3736_Test_rainbow(void)
{

  // uint8_t RED[3] = {0x00, 0x00, 0xFF };
  // uint8_t ORANGE[3] = {0x00, 0x00, 0xFF };
  // uint8_t YELLOW[3] = {0x00, 0x00, 0xFF };
  // uint8_t SPRING_GREEN[3] = {0x00, 0x00, 0xFF };
  // uint8_t GREEN[3] = {0x00, 0x00, 0xFF };
  // uint8_t TURQUOISE[3] = {0x00, 0x00, 0xFF };
  // uint8_t CYAN[3] = {0x00, 0x00, 0xFF };
  // uint8_t OCEAN[3] = {0x00, 0x00, 0xFF };
  // uint8_t BLUE[3] = {0x00, 0x00, 0xFF };
  // uint8_t VIOLET[3] = {0x00, 0x00, 0xFF };
  // uint8_t MAGENTA[3] = {0x00, 0x00, 0xFF };
  // uint8_t RASPBERRY[3] = {0x00, 0x00, 0xFF };


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

   for ( uint8_t i=0; i<12; i++) {
     for( uint8_t j=0; j<16; j++) {
       pwm_reg[i][j] = 0x00;
     }
   }


  uint8_t raddr = 0;
  uint8_t caddr = 0;
  // For each colour, we want to transform into the next.
  uint8_t *initialValue = rainbow[RASPBERRY];
  uint8_t *goalValue;
  uint8_t current_buff[3];
  for (uint8_t colour = 0; colour < 12; colour++) {
    digitalWrite( heartbeat, !digitalRead( heartbeat) );
    goalValue = rainbow[colour];
    boolean finished = false;
    while( !finished) {
      for ( uint8_t row = 0; row < ROWS; row++) {
        for( uint8_t column = RED; column < COLUMNS; column++){
          raddr = row*3;
          caddr = column *2;
          current_buff[RED_LED] = pwm_reg[raddr + RED_LED][caddr];
          current_buff[GREEN_LED] = pwm_reg[raddr + GREEN_LED][caddr];
          current_buff[BLUE_LED] = pwm_reg[raddr + BLUE_LED][caddr];

          finished = mergeValues(initialValue, goalValue, current_buff);

          pwm_reg[raddr + RED_LED][caddr] = current_buff[RED_LED];
          pwm_reg[raddr + GREEN_LED][caddr] = current_buff[GREEN_LED];
          pwm_reg[raddr + BLUE_LED][caddr] = current_buff[BLUE_LED];

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
      initialValue = rainbow[colour];
      delay(10);
    }
  }
}

uint16_t incrPhase(uint16_t phase) {
  uint16_t newPhase = phase + 1;
  if(newPhase > 0x5FF)
    newPhase = 0x0000;
  return newPhase;
}


void x_nextPhase(uint16_t *phase, uint8_t *pwm) {
  // RED
  uint16_t c_phase = phase[RED_LED];
  if ( 0x0000 <= c_phase &&  c_phase < 0x0100 ) {
    pwm[RED_LED] = 0xFF;
  } else if ( 0x100 <= c_phase &&  c_phase < 0x0200 ) {
    pwm[RED_LED] = 0x0200 - phase[RED_LED];
  } else if ( 0x200 <= c_phase &&  c_phase < 0x0400 ) {
    pwm[RED_LED] = 0x00;
  } else if ( 0x400 <= c_phase &&  c_phase < 0x0500 ) {
    pwm[RED_LED] =c_phase - 0x0400;
  } else if ( 0x500 <= c_phase ) {
    pwm[RED_LED] = 0xFF;
  }
  phase[RED_LED] = incrPhase(phase[RED_LED]);
  if(phase[RED_LED] == 0x0000 ){
    pwm[RED_LED] = 0xFF;
  }
  // GREEN
  c_phase = phase[GREEN_LED];
  if ( 0x0000 <= c_phase &&  c_phase < 0x0100 ) {
    pwm[GREEN_LED] = c_phase;
  } else if ( 0x100 <= c_phase &&  c_phase < 0x0300 ) {
    pwm[GREEN_LED] = 0xFF;
  } else if ( 0x300 <= c_phase && c_phase < 0x0400 ) {
    pwm[GREEN_LED] = 0x0400 - c_phase;
  } else if ( 0x400 <= c_phase ) {
    pwm[GREEN_LED] = 0x00;
  }
  phase[GREEN_LED] = incrPhase(phase[GREEN_LED]);
  if(phase[GREEN_LED] == 0x0000 ){
    pwm[GREEN_LED] = 0x00;
  }
  // BLUE
  c_phase = phase[BLUE_LED];
  if ( 0x0000 <= c_phase &&  c_phase < 0x0200 ) {
    pwm[BLUE_LED] = 0x00;
  } else if ( 0x200 <=c_phase &&  c_phase < 0x0300 ) {
    pwm[BLUE_LED] = phase[BLUE_LED] - 0x200;
  } else if ( 0x300 <= c_phase &&  c_phase < 0x0500 ) {
    pwm[BLUE_LED] = 0xFF;
  } else if ( 0x500 <= c_phase ) {
    pwm[BLUE_LED] = 0x500 - c_phase;
  }
  phase[BLUE_LED] = incrPhase(phase[BLUE_LED]);
  if(phase[BLUE_LED] == 0x0000 ){
    pwm[BLUE_LED] = 0x00;
  }
}

void nextPhase(uint16_t *phase, uint8_t *pwm) {
  // RED
  uint16_t c_phase = phase[RED_LED];
  if ( c_phase == 0x0000 ) {
    delay(1000);
  }
  if ( c_phase == 0x0200 ) {
    delay(1000);
  }
  if ( c_phase == 0x0400 ) {
    delay(1000);
  }
  if ( 0x0000 <= c_phase &&  c_phase < 0x0100 ) {
    pwm[RED_LED] = 0xFF;
    if (pwm[GREEN_LED] < 0xFF)
      pwm[GREEN_LED]++;
    pwm[BLUE_LED] = 0x00;
  } else if ( 0x100 <= c_phase &&  c_phase < 0x0200 ) {
    if (pwm[RED_LED] > 0)
      pwm[RED_LED]--;
    pwm[GREEN_LED] = 0xFF;
    pwm[BLUE_LED] = 0x00;
  } else if ( 0x200 <= c_phase &&  c_phase < 0x0300 ) {
    pwm[RED_LED] = 0x00;
    pwm[GREEN_LED] = 0xFF;
    if (pwm[BLUE_LED] < 0xFF)
      pwm[BLUE_LED]++;
  } else if ( 0x300 <= c_phase &&  c_phase < 0x0400 ) {
    pwm[RED_LED] = 0x00;
    if (pwm[GREEN_LED] > 0)
      pwm[GREEN_LED]--;
    pwm[BLUE_LED] = 0xFF;
  } else if ( 0x400 <= c_phase &&  c_phase < 0x0500  ) {
    if (pwm[RED_LED] < 0xFF)
      pwm[RED_LED]++;
    pwm[GREEN_LED] = 0x00;
    pwm[BLUE_LED] = 0xFF;
  }  else if ( 0x500 <= c_phase ) {
    pwm[RED_LED] = 0xFF;
    pwm[GREEN_LED] = 0x00;
    if (pwm[BLUE_LED] > 0)
      pwm[BLUE_LED]--;
  }

  phase[RED_LED] = incrPhase(phase[RED_LED]);
}


void IS31FL3736_Test_rainbow_slow(void)
{
  uint8_t pwm_reg[12][16];

  // for ( uint8_t i=0; i<12; i++) {
  //   for( uint8_t j=0; j<16; j++) {
  //     pwm_reg[i][j] = 0x00;
  //   }
  // }
  //
  uint16_t phase[3];
  uint8_t pwm[3];
  phase[RED_LED] = 0x0000;
  phase[GREEN_LED] = 0x0000;
  phase[BLUE_LED] = 0x0000;
  pwm[RED_LED] = 0xFF;
  pwm[GREEN_LED] = 0x00;
  pwm[BLUE_LED] = 0x00;
  uint8_t raddr = 0;
  uint8_t caddr = 0;

  while(1) {
    for ( uint8_t row = 0; row < ROWS; row++) {
      for( uint8_t column = RED; column < COLUMNS; column++){
        raddr = row*3;
        caddr = column *2;

        pwm_reg[raddr + RED_LED][caddr] = pwm[RED_LED];
        pwm_reg[raddr + GREEN_LED][caddr] = pwm[GREEN_LED];
        pwm_reg[raddr + BLUE_LED][caddr] = pwm[BLUE_LED];

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
    // delay(100);
    nextPhase(phase, pwm);
  }
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
  IS31FL3736_Test_rainbow_slow();
}
