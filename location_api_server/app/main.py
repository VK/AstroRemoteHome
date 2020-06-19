"""
Routes and views for the flask application.
"""
from flask import Flask
from flask import request
from flask_restplus import Api, Resource
from os import environ

# load ip geolocation
import geoip2.database
geo = geoip2.database.Reader('./GeoLite2-City.mmdb')

# load timezone finder
from timezonefinder import TimezoneFinder
tf = TimezoneFinder()
from pytz import timezone
import datetime



app = Flask(__name__)

api = Api(app, version='1.0', title='AstroRemoteHome Time Api',
          description='Reimplement some api calls for geolocation and timezones.',)

check_location = api.namespace( 'check', description='get the approximate geolocation by ip')
@check_location.route("/")
@check_location.response(404, 'no process possible')
class CheckLocation(Resource):
    '''Make a geolocation check'''
    @check_location.doc('get_check')
    def get(self):
        '''Make a geolocation check'''

        try:
            response = geo.city(request.remote_addr)

            return {
                "latitude": response.location.latitude,
                "longitude": response.location.longitude}
        except:
            return {"latitude": 49.4, "longitude": 11.1}



time_zone = api.namespace( 'get-time-zone', description='get the time zone based on the geolocation')
@time_zone.route("/")
@time_zone.response(404, 'no process possible')
@time_zone.param("lat", "latitude of the lookup")
@time_zone.param("lng", "longitude of the lookup")
class GetTimeZone(Resource):
    '''Make a timezone lookup'''
    @time_zone.doc('get_time_zone')
    def get(self):
        '''Make a timezone lookup'''

        try:
            loc = tf.timezone_at(lng=float(request.args["lng"]), lat=float(request.args["lat"]))
            locTz = timezone(loc)
            now = datetime.datetime.utcnow()

            return {"gmtOffset": locTz.utcoffset(now).seconds, "dst": int(not locTz.dst(now).seconds == 0) }
        except:
            return {}


if __name__ == '__main__':
    HOST = environ.get('SERVER_HOST', 'localhost')
    try:
        PORT = int(environ.get('SERVER_PORT', '80'))
    except ValueError:
        PORT = 80
    app.run(HOST, PORT, debug=True)
