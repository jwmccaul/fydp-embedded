//SPI MASTER (ARDUINO)

#include <SPI.h>      

#define ROW_1_LOW       7
#define ROW_1_HIGH      8
#define ROW_2_LOW       2
#define ROW_2_HIGH      4
#define ROW_3_LOW       0
#define ROW_3_HIGH      1
#define ROW_4_LOW       A4
#define ROW_4_HIGH      A5
#define ROW_5_LOW       A2
#define ROW_5_HIGH      A3

#define ARD_CTRL_MOSI   9
#define ARD_CTRL_MISO   A1

#define BRIGHTNESS_POT  A0

#define ALOG_12         6  // OC0A
#define ALOG_3          5  // OC0B
#define ALOG_45         3   // OC2B

#define SPI_1_CLK       19
#define SPI_1_MISO      18
#define SPI_1_MOSI      17
#define SPI_1_CS        16

void write_all_float() {
    digitalWrite(ROW_1_HIGH, HIGH);
    digitalWrite(ROW_1_LOW, LOW);
    digitalWrite(ROW_2_HIGH, HIGH);
    digitalWrite(ROW_2_LOW, LOW);
    digitalWrite(ROW_3_HIGH, HIGH);
    digitalWrite(ROW_3_LOW, LOW);
    digitalWrite(ROW_4_HIGH, HIGH);
    digitalWrite(ROW_4_LOW, LOW);
    digitalWrite(ROW_5_HIGH, HIGH);
    digitalWrite(ROW_5_LOW, LOW);
}

void write_rows(char recv) {
    if (recv & 1) {
        digitalWrite(ROW_1_HIGH, LOW);
    } else {
        digitalWrite(ROW_1_LOW, HIGH);
    }

    if (recv & (1 << 1)) {
        digitalWrite(ROW_2_HIGH, LOW);
    } else {
        digitalWrite(ROW_2_LOW, HIGH);
    }

    if (recv & (1 << 2)) {
        digitalWrite(ROW_3_HIGH, LOW);
    } else {
        digitalWrite(ROW_3_LOW, HIGH);
    }

    if (recv & (1 << 3)) {
        digitalWrite(ROW_4_HIGH, LOW);
    } else {
        digitalWrite(ROW_4_LOW, HIGH);
    }

    if (recv & (1 << 4)) {
        digitalWrite(ROW_5_HIGH, LOW);
    } else {
        digitalWrite(ROW_5_LOW, HIGH);
    }
}

void setup(void) {
    // Set row pins as outputs
    pinMode(ROW_1_HIGH, OUTPUT);  
    pinMode(ROW_1_LOW, OUTPUT);  
    pinMode(ROW_2_HIGH, OUTPUT);  
    pinMode(ROW_2_LOW, OUTPUT);  
    pinMode(ROW_3_HIGH, OUTPUT);  
    pinMode(ROW_3_LOW, OUTPUT);  
    pinMode(ROW_4_HIGH, OUTPUT);  
    pinMode(ROW_4_LOW, OUTPUT);  
    pinMode(ROW_5_HIGH, OUTPUT);  
    pinMode(ROW_5_LOW, OUTPUT);

    // Set MOSI/MISO as input/output
    pinMode(ARD_CTRL_MOSI, OUTPUT);  
    pinMode(ARD_CTRL_MISO, INPUT);

    // Set brightness potentiometer as input
    pinMode(BRIGHTNESS_POT, INPUT);

    // Set analog outputs as outputs
    pinMode(ALOG_12, OUTPUT);  
    pinMode(ALOG_3, OUTPUT);  
    pinMode(ALOG_45, OUTPUT);

    // Set SPI comms with Jetson Nano
    pinMode(SPI_1_CLK, INPUT);  
    pinMode(SPI_1_MISO, OUTPUT);  
    pinMode(SPI_1_MOSI, INPUT);
    pinMode(SPI_1_CS, INPUT);

    write_all_float();
    
    // bleh
    analogWrite(ALOG_45, 128);
    analogWrite(ALOG_12, 128);
    analogWrite(ALOG_3, 128);
    //digitalWrite(ROW_4_HIGH, HIGH);
    digitalWrite(ARD_CTRL_MOSI, LOW);
}

// V2: Go row-by-row
void loop(void) {
    /*for (int i = 0; i < 5; i++) {
        write_rows((1 << i));

        delay(2000);

        write_all_float();
    }*/
/*
      // 01010101=85, 00000101=5
      write_rows(0b00010101);

      delay(312);

      //write_all_float();

      // 10101010=170, 00000010=2
      write_rows(0b00001010);

      delay(312);
*/

  digitalWrite(ARD_CTRL_MOSI, HIGH);

  while (!digitalRead(ARD_CTRL_MISO));

  delay(624);

  digitalWrite(ARD_CTRL_MOSI, LOW);

  while (digitalRead(ARD_CTRL_MISO));

  delay(624);
}