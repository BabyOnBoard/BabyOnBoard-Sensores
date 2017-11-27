#!/usr/bin/env python
#-*- coding: utf-8 -*-

import wave
import numpy as np
import matplotlib
matplotlib.use('Pdf')
#import matplotlib.pyplot as plt
import math
import requests

wf = wave.open('/home/pi/Git/BabyOnBoard-Sensores/teste.wav', 'r')
fs = wf.getframerate()
ts=fs

sinal =(wf.readframes(fs))
ref=32767

Amplitude = (np.fromstring(sinal, dtype=np.int16))/(np.sqrt(2))

rms = np.sqrt(np.mean(Amplitude**2))
dbspl = 94 + (20*np.log10(rms/ref))

# Janela de Hamming
win = np.hamming(len(Amplitude))
Amplitude = (Amplitude * win)

AMP = np.fft.fft(Amplitude)
FREQ = np.fft.fftfreq(ts, 1.0/(len(Amplitude)))

#Corrigindo a magnitude da FFT pela janela de Hamming e a energia do sinal
#multiplicando por 2 por conta da utilização de metade da FFT

AMP_MAG = np.abs(AMP) * 2 / np.sum(win)

# Para obter o valor RMS da FFT, basta multiplica por raiz de 2
AMP_RMS = AMP_MAG / np.sqrt(2)

# Deslocando as amostras de magnitude e frequência para obter  
# o gráfico em db
FREQ   = np.fft.fftshift(FREQ)
AMP_RMS = np.fft.fftshift(AMP_RMS)

# Conversão para escala em DB
AMP_DB = (20 * np.log10( AMP_RMS/ref )) + 94

###plt.semilogx(FREQ, AMP_DB)
###plt.figure(1)
###plt.xlim( (0, fs/2)  )
###plt.ylim( (-200, 200))

#plt.figure(2)
#plt.plot(FREQ, AMP_RMS)
#plt.grid('on')

###plt.show()
# Comparando as Amplitudes
print('{} {}'.format(dbspl, AMP_DB.max()))

#Valor definido como inicio do choro do bebê db=50
#Escrita em arquivo

if (dbspl >=50 or (AMP_DB.max() >= 60) ):
	content = {
		"status": "true"
	}
	requests.post('http://0.0.0.0:8000/api/v1/noise/', json=content)
#	arquivo= open('/home/pi/Desktop/Douglas/arq_estado.txt', 'w')
#	arquivo.write("{\"estado\": \"criança chorando\"}")
#	arquivo.close()
else:
	content = {
		"is_crying": "false"
	}
	requests.post('http://0.0.0.0:8000/api/v1/noise/', json=content)
#	arquivo= open('/home/pi/Desktop/Douglas/arq_estado.txt', 'w')
#	arquivo.write("{\"estado\": \"não está chorando\"}")
#	arquivo.close()

