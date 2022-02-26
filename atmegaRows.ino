#include <SPI.h>
#include <RowActuator.h>
#include <ColumnActuator.h>

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

#define ALOG_12         6   // OC0A
#define ALOG_3          5   // OC0B
#define ALOG_45         3   // OC2B

// #define SPI_1_CLK       13  //19
// #define SPI_1_MISO      12  //18
// #define SPI_1_MOSI      11  //17
// #define SPI_1_CS        10  //16

#define ACK1 0x77
#define ACK2 0x78
#define START_BYTE 0xB4
#define END_BYTE 0xB5

#define WAIT_TIME_10MS 10

#define NUM_ROWS 5
#define NUM_COLS 11

#define NUM_ROWS_TEST_GRID 2
#define NUM_COLS_TEST_GRID 2

RowActuator rowAct1(ROW_1_HIGH, ROW_1_LOW, ALOG_12);
RowActuator rowAct2(ROW_2_HIGH, ROW_2_LOW, ALOG_12);
// RowActuator rowAct3(ROW_3_HIGH, ROW_3_LOW, ALOG_3);
// RowActuator rowAct4(ROW_4_HIGH, ROW_4_LOW, ALOG_45);
// RowActuator rowAct5(ROW_5_HIGH, ROW_5_LOW, ALOG_45);

volatile byte startByte, endByte;
volatile byte rowsForCol1;
volatile byte rowsForCol2;
// volatile byte rowsForCol3;
// volatile byte rowsForCol4;
// volatile byte rowsForCol5;
// volatile byte rowsForCol6;
// volatile byte rowsForCol7;
// volatile byte rowsForCol8;
// volatile byte rowsForCol9;
// volatile byte rowsForCol10;
// volatile byte rowsForCol11;

bool isTimerExceeded;

volatile byte pixelBrightness;
//int max_dimming, scaled_max;

unsigned char spi_transceive_simple(unsigned char data) {
    SPDR = data;
    
    while (!(SPSR & 1 << SPIF));

    return SPDR;
}

unsigned char spi_transceive_data(unsigned char data) {
  uint32_t startTime = millis();
  SPDR = data;

  while (!(SPSR & 1 << SPIF) && (millis() - startTime) < 1000);

  if ((millis() - startTime) > 1000) {
    isTimerExceeded = true;
  } 

  byte receivedVal = SPDR;
  SPDR = 0;
  return receivedVal;
}

void writeRows(byte receivedRows) {
  if (receivedRows & 1) {
    rowAct1.setHigh(pixelBrightness);
  } else {
    rowAct1.setLow();
  }

  if (receivedRows & (1 << 1)) {
    rowAct2.setHigh(pixelBrightness);
  } else {
    rowAct2.setLow();
  }

  // if (receivedRows & (1 << 2)) {
  //   rowAct3.setHigh(pixelBrightness);
  // } else {
  //   rowAct3.setLow();
  // }

  // if (receivedRows & (1 << 3)) {
  //   rowAct4.setHigh(pixelBrightness);
  // } else {
  //   rowAct4.setLow();
  // }
  
  // if (receivedRows & (1 << 4)) {
  //   rowAct5.setHigh(pixelBrightness);
  // } else {
  //   rowAct5.setLow();
  // }
}

void setAllFloat() {
  rowAct1.setFloat();
  rowAct2.setFloat();
  // rowAct3.setFloat();
  // rowAct4.setFloat();
  // rowAct5.setFloat();
}

// TODO: WATCH FOR TIME?
void actuateRow(byte rowsForCurrentColumn) {
  // While ARD_CTRL_MOSI is LOW, wait
  while (!digitalRead(ARD_CTRL_MOSI));
  
  // Write to rows
  writeRows(rowsForCurrentColumn);

  // Send signal to master using ARD_CTRL_MOSI
  digitalWrite(ARD_CTRL_MISO, HIGH);

  // While ARD_CTRL_MOSI is high, wait
  while (digitalRead(ARD_CTRL_MOSI));

  // Set cols to float
  setAllFloat();

  // Write MISO low to end transaction
  digitalWrite(ARD_CTRL_MISO, LOW);
}

void resetGlobalVars() {
  startByte = 0;
  endByte = 0;
  isTimerExceeded = false;
  rowsForCol1 = 0;
  rowsForCol2 = 0;
  // rowsForCol3 = 0;
  // rowsForCol4 = 0;
  // rowsForCol5 = 0;
  // rowsForCol6 = 0;
  // rowsForCol7 = 0;
  // rowsForCol8 = 0;
  // rowsForCol9 = 0;
  // rowsForCol10 = 0;
  // rowsForCol11 = 0;
}

