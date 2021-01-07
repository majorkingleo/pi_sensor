I've a Groove Pi GPS module 
git clone https://github.com/DexterInd/GrovePi.git

It's connected to the searial interface acoording to the instructions
from GrovePi/Software/Python/grove_gps/README.md

Since my Raspi has no internet connection I wan't to sync the 
time via GPS. I'm not using a RTC module, because I simple have 
this unused GPS module.

based on dextergps.py I extract date and time from this line from
the GPS module
        
$GPRMC,144357.000,A,4706.0552,N,01525.8247,E,0.00,43.35,060121,,,A*56
       ^^^^^^                                           ^^^^^^
       HHMMDD                                           DDMMYY

and adjust the date and time on the raspberry
