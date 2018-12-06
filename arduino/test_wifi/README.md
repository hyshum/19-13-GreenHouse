Must install dht library and ESP8266Wifi library. Must change line 116 of dht.cpp library file to the following:

    volatile uint8_t *PIR = (volatile uint8_t*)portInputRegister(port);
