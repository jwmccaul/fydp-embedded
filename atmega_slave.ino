//SPI MASTER (ARDUINO)

#include <SPI.h>

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

#define SPI_2_CLK   19
#define SPI_2_MISO  18
#define SPI_2_MOSI  17
#define SPI_2_CS    16

#define ACK             0xFF

#define WAIT_TIME_10MS  10

volatile char received_pixels_first, received_pixels_second;
//int max_dimming, scaled_max;
//volatile int potentiometer_val;

unsigned char spi_slave_transceive(unsigned char data) {
    SPDR = data;
    
    while (!(SPSR & 1 << SPIF));

    return SPDR;
}

void write_cols(volatile char recv_first, volatile char recv_second) {
    // First byte gives first 8 columns
    if (recv_first & 1) {
        digitalWrite(COLUMN_1_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_1_LOW, LOW);
    }
    
    if (recv_first & (1 << 1)) {
        digitalWrite(COLUMN_2_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_2_LOW, LOW);
    }
    
    if (recv_first & (1 << 2)) {
        digitalWrite(COLUMN_3_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_3_LOW, LOW);
    }
    
    if (recv_first & (1 << 3)) {
        digitalWrite(COLUMN_4_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_4_LOW, LOW);
    }
    
    if (recv_first & (1 << 4)) {
        digitalWrite(COLUMN_5_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_5_LOW, LOW);
    }
    
    if (recv_first & (1 << 5)) {
        digitalWrite(COLUMN_6_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_6_LOW, LOW);
    }
    
    if (recv_first & (1 << 6)) {
        digitalWrite(COLUMN_7_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_7_LOW, LOW);
    }
    
    if (recv_first & (1 << 7)) {
        digitalWrite(COLUMN_8_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_8_LOW, LOW);
    }
    

    // Second byte gives second 8 columns
    if (recv_second & 1) {
        digitalWrite(COLUMN_9_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_9_LOW, LOW);
    }
    
    if (recv_second & (1 << 1)) {
        digitalWrite(COLUMN_10_LOW, HIGH);
    } 
    else {
        digitalWrite(COLUMN_10_LOW, LOW);
    }
    
    if (recv_second & (1 << 2)) {
        digitalWrite(COLUMN_11_LOW, HIGH);
    }
    else {
        digitalWrite(COLUMN_11_LOW, LOW);
    }
}

void write_all_float() {
    digitalWrite(COLUMN_1_LOW, LOW);
    digitalWrite(COLUMN_2_LOW, LOW);
    digitalWrite(COLUMN_3_LOW, LOW);
    digitalWrite(COLUMN_4_LOW, LOW);
    digitalWrite(COLUMN_5_LOW, LOW);
    digitalWrite(COLUMN_6_LOW, LOW);
    digitalWrite(COLUMN_7_LOW, LOW);
    digitalWrite(COLUMN_8_LOW, LOW);
    digitalWrite(COLUMN_9_LOW, LOW);
    digitalWrite(COLUMN_10_LOW, LOW);
    digitalWrite(COLUMN_11_LOW, LOW);
}

void setup(void) {
    // Set column pins as outputs
    pinMode(COLUMN_1_LOW, OUTPUT);  
    pinMode(COLUMN_2_LOW, OUTPUT);  
    pinMode(COLUMN_3_LOW, OUTPUT);  
    pinMode(COLUMN_4_LOW, OUTPUT);  
    pinMode(COLUMN_5_LOW, OUTPUT);  
    pinMode(COLUMN_6_LOW, OUTPUT);  
    pinMode(COLUMN_7_LOW, OUTPUT);  
    pinMode(COLUMN_8_LOW, OUTPUT);  
    pinMode(COLUMN_9_LOW, OUTPUT);  
    pinMode(COLUMN_10_LOW, OUTPUT);
    pinMode(COLUMN_11_LOW, OUTPUT);

    // Set MOSI/MISO as input/output
    pinMode(ARD_CTRL_MOSI, INPUT);  
    pinMode(ARD_CTRL_MISO, OUTPUT);

    // Set peripheral inputs/outputs
    pinMode(HINGE_POT, INPUT);
    pinMode(LED_CTRL, OUTPUT);
    pinMode(BUTTON_SIG, INPUT);

    // Set SPI comms with Jetson Nano
    pinMode(SPI_2_CLK, INPUT);  
    pinMode(SPI_2_MISO, OUTPUT);  
    pinMode(SPI_2_MOSI, INPUT);
    pinMode(SPI_2_CS, INPUT);

    // Enable SPI
    SPCR = (1 << SPE);

    // Make sure ARD_CTRL_MOSI is low to not trigger column control early
    digitalWrite(ARD_CTRL_MISO, LOW);
    write_all_float();

    // Turn LED on
    digitalWrite(LED_CTRL, HIGH);
}

// V2: Go row-by-row
void loop(void) {
    // TODO:    - If hinge pot indicates closed position, or button is off, send signal back to Nano
    //          - Else, send and make sure LED is on
    //potentiometer_val = analogRead(HINGE_POT);

    // Get received byte from Jetson Nano e.g. 0000001 for row 1 dimming
    received_pixels_first = spi_slave_transceive(ACK);
    
    // Get pixel brightness -- 0 is off, 255 is max dimming
    received_pixels_second = spi_slave_transceive(ACK);

    // While ARD_CTRL_MOSI is LOW, wait
    while (!digitalRead(ARD_CTRL_MOSI));

    // Write to rows
    write_cols(received_pixels_first, received_pixels_second);

    // Send signal to master using ARD_CTRL_MOSI
    digitalWrite(ARD_CTRL_MISO, HIGH);

    // While ARD_CTRL_MOSI is high, wait
    while (digitalRead(ARD_CTRL_MOSI));

    // Set cols to float
    write_all_float();
    digitalWrite(ARD_CTRL_MOSI, LOW);
}