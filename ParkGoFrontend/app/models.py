from sqlalchemy.ext.hybrid import hybrid_property

from app import db

class Sensor(db.Model):
    uid = db.Column(db.Integer, primary_key = True)
    parkingspace = db.Column(db.String(26)) #name of parking space
    rotation = db.Column(db.String(1)) #b l t r
    leftCoord = db.Column(db.Integer) #each "unit" moves space by 1/2
    topCoord = db.Column(db.Integer)

class ParkPacket(db.Model):
    packetId = db.Column(db.Integer, primary_key = True) #incrementing unique ID
    sensorId = db.Column(db.Integer, db.ForeignKey('sensor.uid'))
    timestamp = db.Column(db.DateTime)
    occupied = db.Column(db.Boolean)
    temperature = db.Column(db.Integer)
    battVolt = db.Column(db.Integer)
