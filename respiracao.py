# -*- coding: utf-8 -*-
#! /usr/bin/python

import RPi.GPIO as gpio
import time
import requests

""" Global """
SWITCH1=4
SWITCH2=22
SWITCH3=27

#""" Funcoes """
#def action_press_button_loop(gpio_pin):
# while True:
#        try:
#                gpio.wait_for_edge(SWITCH2, gpio.RISING) #detecta, escreve e sa$
#                print("Entrada 22 em nível ALTO: BEBE PAROU DE RESPIRAR")
                #subprocess.call(["halt"])


#        except KeyboardInterrupt: #sai do programa com CRTL+c
#                gpio.cleanup()
#                exit()


#def action_press_button(gpio_pin):
# while True:
#        print("RESPIRANDO")


#        time.sleep(1)

        #if gpio.event_detected (SWITCH2):
        #       print("SEM RESPIRAÇÃO")
#       try:
#                gpio.wait_for_edge(SWITCH2, gpio.RISING) #detecta, escreve e s$
#                print("Entrada 22 em nível ALTO: BEBE PAROU DE RESPIRAR")
#                #subprocess.call(["halt"])
#                time.sleep(1)



        #if gpio.event_detected (SWITCH2):
        #       print("SEM RESPIRAÇÃO")
#       try:
#                gpio.wait_for_edge(SWITCH2, gpio.RISING) #detecta, escreve e s$
#                print("Entrada 22 em nível ALTO: BEBE PAROU DE RESPIRAR")
#                #subprocess.call(["halt"])
#                time.sleep(1)

#        except KeyboardInterrupt: #sai do programa com CRTL+c
#                gpio.cleanup()
#                exit()



""" Configurando GPIO """
# Configurando o modo dos pinos como BCM
gpio.setmode(gpio.BCM)

#                gpio.cleanup()
#                exit()



""" Configurando GPIO """
# Configurando o modo dos pinos como BCM
gpio.setmode(gpio.BCM)

# Configurando PIN como INPUT e modo pull-donw interno
####gpio.setup(SWITCH1, gpio.IN, pull_up_down = gpio.PUD_UP)
####gpio.setup(SWITCH2, gpio.IN, pull_up_down = gpio.PUD_UP)

gpio.setup(SWITCH1,gpio.IN)
gpio.setup(SWITCH2,gpio.IN)
gpio.setup(SWITCH3,gpio.IN)

# Adicionando um evento ao GPIO 23 na mudança FALLING 0V[LOW] - > 3.3V[HIGH]
####gpio.add_event_detect(SWITCH1, gpio.RISING, callback=action_press_button)
####gpio.add_event_detect(switch1, gpio.RISING)
####gpio.add_event_detect(SWITCH2, gpio.RISING)
####gpio.add_event_detect(SWITCH3, gpio.RISING)
# Junto com o parametro callback podemos utilizar ainda o bouncetime
# na linha abaixo estamos dizendo para ignorar nos primeiro 300ms
# gpio.add_event_detect(PIN, gpio.RISING, callback=action_press_button, bouncet$

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
			"status": "no_breathing"
		}
		requests.post(API_URL, json=content)
		time.sleep(2)

gpio.cleanup()
exit()



