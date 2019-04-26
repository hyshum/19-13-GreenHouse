import requests

def turnOn():
	r = requests.get( "https://maker.ifttt.com/trigger/ec464greenhouseON/with/key/cvJYmevJ910Cxw7Zr5Y6Ac", {} )
	print( r )

def turnOff():
	r = requests.get( "https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac", {} )
	print( r )

turnOn()
