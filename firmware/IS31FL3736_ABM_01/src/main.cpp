#include <Arduino.h>

#include<Wire.h>
#include<avr/pgmspace.h>
#define Addr_GND_GND 0xa0//AD1 open (R7 pull-low), ADDR2=GND
#define Addr_GND_VCC 0xa6//AD1 = VIO, ADDR2=GND
#define Addr_GND_SCL 0xa2//AD1 = SCL, ADDR2=GND
#define Addr_GND_SDA 0xa4//AD1 = SDA, ADDR2=GND

#define DISPLAY_ADDR Addr_GND_GND

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


#define IS31FL3736_LED_RED 1
#define IS31FL3736_LED_GREEN 2
#define IS31FL3736_LED_BLUE 3

#define ROWS 4
#define COLUMNS 8

#define BLUE_LED 0
#define GREEN_LED 1
#define RED_LED 2




uint8_t pwm_reg[12][16];
uint8_t abm_reg[12][16];
uint8_t abm_ctl_reg[12];

uint8_t raddr = 0;
uint8_t caddr = 0;

#define ABMCR_3_LTA 0x00
#define ABMCR_3_LTB 0x00
#define ABMCR_3_LB 0x04
#define ABMCR_3_LE 0x06

// PG 3

#define IS31FL3736_CFG     0x00
#define IS31FL3736_GCC     0x01
#define IS31FL3736_ABM_1_1 0x02
#define IS31FL3736_ABM_1_2 0x03
#define IS31FL3736_ABM_1_3 0x04
#define IS31FL3736_ABM_1_4 0x05
#define IS31FL3736_ABM_2_1 0x06
#define IS31FL3736_ABM_2_2 0x07
#define IS31FL3736_ABM_2_3 0x08
#define IS31FL3736_ABM_2_4 0x09
#define IS31FL3736_ABM_3_1 0x0A
#define IS31FL3736_ABM_3_2 0x0B
#define IS31FL3736_ABM_3_3 0x0C
#define IS31FL3736_ABM_3_4 0x0D
#define IS31FL3736_TIME_UPDATE 0x0E

#define ABM_LB_BIT 0x04 // Loop times - Endless

#define ABM_LB_T1 0x00 // Loop begin from T1
#define ABM_LB_T2 0x01 // Loop begin from T2
#define ABM_LB_T3 0x02 // Loop begin from T3
#define ABM_LB_T4 0x03 // Loop begin from T4

#define ABM_LE_BIT 0x06 // Loop times - Endless

#define ABM_LE_T3 0x00 // Loop end from T3 // i.e. off state
#define ABM_LE_T1 0x01 // Loop end from T1 // i.e. on state

#define ABM_LTA_BIT 0x00 // Loop times - Endless

#define ABM_LTA_ENDLESS 0x00 // Loop times - Endless
#define ABM_LTA_1 0x01 // Loop times - 1
#define ABM_LTA_2 0x02 // Loop times - 2
#define ABM_LTA_3 0x03 // Loop times - 3
#define ABM_LTA_4 0x04 // Loop times - 4
#define ABM_LTA_5 0x05 // Loop times - 5
#define ABM_LTA_6 0x06 // Loop times - 6
#define ABM_LTA_7 0x07 // Loop times - 7
#define ABM_LTA_8 0x08 // Loop times - 8
#define ABM_LTA_9 0x09 // Loop times - 9
#define ABM_LTA_10 0x0A // Loop times - 10
#define ABM_LTA_11 0x0B // Loop times - 11
#define ABM_LTA_12 0x0C // Loop times - 12
#define ABM_LTA_13 0x0F // Loop times - 13
#define ABM_LTA_14 0x0E // Loop times - 14
#define ABM_LTA_15 0x0F // Loop times - 15

#define ABM_LTB_BIT 0x00 // Loop times - Endless


#define T1_BITS 0x05
// Register 1 of ABM-x
#define T1_0021 0x00 // 0.21s
#define T1_0042 0x01 // 0.42s
#define T1_0084 0x02 // 0.84s
#define T1_0168 0x03 // 1.68s
#define T1_0336 0x04 // 3.36s
#define T1_0672 0x05 // 6.72s
#define T1_1344 0x06 // 13.44s
#define T1_2688 0x07 // 26.88s

#define T2_BITS 0x01
#define T2_00000 0x00 //0s
#define T2_00021 0x01 // 0.21s
#define T2_00042 0x02 // 0.42s
#define T2_00084 0x03 // 0.84s
#define T2_00168 0x04 // 1.68s
#define T2_00336 0x05 // 3.36s
#define T2_00672 0x06 // 6.72s
#define T2_01344 0x07 // 13.44s
#define T2_02688 0x08 // 26.88s

