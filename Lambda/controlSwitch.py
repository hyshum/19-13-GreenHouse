import requests

def turnOn():
	requests.get( "https://maker.ifttt.com/trigger/ec464greenhouseON/with/key/cvJYmevJ910Cxw7Zr5Y6Ac", {} )

def turnOff():
	requests.get( "https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac", {} )

turnOn()
