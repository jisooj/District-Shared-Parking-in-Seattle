from flask.ext.wtf import Form
from wtforms import StringField, BooleanField
from wtforms.validators import DataRequired

class DataForm(Form):
    sensorId = StringField('sensorId', validators=[DataRequired()])
    occupied = StringField('occupied', validators=[DataRequired()])
    temperature = StringField('temperature', validators=[DataRequired()])
    battVolt = StringField('battVolt', validators=[DataRequired()])

class ParkingSpaceForm(Form):
    uid = StringField('uid', validators=[DataRequired()])
    parkingspace = StringField('parkingspace', validators=[DataRequired()])
    topCoord = StringField('topCoord', validators=[DataRequired()])
    leftCoord = StringField('leftCoord', validators=[DataRequired()])
    rotation = StringField('rotation', validators=[DataRequired()])
