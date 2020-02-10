from machine import Pin
from time import sleep
led = Pin(2, Pin.OUT)
for a in range(10):
	led.on()
	sleep(0.5)
	led.off()
	sleep(0.5)