// Register 2 of ABM-x
#define T3_BITS 0x05
#define T3_0021 0x00 // 0.21s
#define T3_0042 0x01 // 0.42s
#define T3_0084 0x02 // 0.84s
#define T3_0168 0x03 // 1.68s
#define T3_0336 0x04 // 3.36s
#define T3_0672 0x05 // 6.72s
#define T3_1344 0x06 // 13.44s
#define T3_2688 0x07 // 26.88s

#define T4_BITS 0x01
#define T4_00000 0x00 //0s
#define T4_00021 0x01 // 0.21s
#define T4_00042 0x02 // 0.42s
#define T4_00084 0x03 // 0.84s
#define T4_00168 0x04 // 1.68s
#define T4_00336 0x05 // 3.36s
#define T4_00672 0x06 // 6.72s
#define T4_01344 0x07 // 13.44s
#define T4_02688 0x08 // 26.88s
#define T4_05376 0x09 // 53.76s
#define T4_10752 0x0A // 107.52s



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


void IS31FL3736_pwm_init(void)//white LED
{
  uint8_t i;
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//Unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x03);//Turn to page 3: function registers
  IS_IIC_WriteByte(DISPLAY_ADDR,0x00,0x00);//Enable software shutdown
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//Unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x00);// Turn to page 0: control registers
  for(i=0;i<0x18;i++) {
    IS_IIC_WriteByte(DISPLAY_ADDR,i,0xFF);// Enable all LEDs
  }
  //can use buffer write type as figure 7 in datasheet
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x01);//Turn to page 1: PWM registers
  for(i=0;i<192;i++) {
    IS_IIC_WriteByte(DISPLAY_ADDR,i,0x00);//Set PWM data to 0
  }
  //can use buffer write type as figure 7 in datasheet
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x03);//Turn to page 3: function registers
  IS_IIC_WriteByte(DISPLAY_ADDR,0x00,0x01);//Release software shutdown to normal operation
  IS_IIC_WriteByte(DISPLAY_ADDR,0x01,0x1f);//global current
}

void by_rows() {
  for ( uint8_t row = 0; row < ROWS; row++) {
    for( uint8_t column = 0; column < COLUMNS; column++){
      raddr = row*3;
      caddr = column *2;
      switch(row){
        case 0:
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        break;
        case 1:
        abm_reg[raddr + RED_LED][caddr] = 0x02;
        abm_reg[raddr + GREEN_LED][caddr] = 0x03;
        abm_reg[raddr + BLUE_LED][caddr] = 0x01;
        break;
        case 2:
        abm_reg[raddr + RED_LED][caddr] = 0x03;
        abm_reg[raddr + GREEN_LED][caddr] = 0x01;
        abm_reg[raddr + BLUE_LED][caddr] = 0x02;
        break;
        case 3:
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        break;
      }
    }
  }

}

void by_columns_(){
  for ( uint8_t row = 0; row < ROWS; row++) {
    for( uint8_t column = 0; column < COLUMNS; column++){
      raddr = row*3;
      caddr = column *2;

      // abm_reg[raddr + RED_LED][caddr] = 0x01;
      // abm_reg[raddr + GREEN_LED][caddr] = 0x02;
      // abm_reg[raddr + BLUE_LED][caddr] = 0x03;

      switch(column){
        case 0:
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        break;
        case 1:
        abm_reg[raddr + RED_LED][caddr] = 0x02;
        abm_reg[raddr + GREEN_LED][caddr] = 0x03;
        abm_reg[raddr + BLUE_LED][caddr] = 0x01;
        break;
        case 2:
        abm_reg[raddr + RED_LED][caddr] = 0x03;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x01;
        break;
        case 3:
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        break;
        case 4:
        abm_reg[raddr + RED_LED][caddr] = 0x02;
        abm_reg[raddr + GREEN_LED][caddr] = 0x03;
        abm_reg[raddr + BLUE_LED][caddr] = 0x01;
        break;
        case 5:
        abm_reg[raddr + RED_LED][caddr] = 0x03;
        abm_reg[raddr + GREEN_LED][caddr] = 0x01;
        abm_reg[raddr + BLUE_LED][caddr] = 0x02;
        break;
        case 6:
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        break;
        case 7:
        abm_reg[raddr + RED_LED][caddr] = 0x02;
        abm_reg[raddr + GREEN_LED][caddr] = 0x03;
        abm_reg[raddr + BLUE_LED][caddr] = 0x01;
        break;
      }
    }
  }

}

