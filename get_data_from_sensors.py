# READS data from 2 DHT22 sensors (one indor and one outdoor) and writes them into
# a csv file


# vendor:
# https://joy-it.net/de/products/SEN-DHT22

# sudo pip3 install pigpio-dht
# for a systemd script for autostart see etc/system.d/

from pigpio_dht import DHT22
import datetime
import time;
import pigpio;
import traceback;

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

# The DHT22 sensor is not responding from time to time
# The only thing that helps is unpowering the sensor
# But this cannot be done by rebooting the raspi
# So I connected the 3.3V power source of the DHT22 sensor
# to a relais that is controlled by GPIO21 port.
# 0: power on
# 1: power off
#
# Note it would by possible doing this by a transistor, or schmitttrigger
# but you'l loose 0.7V and I'm not sure if the DTH22 would work
# and in anyway I may wan't to controll a fan or something alse with the 
# second relay block.
class Watchdog:
    gpio = 21
    pi = 0 

    def __init__( self, gpio ):
        self.gpio = gpio
        self.pi = pigpio.pi()
        self.pi.set_mode(self.gpio, pigpio.OUTPUT)
        self.pi.write(self.gpio, 0)

    def reset( self, wait_time = 5 ):
        self.pi.write(self.gpio, 1)
        time.sleep( wait_time )	
        self.pi.write(self.gpio, 0 )
 


#gpio = 22
#sensor = DHT22(gpio)
#result = sensor.sample( samples = 5 )

samples = 5
s_innen = MySensor( gpio =  4, samples = samples);
s_aussen = MySensor( gpio = 22, samples = samples );
out_csv = open( "temperatures.csv", "a" )

watchdog = Watchdog( 21 )

print( "start" )

while True:
    date = datetime.datetime.now()
    year_str = date.strftime( "%Y-%m-%d" )
    time_str = date.strftime( "%H:%M:%S" )
    #print( year_str, time_str )

    try:
       s_innen.collectData()
       s_aussen.collectData()

    except TimeoutError  as error:
       print( error )
       traceback.print_exc() 
       print( "trying to reset the sensor")
       watchdog.reset()
       time.sleep( 20 )	# wait until the sensor comes up again
       continue

    #s_innen.print()
    #s_aussen.print()

    csv_line = "%s;%s;%s;%s\n" % ( year_str, time_str, s_innen.getCSV(), s_aussen.getCSV() )
    print( csv_line )
    out_csv.write( csv_line )
    out_csv.flush()
    time.sleep(5*60)


#print( result )
