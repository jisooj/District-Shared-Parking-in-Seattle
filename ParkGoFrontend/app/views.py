from app import app, db

from flask import render_template, flash, url_for, redirect, g
from datetime import datetime
from sqlalchemy import desc, func
from sqlalchemy.orm import aliased

import redis #for IPC with radio parser

from .forms import DataForm, ParkingSpaceForm
from .models import ParkPacket, Sensor

def redis_thread():
    app.logger.info("Redis Init")
    r = redis.StrictRedis(host="localhost", port=6379, db=0)
    r = r.pubsub()
    r.subscribe("radio_pkt")
    while True:
        for m in r.listen():
            if m['type'] == "message":
                sendDeviceID, batteryVoltage, \
                temperature, packetNumber, \
                carStatus = m['data'].decode('utf-8').split(" ", 4)

                newData(sendDeviceID, batteryVoltage, temperature, \
                packetNumber, carStatus)

#Called when there is updated information about the state of the parking lot
def newData(sendDeviceID, batteryVoltage, temperature, packetNumber, carStatus):
    print("New DATA from" + sendDeviceID)
    dataIn = ParkPacket(
        sensorId=sendDeviceID,
        occupied=(carStatus == 'True'),
        temperature=temperature,
        battVolt=batteryVoltage,
        timestamp=datetime.utcnow()
        )
    db.session.add(dataIn)
    db.session.commit()

@app.route('/')
def home_page():

    ''' What the following SQLALCHEMY query should do
    SELECT pp.*
    FROM
    	park_packet as pp,
    	(SELECT sensorID, MAX(timestamp) as maxts
    	FROM park_packet
    	GROUP BY sensorID) as maxpp
    WHERE pp.sensorID = maxpp.sensorID
    AND pp.timestamp = maxpp.maxts;
    '''

    subq = db.session.query(ParkPacket.sensorId.label('sid'),     \
            func.max(ParkPacket.timestamp).label('mts')).         \
            group_by(ParkPacket.sensorId).subquery()
    sensors = db.session.query(ParkPacket).                       \
              filter(ParkPacket.sensorId == subq.c.sid,           \
              ParkPacket.timestamp == subq.c.mts)

    lotl = db.session.query(Sensor)
    
    print(lotl)
    
    return render_template('home.html', parkingSpots=sensors, lotLayout=lotl)

@app.route('/sensorin', methods=["GET", "POST"])
def sensorin():
    form = DataForm()

    if form.validate_on_submit():
        dataIn = ParkPacket(
            sensorId=form.sensorId.data,
            occupied=(form.occupied.data == 'True'),
            temperature=form.temperature.data,
            battVolt=form.battVolt.data,
            timestamp=datetime.utcnow()
            )
        db.session.add(dataIn)
        db.session.commit()

    return render_template('sensorin.html', form=form)

@app.route('/setup', methods=["GET", "POST"])
def lotsetup():
    form = ParkingSpaceForm()

    if form.validate_on_submit():
        dataIn = Sensor(
            uid = form.uid.data,
            parkingspace = form.parkingspace.data,
            rotation = form.rotation.data,
            leftCoord = form.leftCoord.data,
            topCoord = form.topCoord.data
            )
        db.session.add(dataIn)
        db.session.commit()

    return render_template('lotsetup.html', form=form)

@app.errorhandler(404)
def page_not_found(error):
	app.logger.error('404d')
	return redirect(url_for("home_page"))
