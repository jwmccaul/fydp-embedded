import spidev

SB = 180
EB = 181
NUM_ROWS = 1
ACK = [119]

spi = spidev.SpiDev()
spi.open(0, 0)
# spi.max_speed_hz = 250000
spi.max_speed_hz = 250000

class writeRows:
    def __init__(self):
        self.SB = SB
        self.EB = EB
        self.NUM_ROWS = NUM_ROWS
        self.ACK = [119]

        self.spi = spidev.SpiDev()
        self.spi.open(0, 0)
        # spi.max_speed_hz = 250000
        self.spi.max_speed_hz = 250000

    def validateData(self, data, numRequiredElements):
        print(numRequiredElements)
        if(len(data) != numRequiredElements):
            print(data)
            print("WRONG LENGTH")
            return 0
        for i in data:
            if(i < 0 or i>255 or i == ACK[0] 
                or i == SB or i==EB):
                print("BAD VALUE")
                return 0
        return 1

    def sendRowData(self,data):
        isDataValid = self.validateData(data, self.NUM_ROWS)
        if( not isDataValid):
            print("Data validation error!")
            return 0

        
        # Prepend start byte and append end byte
        data.insert(0, SB)
        data.insert(len(data), EB)

        for i in data:
            response = self.spi.xfer2([i])
            if(response != self.ACK):
                print("ACK MISSING")
            else:
                print("ACK RECEIVED: " + str(response[0]))
        return 1

testing = writeRows()
testing.sendRowData([0b000])

