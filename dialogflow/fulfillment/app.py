# -*- coding:utf8 -*-
# !/usr/bin/env python

from __future__ import print_function
from flask_basicauth import BasicAuth
from flask import make_response
from flask import request
from flask import Flask
import datetime
import paho.mqtt.client as mqtt
import os
import json
from urllib.error import HTTPError
from urllib.parse import urlparse, urlencode
from future.standard_library import install_aliases
install_aliases()

#from urllib.request import urlopen, Request


# Flask app
app = Flask(__name__)

# MQTT client
mqttc = mqtt.Client()

# Current socket state
sockets = {}

# add basic auth
app.config['BASIC_AUTH_USERNAME'] = os.environ.get('BASIC_AUTH_USERNAME')
app.config['BASIC_AUTH_PASSWORD'] = os.environ.get('BASIC_AUTH_PASSWORD')
basic_auth = BasicAuth(app)


# Parse CLOUDMQTT_URL (or fallback to localhost)
url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://localhost:1883')
url = urlparse(url_str)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):

    # handle the update of MySockets
    if (msg.topic.startswith("MySockets/")):
        switchName = msg.topic[10:]

        # delete if empty
        if (msg.payload == ""):
            del sockets[switchName]
        # add if message is long enough to be a json (ON, OFF also possible)
        elif (len(msg.payload) > 4):
            data = json.loads(msg.payload)
            sockets[data["name"]] = data


@app.route('/webhook', methods=['POST'])
@basic_auth.required
def webhook():
    req = request.get_json(silent=True, force=True)
    res = processRequest(req)
    res = json.dumps(res, indent=4)
    r = make_response(res)
    r.headers['Content-Type'] = 'application/json'
    return r


def mqtt_start():
    print("Try connect mqtt")
    mqttc.username_pw_set(url.username, url.password)
    mqttc.connect(url.hostname, url.port)
    mqttc.subscribe("MySockets/#")
    mqttc.on_message = on_message

    mqttc.publish("DFServer/Message",
                  "Start {}".format(datetime.datetime.now()))

    mqttc.loop_start()


def internalSwitchOn(name):
    print("Switch on: " + name)
    if(name in sockets.keys()):
        s = sockets[name]
        if (s["state"] == False):
            s["state"] = True
            data = json.dumps(s)
            mqttc.publish("MySockets/"+s["name"], data, 0,  True)


def internalSwitchOff(name):
    print("Switch off: " + name)
    if(name in sockets.keys()):
        s = sockets[name]
        if (s["state"] == True):
            s["state"] = False
            data = json.dumps(s)
            mqttc.publish("MySockets/"+s["name"], data, 0,  True)


def simple_response(text, expectedUserResponse=True):
    """
    Send back a response using dialog flow 2.0 
    """
    return {"payload": {"google": {"expectUserResponse": expectedUserResponse, "richResponse": {"items": [{"simpleResponse": {"textToSpeech": text}}]}}}}


def processRequest(req):
    """
    Handle all intents from diaglog flow and trigger the mqtt switching
    """
    intent_name = req["queryResult"]["intent"]["displayName"]

    print("Intent: {}".format(intent_name))
    if ("Status" == intent_name):
        socket_name = req["queryResult"]["parameters"]["Socket"]

        if (socket_name in sockets.keys()):
            if sockets[socket_name]["state"]:
                return simple_response("{} is grad an.".format(socket_name))
            else:
                return simple_response("{} is grad aus.".format(socket_name))
        else:
            return simple_response("{} kenn ich ned.".format(socket_name))

    if ("SwitchOff" == intent_name):
        socket_name = req["queryResult"]["parameters"]["Socket"]
        if (socket_name in sockets.keys()):
            if sockets[socket_name]["state"]:
                internalSwitchOff(socket_name)
                return simple_response("Ok, ich schalt {} aus.".format(socket_name))
            else:
                return simple_response("{} is scho aus.".format(socket_name))
        else:
            return simple_response("{} kenn ich ned.".format(socket_name))

    if ("SwitchOn" == intent_name):
        socket_name = req["queryResult"]["parameters"]["Socket"]
        if (socket_name in sockets.keys()):
            if sockets[socket_name]["state"]:
                return simple_response("{} is scho an.".format(socket_name))
            else:
                internalSwitchOn(socket_name)
                return simple_response("Ok, ich schalt {} an.".format(socket_name))
        else:
            return simple_response("{} kenn ich ned.".format(socket_name))

    if ("AllOff" == intent_name):
        for socket_name in sockets.keys():
            internalSwitchOff(socket_name)
        return simple_response("Ok, ich schalt alles aus.")

    if ("AllOn" == intent_name):
        for socket_name in sockets.keys():
            internalSwitchOn(socket_name)
        return simple_response("Ok, ich schalt alles ein.")

    return simple_response("Des versteh ich ned.")


if __name__ == '__main__':
    #start mqtt connection
    mqtt_start()

    port = int(os.getenv('PORT', 5000))
    print("Starting app on port %d" % port)

    #run the fulfillment flask app
    app.run(debug=False, port=port, host='0.0.0.0')
