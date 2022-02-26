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

#define ACK             0xFF

#define WAIT_TIME_10MS  10

volatile char received_pixel;
int pixel_brightness;
int max_dimming, scaled_max;

unsigned char spi_transceive(unsigned char data) {
    SPDR = data;
  
    while (!(SPSR & 1 << SPIF));

    return SPDR;
}

// TODO: Condense with write_rows()
void write_analogs(char row_locn, char pix_brightness) {
    if ((row_locn & 1) || row_locn & (1 << 1)) {
        analogWrite(ALOG_12, pix_brightness);
    } else if (row_locn & (1 << 2)) {
        analogWrite(ALOG_3, pix_brightness);
    } else if ((row_locn & (1 << 3)) || (row_locn & (1 << 4))) {
        analogWrite(ALOG_45, pix_brightness);
    }
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

    // Enable SPI
    SPCR = (1 << SPE);

    // Set SPI comms with Jetson Nano
    pinMode(SPI_1_CLK, INPUT);  
    pinMode(SPI_1_MISO, OUTPUT);  
    pinMode(SPI_1_MOSI, INPUT);
    pinMode(SPI_1_CS, INPUT);

    // Make sure ARD_CTRL_MOSI is low to not trigger column control early
    digitalWrite(ARD_CTRL_MOSI, LOW);   
    write_all_float(); 
}

// V2: Go row-by-row
void loop(void) {
    // Get received byte from Jetson Nano e.g. 0000001 for row 1 dimming
    received_pixel = spi_transceive(ACK);
    
    // Get pixel brightness -- 0 is off, 255 is max dimming
    // Make it one of 8 discrete brightness vals
    pixel_brightness = (received_pixel >> 5) * 255 / 7;

    // Get maximum dimming from potentiometer
    max_dimming = analogRead(BRIGHTNESS_POT);

    // Cap brightness value by potentiometer
    scaled_max = (max_dimming * 255) / 1024;
    if (pixel_brightness > scaled_max) {
        pixel_brightness = scaled_max;
    }

    // Set analog values using ATMEGA PWM outputs
    write_analogs(received_pixel, pixel_brightness);

    // Write to rows
    write_rows(received_pixel);

    // Send signal to slave using ARD_CTRL_MOSI
    digitalWrite(ARD_CTRL_MOSI, HIGH);

    // While ARD_CTRL_MISO is LOW, wait
    while (!digitalRead(ARD_CTRL_MISO));
    
    // Once surpassed, wait a defined period of time
    delay(WAIT_TIME_10MS);

    // Set rows to float
    write_all_float();

    // Send signal to slave using ARD_CTRL_MOSI
    digitalWrite(ARD_CTRL_MOSI, LOW);

    // While ARD_CTRL_MISO is LOW, wait
    while (!digitalRead(ARD_CTRL_MISO));
}