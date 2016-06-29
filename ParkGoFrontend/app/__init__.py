from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
import threading

app = Flask(__name__)
app.config.from_object('config')
db = SQLAlchemy(app)

from app import views, models

t = threading.Thread(target=views.redis_thread, args=())
t.setDaemon(True)
t.start()
