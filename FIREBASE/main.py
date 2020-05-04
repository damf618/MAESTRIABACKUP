#!/usr/bin/python
# -*- coding: utf-8 -*-
# autor: Jefferson Rivera
# Abril de 2018
# email: riverajefer@gmail.com

import sys
from time import sleep
import signal
from gpiozero import LED, Button
from threading import Thread
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

#LED = LED(17)
ALARM_CONTACT = Button(20)
FAIL_CONTACT = Button(21)

PAHT_CRED = '/home/pi/iot/credential_RPI.json'
URL_DB = 'https://maestria1-24022020.firebaseio.com/'
REF_SDAI = 'SISTEMA DE DETECCION INCENDIO DAMF'
REF_Contactos = 'PROPIOS'
REF_CAlarma = 'Alarma'
REF_CFalla = 'Falla'
REF_COMRF = 'ComRF'
REF_RFDSecund = 'RFDSecund'

class IOT():

    def __init__(self):
        cred = credentials.Certificate(PAHT_CRED)
        firebase_admin.initialize_app(cred, {
            'databaseURL': URL_DB
        })

        self.refSDAI = db.reference(REF_SDAI)
        
        self.estructuraInicialDB() # solo ejecutar la primera vez

        self.refContactos = self.refSDAI.child(REF_Contactos)
        self.refCAlarma = self.refContactos.child(REF_CAlarma)
	self.refCFalla = self.refContactos.child(REF_CFalla)
		
	self.refCOMRF = self.refSDAI.child(REF_COMRF)
	self.refRFDSecund = self.refCOMRF.child(REF_RFDSecund)

    def estructuraInicialDB(self):
        self.refSDAI.set({
            'Contactos': {
                'Alarma':True,
                'Falla':True,
				'Extra':False
            },
            'COMRF':{
                'RFDSecundario':True,
                'Extra':False
            }
        })
      
    def contacto_Alarm_on(self):
        print('Alarma ON!')
        self.refCAlarma.set(True)
	
    def contacto_Fail_on(self):
        print('Falla ON!')
        self.refCFalla.set(True)

    def contacto_Alarm_off(self):
        print('Alarma OFF!')
        self.refCAlarma.set(False)
	
    def contacto_Fail_off(self):
        print('Alarma OFF')
        self.refCFalla.set(False)
		

    def botonesStart(self):
        print('Start btn !')
        ALARM_CONTACT.when_pressed = self.contacto_Alarm_on
        FAIL_CONTACT.when_pressed = self.contacto_Fail_on
        ALARM_CONTACT.when_released = self.contacto_Alarm_off
        FAIL_CONTACT.when_released = self.contacto_Fail_off


print ('START !')
iot = IOT()

subproceso_btn = Thread(target=iot.botonesStart)
subproceso_btn.daemon = True
subproceso_btn.start()
signal.pause()
