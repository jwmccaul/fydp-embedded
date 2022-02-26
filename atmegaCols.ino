#include <SPI.h>
#include <RowActuator.h>
#include <ColumnActuator.h>

#define COLUMN_1_LOW    0
#define COLUMN_2_LOW    1
#define COLUMN_3_LOW    2
#define COLUMN_4_LOW    3
#define COLUMN_5_LOW    4
#define COLUMN_6_LOW    5
#define COLUMN_7_LOW    A5
#define COLUMN_8_LOW    A4
#define COLUMN_9_LOW    A3
#define COLUMN_10_LOW   A2
#define COLUMN_11_LOW   A1

#define ARD_CTRL_MOSI 8
#define ARD_CTRL_MISO 9

#define HINGE_POT   A0
#define LED_CTRL    6
#define BUTTON_SIG  7

// #define SPI_2_CLK   19
// #define SPI_2_MISO  18
// #define SPI_2_MOSI  17
// #define SPI_2_CS    16

#define ACK1 0x77
#define START_BYTE 0xB4
#define END_BYTE 0xB5

#define WAIT_TIME_10MS  10

#define NUM_ROWS 5
#define NUM_COLS 11

#define NUM_COLS_TEST_GRID 2
#define NUM_ROWS_TEST_GRID 2

ColumnActuator colAct1(COLUMN_1_LOW);
ColumnActuator colAct2(COLUMN_2_LOW);
// ColumnActuator colAct3(COLUMN_3_LOW);
// ColumnActuator colAct4(COLUMN_4_LOW);
// ColumnActuator colAct5(COLUMN_5_LOW);
// ColumnActuator colAct6(COLUMN_6_LOW);
// ColumnActuator colAct7(COLUMN_7_LOW);
// ColumnActuator colAct8(COLUMN_8_LOW);
// ColumnActuator colAct9(COLUMN_9_LOW);
// ColumnActuator colAct10(COLUMN_10_LOW);
// ColumnActuator colAct11(COLUMN_11_LOW);

volatile byte startByte, startConfirm, endByte;

bool isTimerExceeded;

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

void setAllFloat() {
  colAct1.setFloat();
  colAct2.setFloat();
//   colAct3.setFloat();
//   colAct4.setFloat();
//   colAct5.setFloat();
//   colAct6.setFloat();
//   colAct7.setFloat();
//   colAct8.setFloat();
//   colAct9.setFloat();
//   colAct10.setFloat();
//   colAct11.setFloat();
}

void actuateColumn(ColumnActuator colAct) {
  // Set analog values using ATMEGA PWM outputs
  colAct.setLow();

  // Send signal to slave using ARD_CTRL_MOSI
  digitalWrite(ARD_CTRL_MOSI, HIGH);

  // While ARD_CTRL_MISO is LOW, wait
  while (!digitalRead(ARD_CTRL_MISO));

  // Once surpassed, wait a defined period of time
  delay(WAIT_TIME_10MS);

  // Set rows to float
  setAllFloat();

  // Send signal to slave using ARD_CTRL_MOSI
  digitalWrite(ARD_CTRL_MOSI, LOW);

  // While ARD_CTRL_MISO is LOW, wait
  while (!digitalRead(ARD_CTRL_MISO));
}

void resetGlobalVars() {
  startByte = 0;
  startConfirm = 0;
  endByte = 0;

  isTimerExceeded = false;
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
  pinMode(COLUMN_1_LOW, OUTPUT);  
  pinMode(COLUMN_2_LOW, OUTPUT);  
//   pinMode(COLUMN_3_LOW, OUTPUT);  
//   pinMode(COLUMN_4_LOW, OUTPUT);  
//   pinMode(COLUMN_5_LOW, OUTPUT);  
//   pinMode(COLUMN_6_LOW, OUTPUT);  
//   pinMode(COLUMN_7_LOW, OUTPUT);  
//   pinMode(COLUMN_8_LOW, OUTPUT);  
//   pinMode(COLUMN_9_LOW, OUTPUT);  
//   pinMode(COLUMN_10_LOW, OUTPUT);
//   pinMode(COLUMN_11_LOW, OUTPUT);

  // Set MOSI/MISO as input/output
  pinMode(ARD_CTRL_MOSI, OUTPUT);  
  pinMode(ARD_CTRL_MISO, INPUT);

  // Set peripheral inputs/outputs
  pinMode(HINGE_POT, INPUT);
  pinMode(LED_CTRL, OUTPUT);
  pinMode(BUTTON_SIG, INPUT);

  // Set MISO to low to avoid accidentally missing first column
  digitalWrite(ARD_CTRL_MOSI, LOW);
}

void loop () {  
  // Reset variables at start of loop
  resetGlobalVars();

  Serial.println("Waiting");
  startByte = spi_transceive_simple(ACK1);
  Serial.println(startByte, HEX);
  if(startByte == START_BYTE) {
    Serial.println("Received start byte, receiving row values");

    startConfirm = spi_transceive_data(ACK1);
    
    endByte = spi_transceive_data(ACK1);
    //Serial.println(endByte);
    if (!isTimerExceeded) {
      if (endByte == END_BYTE) {
        Serial.println("Proper end byte received. Data bytes: ");
        Serial.println(startConfirm);

        ColumnActuator columnActuators[2] = {colAct1, colAct2/*, colAct3, colAct4, colAct5, colAct6, colAct7, colAct8, colAct9, colAct10, colAct11*/};

        Serial.println("Entering loop");
        // Loop through each column and actuate the rows accordingly
        for (int i = 0; i < NUM_COLS_TEST_GRID; i++) {
          Serial.print("In loop for column ");
          Serial.println(i+1);
          actuateColumn(columnActuators[i]);
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