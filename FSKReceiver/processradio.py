#!/usr/local/bin/python3.5

import socket
import redis

# precondition: n should be 16bit integer
def unsigned2signed(n):
    if n >= 2**15:
        n -= (2**16)
    return n

TCP_IP = '127.0.0.1'
TCP_PORT = 9000

print("INFO: Server for GNURadio Launched")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

r = redis.StrictRedis(host='localhost', port=6379, db=0)

#preamble starts with a 0 - not included in array size:32 - 1 = 31 elements -> index 30
preamble =  (   1, 0, 1, 0, 1, 0, 1,
             0, 1, 0, 1, 0, 1, 0, 1,
             0, 1, 1, 0, 0, 1, 0, 1, 
             0, 1, 0, 1, 1, 0, 1, 0)
bytesInFrame = 8

while True:
    connection, ClientIP = s.accept() # Wait for a connection
    print(ClientIP)

    preamblepoint = 0
    bitbuf = 0
    bitstep = 0
    frame = bytearray()

    ID_map = {}
    prevPacketNumber = None
    prevID = None
    count = 0
    while True:
        data = connection.recv(1) #GNU Radio gives us a one or zero here
        if (preamblepoint == 31): #start processing the packet
            #print("processing...")

            if (data == b'\x01'):
                prevbit = 1
            else:
                prevbit = 0
            data = connection.recv(1) #get another number
            if (data == b'\x01'):
                if (prevbit == 0): #01 -> 1
                    dbit = 1
                else:              #11 -> clock error
                    print("clock desync error")
                    preamblepoint = 0
                    frame = bytearray() #clear frame
            else:
                if (prevbit == 1): #10 -> 0
                    dbit = 0
                else:              #00 -> clock error      
                    print("clock desync error")   
                    preamblepoint = 0 
                    frame = bytearray() #clear frame
            
            if (dbit == 1):
                bitbuf |= 0b10000000
            bitstep += 1
            if (bitstep % 8 == 0):
                frame.append(bitbuf)
                bitbuf = 0
                if (bitstep == 8 * bytesInFrame):
                    bitstep = 0
                    #print("***NEW PACKET***")
                    preamblepoint = 0
                    last = 0
                    
                    #-----------PACKET PROCESSING----------
                    if (len(frame) > 4):
                        sendDeviceID = frame[3] + (frame[2] << 8) + (frame[1] << 16) + (frame[0] << 24)
                        batteryVoltage = frame[7] + (frame[6] << 8)
                        temperature = frame[5]
                        packetNumber = frame[4] & 0x7F
                        carStatus = (frame[4] & 0x80) != 0

                        '''-------------PACKET DEBUG--------------------'''
                        # Error handling showing N / 4 confidence percentage
                        if (prevPacketNumber != None and prevPacketNumber != packetNumber):
                            print("***NEW PACKET***")
                            print("-->PrevPacket number %d" % prevPacketNumber)
                            print("-->ID %d: %d / 4 packets received" % (prevID, count))
                            count = 1
                        else:
                            count = count + 1
                        prevPacketNumber = packetNumber 
                        prevID = sendDeviceID

                        # Handle duplicate packet sent from si4010
                        if (str(sendDeviceID) in ID_map):
                            if (ID_map[str(sendDeviceID)] == packetNumber):
                                frame = bytearray()
                                continue
                            else:
                                ID_map[str(sendDeviceID)] = packetNumber
                        else:
                            ID_map[str(sendDeviceID)] = packetNumber
                        '''---------------------------------------------'''
                        
                        print("Sent From  : " + str(sendDeviceID))        
                        print("Voltage    : " + str(batteryVoltage))        
                        print("Temperature: " + str(temperature))        
                        print("Packet Num : " + str(packetNumber))        
                        print("Car        : " + str(carStatus)) 
                        print("FRAMEDEBUG        : " + str(bin(frame[4]))) 
                        r.publish("radio_pkt", "%d %d %d %d %s" % (sendDeviceID, batteryVoltage, temperature, packetNumber, carStatus))                    
                        #--------------------------------------        
                    else:
                        print("Malformed Packet");
                    frame = bytearray() #clear frame
            else:
                bitbuf = bitbuf >> 1
        else:
            if (data == b'\x01'):
                if(preamble[preamblepoint] == 1):
                    preamblepoint += 1
                else:
                    preamblepoint = 0
            elif (data == b'\x00'):
                if(preamble[preamblepoint] == 0):
                    preamblepoint += 1
                else:
                    preamblepoint = 0