void setup() {
  // Comment out Serial comms for ATmega use
  Serial.begin(115200);
  
  // ***SPI Setup***
  // Have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // Turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // Set row pins as outputs
  pinMode(ROW_1_HIGH, OUTPUT);  
  pinMode(ROW_1_LOW, OUTPUT);  
  pinMode(ROW_2_HIGH, OUTPUT);  
  pinMode(ROW_2_LOW, OUTPUT);  
  // pinMode(ROW_3_HIGH, OUTPUT);  
  // pinMode(ROW_3_LOW, OUTPUT);  
  // pinMode(ROW_4_HIGH, OUTPUT);  
  // pinMode(ROW_4_LOW, OUTPUT);  
  // pinMode(ROW_5_HIGH, OUTPUT);  
  // pinMode(ROW_5_LOW, OUTPUT);

  // Set MOSI/MISO as input/output
  pinMode(ARD_CTRL_MOSI, INPUT);  
  pinMode(ARD_CTRL_MISO, OUTPUT);

  // Set brightness potentiometer as input
  pinMode(BRIGHTNESS_POT, INPUT);

  // Set analog outputs as outputs
  pinMode(ALOG_12, OUTPUT);  
  pinMode(ALOG_3, OUTPUT);  
  pinMode(ALOG_45, OUTPUT);

  // Set MISO to low to avoid accidentally missing first column
  digitalWrite(ARD_CTRL_MISO, LOW);

  // Set initial max dimming to MAX
  pixelBrightness = 255;
}

void loop () {  
  // Reset variables at start of loop
  resetGlobalVars();

  /*
  // Get pixel brightness -- 0 is off, 255 is max dimming
  // Make it one of 8 discrete brightness vals
  pixelBrightness = (received_pixel >> 5) * 255 / 7;
  
  // Get maximum dimming from potentiometer
  max_dimming = analogRead(BRIGHTNESS_POT);
  
  // Cap brightness value by potentiometer
  scaled_max = (max_dimming * 255) / 1024;
  if (pixelBrightness > scaled_max) {
      pixelBrightness = scaled_max;
  }*/
  pixelBrightness = 128;

  Serial.println("Waiting");
  startByte = spi_transceive_simple(ACK1);
  Serial.println(startByte, HEX);
  if(startByte == START_BYTE) {
    Serial.println("Received start byte, receiving row values");

    rowsForCol1 = spi_transceive_data(ACK1);
    rowsForCol2 = spi_transceive_data(ACK1);
    // rowsForCol3 = spi_transceive_data(ACK1);
    // rowsForCol4 = spi_transceive_data(ACK1);
    // rowsForCol5 = spi_transceive_data(ACK1);
    // rowsForCol6 = spi_transceive_data(ACK1);
    // rowsForCol7 = spi_transceive_data(ACK1);
    // rowsForCol8 = spi_transceive_data(ACK1);
    // rowsForCol9 = spi_transceive_data(ACK1);
    // rowsForCol10 = spi_transceive_data(ACK1);
    // rowsForCol11 = spi_transceive_data(ACK1);
    
    endByte = spi_transceive_data(ACK1);
    //Serial.println(endByte);
    if (!isTimerExceeded) {
      if (endByte == END_BYTE) {
        Serial.println("Proper end byte received. Data bytes: ");
        Serial.println(rowsForCol1);
        Serial.println(rowsForCol2);
        // Serial.println(rowsForCol3);
        // Serial.println(rowsForCol4);
        // Serial.println(rowsForCol5);
        // Serial.println(rowsForCol6);
        // Serial.println(rowsForCol7);
        // Serial.println(rowsForCol8);
        // Serial.println(rowsForCol9);
        // Serial.println(rowsForCol10);
        // Serial.println(rowsForCol11);

        int rowsForEachCol[2] = {rowsForCol1, rowsForCol2/*, rowsForCol3, rowsForCol4, rowsForCol5,
                                  rowsForCol6, rowsForCol7, rowsForCol8, rowsForCol9, rowsForCol10, rowsForCol11*/};

        // Loop through each column and actuate the rows accordingly
        for (int i = 0; i < NUM_COLS_TEST_GRID; i++) {
          actuateRow(rowsForEachCol[i]);
        }

      } else {
        Serial.println("Proper end byte not received");
      }
    } else {
      Serial.println("Waited too long for transaction. Starting over.");  
    }
  } else {
    Serial.println("Start byte not received");
  }
}