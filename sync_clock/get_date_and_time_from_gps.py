# Released under the MIT license (http://choosealicense.com/licenses/mit/).
# For more information see https://github.com/DexterInd/GrovePi/blob/master/LICENSE

import grovepi
import serial, time, sys
import re

from datetime import datetime

import pytz
from tzlocal import get_localzone # $ pip install tzlocal

en_debug = True

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def debug(in_str):
    if en_debug:
        eprint(in_str)

# extract date and time from this line
# $GPRMC,144357.000,A,4706.0552,N,01525.8247,E,0.00,43.35,060121,,,A*56
#        ^^^^^^                                           ^^^^^^
#        HHMMDD                                           DDMMYY

# See http://aprs.gids.nl/nmea/#rmc
patterns=["$GPRMC",
    "/[0-9]{6}\.[0-9]{2}/", # timestamp hhmmss.ss
    "[AV]",		    # A Navigation receiver warning A = OK, V = warning
    "/[0-9]{4}.[0-9]{2,/}", # latitude of position
    "/[NS]",  # North or South
    "/[0-9]{4}.[0-9]{2}", # longitude of position
    "/[EW]",  # East or West
    "/[0-9]+.[0-9]+/", # Speed over ground, Knots
    "/[0-9]+.[0-9]+/", # Course Made Good, True
    "/[0-9]{6}/", # Date 
    "/[0-9]+.[0-9]+/", # Magnetic variation 20.3
    "/[EW]/", # East or West
#    "//", # mandatory checksum
    ]

class GROVEGPS():
    def __init__(self, port='/dev/ttyAMA0', baud=9600, timeout=0):
        self.ser = serial.Serial(port, baud, timeout=timeout)
        self.ser.flush()
        self.raw_line = ""
        self.gga = []
        self.validation =[] # contains compiled regex

        # compile regex once to use later
        for i in range(len(patterns)-1):
            self.validation.append(re.compile(patterns[i]))

        self.clean_data()
        # self.get_date()  # attempt to gete date from GPS.

    def clean_data(self):
        '''
        clean_data:
        ensures that all relevant GPS data is set to either empty string
        or -1.0, or -1, depending on appropriate type
        This occurs right after initialisation or
        after 50 attemps to reach GPS
        '''
        self.timestamp = ""
        self.lat = -1.0    # degrees minutes and decimals of minute
        self.NS = ""
        self.lon = -1.0
        self.EW = ""
        self.altitude = -1.0
        self.latitude = -1.0  #degrees and decimals
        self.longitude = -1.0
        self.fancylat = ""  #
        self.date = ""
        self.datetime = 0;

    # def get_date(self):
    #     '''
    #     attempt to get date from GPS data. So far no luck. GPS does
    #     not seem to send date sentence at all
    #     function is unfinished
    #     '''
    #     valid = False
    #     for i in range(50):
    #         time.sleep(0.5)
    #         self.raw_line = self.ser.readline().strip()
    #         if self.raw_line[:6] == "GPZDA":  # found date line!
    #             print (self.raw_line)


    def read(self):
        '''
        Attempts 50 times at most to get valid data from GPS
        Returns as soon as valid data is found
        If valid data is not found, then clean up data in GPS instance
        '''
        valid = False
        for _ in range(50):
            time.sleep(0.5)
            self.raw_line = self.ser.readline()
            try:
                self.line = self.raw_line.decode('utf-8')
                self.line = self.line.strip()
            except:
                self.line = ""
            debug(self.line)
            if self.validate(self.line):
                valid = True
                break

        if valid:
            return self.gga
        else:
            self.clean_data()
            return []

    def validate(self, in_line):
        '''
        Runs regex validation on a GPGAA sentence.
        Returns False if the sentence is mangled
        Return True if everything is all right and sets internal
        class members.
        '''
        if in_line == "":
            return False
        if in_line[:6] != "$GPRMC":
            return False

        self.gga = in_line.split(",")
        debug (self.gga)

        #Sometimes multiple GPS data packets come into the stream. Take the data only after the last '$GPRMC' is seen
        try:
            ind=self.gga.index('$GPRMC', 5, len(self.gga))
            self.gga=self.gga[ind:]
        except ValueError:
            pass

        if len(self.gga) != 13:
            debug ("Failed: wrong number of parameters ")
            debug (self.gga)
            return False

        for i in range(len(self.validation)-1):
            if len(self.gga[i]) == 0:
                debug ("Failed: empty string %d"%i)
                return False
            test = self.validation[i].match(self.gga[i])
            if test == False:
                debug ("Failed: wrong format on parameter %d"%i)
                return False
            else:
                debug("Passed %d"%i)

        try:
            self.timestamp = self.gga[1]
            self.lat = float(self.gga[3])
            self.NS = self.gga[4]
            self.lon = float(self.gga[5])
            self.EW = self.gga[6]
            self.date = self.gga[9]
#            self.quality = int(self.gga[6])

            self.latitude = self.lat // 100 + self.lat % 100 / 60
            if self.NS == "S":
                self.latitude = - self.latitude
            self.longitude = self.lon // 100 + self.lon % 100 / 60
            if self.EW == "W":
                self.longitude = -self.longitude

            date_day  = int(self.date[0:2]);
            date_mon  = int(self.date[2:4]); 
            date_year = int(self.date[4:6]); 

            time_hour = int(self.timestamp[0:2]);
            time_min = int(self.timestamp[2:4]);
            time_sec = int(self.timestamp[4:6]);
    
            time_str = "20%02d-%02d-%02d %02d:%02d:%02d" % ( date_year, date_mon, date_day, time_hour, time_min, time_sec )
            debug( "date: " + self.date + " day: " + self.date[0:2] + " timestr: " + time_str )

            self.datetime = datetime.strptime( time_str + " UTC+0000", '%Y-%m-%d %H:%M:%S %Z%z' )

        except ValueError:
            debug( "FAILED: invalid value: " + ValueError)

        return True


if __name__ =="__main__":
    gps = GROVEGPS()
    while True:
        time.sleep(1)
        in_data = gps.read()
        if in_data != []:
            # print (in_data) 
            debug( "local timezone: " + str(get_localzone()) )
            #localtime = gps.datetime.astimezone(get_localzone())
            localtime = gps.datetime
            localtime = localtime.astimezone( get_localzone())
            print( datetime.strftime(localtime, '%Y-%m-%d %H:%M:%S' ) )
            break