void by_columns(){
  for ( uint8_t row = 0; row < ROWS; row++) {
    for( uint8_t column = 0; column < COLUMNS; column++){
      raddr = row*3;
      caddr = column *2;

      // abm_reg[raddr + RED_LED][caddr] = 0x01;
      // abm_reg[raddr + GREEN_LED][caddr] = 0x02;
      // abm_reg[raddr + BLUE_LED][caddr] = 0x03;
        abm_reg[raddr + RED_LED][caddr] = 0x01;
        abm_reg[raddr + GREEN_LED][caddr] = 0x02;
        abm_reg[raddr + BLUE_LED][caddr] = 0x03;
    }
  }

}


void IS31FL3736_abm_init(void)//white LED
{
  uint8_t i;
  IS_IIC_WriteByte(DISPLAY_ADDR, 0xFE, 0xc5); //Unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR, 0xFD, 0x00); // Turn to page 0: LED Control
  for(i=0;i<0x18;i++) {
    IS_IIC_WriteByte(DISPLAY_ADDR, i, 0xFF); // Enable all LEDs
  }


  by_columns();

  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x02);//Turn to page 2: ABM registers
  uint8_t pwmAddr = 0;
  for ( uint8_t i=0; i<12; i++) {
    for( uint8_t j=0; j<16; j=j+2) {
      IS_IIC_WriteByte(DISPLAY_ADDR, pwmAddr, abm_reg[i][j]);//Set PWM data to 0
      pwmAddr = pwmAddr+2;
    }
  }

  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x03);//Turn to page 3: Config Registers
  IS_IIC_WriteByte(DISPLAY_ADDR,0x00,0x00); //Enable software shutdown
  uint8_t timing_1 = T1_0021<<T1_BITS|T2_00021<<T2_BITS;
  uint8_t timing_2 = T3_0021<<T3_BITS|T4_00021<<T4_BITS;
  uint8_t timing_3 = T1_0021<<T1_BITS|T2_00021<<T2_BITS;
  uint8_t timing_4 = T3_0021<<T3_BITS|T4_00021<<T4_BITS;
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_1_1, timing_1);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_1_2, timing_2);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_1_3, ABM_LE_T3<<ABM_LE_BIT|ABM_LB_T1<<ABM_LB_BIT|ABM_LTA_ENDLESS<<ABM_LTA_BIT);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_1_4, 0b00000000);

  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_2_1, timing_3);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_2_2, timing_4);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_2_3, ABM_LE_T3<<ABM_LE_BIT|ABM_LB_T3<<ABM_LB_BIT|ABM_LTA_ENDLESS<<ABM_LTA_BIT);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_2_4, 0b00000000);

  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_3_1, timing_1);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_3_2, timing_2);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_3_3, ABM_LE_T3<<ABM_LE_BIT|ABM_LB_T4<<ABM_LB_BIT|ABM_LTA_ENDLESS<<ABM_LTA_BIT);
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_ABM_3_4, 0b00000000);

  //can use buffer write type as figure 7 in datasheet
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5); //Unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x03); //Turn to page 3: function registers
  IS_IIC_WriteByte(DISPLAY_ADDR,0x01,0x1f); //global current
  IS_IIC_WriteByte(DISPLAY_ADDR,0x00,0x03); //Release software shutdown to normal operation


  IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5); //unlock FDh
  IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x03); //Turn to page 3: function registers
  IS_IIC_WriteByte(DISPLAY_ADDR, IS31FL3736_TIME_UPDATE ,0x00);

}


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
   for ( uint8_t i=0; i<12; i++) {
     for( uint8_t j=0; j<16; j++) {
       pwm_reg[i][j] = 0x00;
     }
   }


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

      IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
      IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x01);//Turn to page 1: PWM registers
      uint8_t pwmAddr = 0;
      for ( uint8_t i=0; i<12; i++) {
        for( uint8_t j=0; j<16; j=j+2) {
          IS_IIC_WriteByte(DISPLAY_ADDR, pwmAddr, pwm_reg[i][j]);//Set PWM data to 0
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
    IS_IIC_WriteByte(DISPLAY_ADDR,0xFE,0xc5);//unlock FDh
    IS_IIC_WriteByte(DISPLAY_ADDR,0xFD,0x01);//Turn to page 1: PWM registers
    uint8_t pwmAddr = 0;
    for ( uint8_t i=0; i<12; i++) {
      for( uint8_t j=0; j<16; j=j+2) {
        IS_IIC_WriteByte(DISPLAY_ADDR, pwmAddr, pwm_reg[i][j]);//Set PWM data to 0
        pwmAddr = pwmAddr+2;
      }
    }
    // delay(100);
    nextPhase(phase, pwm);
  }
}

void IS31FL3736_abm_01() {
  delay(1000);
}


void setup()
{
  Wire.begin();
  Wire.setClock(800000); //I2C 800kHz
  Serial.begin(9600);

  pinMode( heartbeat, OUTPUT);

  IS31FL3736_abm_init();

}



void loop()
{
  Serial.println("Starting loop");
  IS31FL3736_abm_01();
}
