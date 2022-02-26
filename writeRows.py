import spidev
from os.path import join
import time

SB = 180
EB = 181
NUM_ROWS = 5
ACK = [119]

spi = spidev.SpiDev()
spi.open(0, 0)
# spi.max_speed_hz = 250000
spi.max_speed_hz = 250000

def validateData(data, numRequiredElements):
    if(len(data) != numRequiredElements):
        return 0
    for i in data:
        if(i < 0 or i>255 or i == ACK[0] 
            or i == SB or i==EB):
            return 0
    return 1

def sendRowData(data):
    

    isDataValid = validateData(data, NUM_ROWS)
    if( not isDataValid):
        print("Data validation error!")
        return 0

    
    # Prepend start byte and append end byte
    data.insert(0, SB)
    data.insert(len(data), EB)

    for i in data:
        response = spi.xfer2([i])
        print(response)
        if(response != ACK):
            return 0
    return 1

try:
    data = [254, 255, 188, 173, 256.5, 5, 6, 6]
    wasSuccessful = sendRowData(data)

    if(wasSuccessful):
        print("Transaction success")
    else:
        print("Transaction failed")
except:
    print("ERROR")
    spi.close()



## file = open(join("Desktop", "heyyyy.txt"), 'w')
## MOSI --> 19
## MISO --> 21
## Clock --> 23
## Chip select --> 24
