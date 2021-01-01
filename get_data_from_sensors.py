# READS data from 2 DHT22 sensors (one indor and one outdoor) and writes them into
# a csv file


# vendor:
# https://joy-it.net/de/products/SEN-DHT22

# sudo pip3 install pigpio-dht
# for a systemd script for autostart see etc/system.d/

from pigpio_dht import DHT22
import datetime
import time;

class MySensor:
    
    gpio = 4
    samples = 5
    sensor = 0
    result = 0
    
    def __init__( self, gpio, samples ):
        self.gpio = gpio
        self.samples = samples
        self.sensor = DHT22( gpio )
        
    def collectData( self ):
        self.result = self.sensor.sample( self.samples )
    
    def print( self ):
        print( self.result )

    def getCSV( self ):
        buf = "%.2f;%.2f" % (self.result["temp_c"], self.result["humidity"])
        return buf


#gpio = 22
#sensor = DHT22(gpio)
#result = sensor.sample( samples = 5 )

samples = 5
s_innen = MySensor( gpio =  4, samples = samples);
s_aussen = MySensor( gpio = 22, samples = samples );
out_csv = open( "temperatures.csv", "a" )

print( "start" )

while True:
    date = datetime.datetime.now()
    year_str = date.strftime( "%Y-%m-%d" )
    time_str = date.strftime( "%H:%M:%S" )
    #print( year_str, time_str )

    s_innen.collectData()
    s_aussen.collectData()

    #s_innen.print()
    #s_aussen.print()

    csv_line = "%s;%s;%s;%s\n" % ( year_str, time_str, s_innen.getCSV(), s_aussen.getCSV() )
    print( csv_line )
    out_csv.write( csv_line )
    out_csv.flush()
    time.sleep(5*60)


#print( result )
