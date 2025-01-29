V9240 driver prototype

Datasheet [https://www.vangotech.com/uploadpic/163903940488.pdf]

The board with this chip was obtained here [https://www.aliexpress.com/item/1005004083630967.html]
The device did not work from the very beginning for some unknown reason, so it was disassembled and examined.
Now there is a desire to make a smart socket based on it, so perhaps in the near future a fork of the tasmota project repository will appear here
Voltage and frequency measurement is working now. I will check the current measurement later when I get the current transformer, 
I don't want to get electrocuted so all connections are through transformers.
A FT232 board was used to communicate with the computer.
Also needed a diode (1n4148) and a 10k resistor.

And this program requires the Qt library with the serial port module. It is event-oriented in the Qt style, 
so I think it should not be difficult to adapt it to the requirements of the tasmot project, 
make calls to the driver class methods from the callback function and so on.
