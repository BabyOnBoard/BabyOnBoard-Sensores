# -*- coding: utf-8 -*-
#! /usr/bin/python

import RPi.GPIO as gpio
import time
import requests

""" Global """
SWITCH1=4
SWITCH2=22
SWITCH3=27


""" Configurando GPIO """
# Configurando o modo dos pinos como BCM
gpio.setmode(gpio.BCM)



""" Configurando GPIO """
# Configurando o modo dos pinos como BCM
gpio.setmode(gpio.BCM)

# Configurando PIN como INPUT 
gpio.setup(SWITCH1,gpio.IN)
gpio.setup(SWITCH2,gpio.IN)
gpio.setup(SWITCH3,gpio.IN)


API_URL="http://0.0.0.0:8000/api/v1/breathing/"

while True:

	while gpio.input(SWITCH1) == 1:
		print("BEBE AUSENTE")
		content = {
			"status": "absent"
		}
		requests.post(API_URL, json=content)
        	time.sleep(2)
	
	while gpio.input(SWITCH2) == 1:
		print("BEBE RESPIRANDO")
		content = {
			"status": "breathing"
		}
		requests.post(API_URL, json=content)
                time.sleep(2)
	
	while gpio.input(SWITCH3) ==1:
		print("MORTO")
		content = {
			"status": "no breathing"
		}
		requests.post(API_URL, json=content)
		time.sleep(2)

gpio.cleanup()
exit()



