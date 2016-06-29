#!/usr/local/bin/python3.5

import redis
import random
r = redis.StrictRedis(host='localhost', port=6379, db=0)

#Variables obtained from FSK Recieve
sendDeviceID = 2345768
batteryVoltage = random.randrange(3000)
temperature = 20
packetNumber = 82
carStatus = True

r.publish("radio_pkt", "%d %d %d %d %s" % (sendDeviceID, batteryVoltage, temperature, packetNumber, carStatus))
