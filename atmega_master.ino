//SPI MASTER (ARDUINO)

#include <SPI.h>      

#define ROW_1_LOW       13
#define ROW_1_HIGH      14
#define ROW_2_LOW       4
#define ROW_2_HIGH      6
#define ROW_3_LOW       2
#define ROW_3_HIGH      3
#define ROW_4_LOW       27
#define ROW_4_HIGH      28
#define ROW_5_LOW       25
#define ROW_5_HIGH      26

#define ARD_CTRL_MOSI   15
#define ARD_CTRL_MISO   24

#define BRIGHTNESS_POT  23

#define ALOG_12         12  // OC0A
#define ALOG_3          11  // OC0B
#define ALOG_45         5   // OC2B

#define SPI_1_CLK       19
#define SPI_1_MISO      18
#define SPI_1_MOSI      17
#define SPI_1_CS        16

#define ACK             0xFF

#define WAIT_TIME_10MS  10

volatile char received_pixel;
int pixel_brightness;
int max_dimming, scaled_max;

unsigned char spi_slave_transceive(unsigned char data) {
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
        digitalWrite(ROW_1_HIGH, HIGH);
    } else {
        digitalWrite(ROW_1_LOW, HIGH);
    }

    if (recv & (1 << 1)) {
        digitalWrite(ROW_2_HIGH, HIGH);
    } else {
        digitalWrite(ROW_2_LOW, HIGH);
    }

    if (recv & (1 << 2)) {
        digitalWrite(ROW_3_HIGH, HIGH);
    } else {
        digitalWrite(ROW_3_LOW, HIGH);
    }

    if (recv & (1 << 3)) {
        digitalWrite(ROW_4_HIGH, HIGH);
    } else {
        digitalWrite(ROW_4_LOW, HIGH);
    }

    if (recv & (1 << 4)) {
        digitalWrite(ROW_5_HIGH, HIGH);
    } else {
        digitalWrite(ROW_5_LOW, HIGH);
    }
}

void write_all_float() {
    digitalWrite(ROW_1_HIGH, LOW);
    digitalWrite(ROW_1_LOW, LOW);
    digitalWrite(ROW_2_HIGH, LOW);
    digitalWrite(ROW_2_LOW, LOW);
    digitalWrite(ROW_3_HIGH, LOW);
    digitalWrite(ROW_3_LOW, LOW);
    digitalWrite(ROW_4_HIGH, LOW);
    digitalWrite(ROW_4_LOW, LOW);
    digitalWrite(ROW_5_HIGH, LOW);
    digitalWrite(ROW_5_LOW, LOW);
}

void setup(void) {
    //Starts Serial Communication at Baud Rate 115200
    //Serial.begin(115200);                    

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

    // Enable SPI
    SPCR = (1 << SPE);

    // Make sure ARD_CTRL_MOSI is low to not trigger column control early
    digitalWrite(ARD_CTRL_MOSI, LOW);   
    write_all_float(); 
}

// V1: Go pixel-by-pixel
void loop(void) {
    // Get received byte from Jetson Nano e.g. 0000001 for row 1 dimming
    received_pixel = spi_slave_transceive(ACK);
    
    // Get pixel brightness -- 0 is off, 255 is max dimming
    //pixel_brightness = spi_slave_transceive(ACK);
    pixel_brightness = (received_pixel >> 5) * 255 / 7; // Value from 0 to 7

    // Get maximum dimming from potentiometer
    max_dimming = analogRead(BRIGHTNESS_POT);
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