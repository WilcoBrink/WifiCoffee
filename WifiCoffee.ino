#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include "wifi_config.h"

const char* host = "koffiemachine";			//access device from http:// host (not working atm)

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

#define PIN_RELAY			10
#define	PIN_FILTERTRAY		16
#define	PIN_WATERLEVEL		5
#define	PIN_MUGSENSORLEFT	13
#define	PIN_MUGSENSORMIDDLE	12
#define	PIN_MUGSENSORRIGHT	14
#define	PIN_SWITCH			4

void setup() {
	pinMode(PIN_RELAY, OUTPUT);
	
	pinMode(PIN_FILTERTRAY, INPUT_PULLDOWN_16);
	pinMode(PIN_WATERLEVEL, INPUT_PULLUP);
	pinMode(PIN_MUGSENSORLEFT, INPUT);
	pinMode(PIN_MUGSENSORMIDDLE, INPUT);
	pinMode(PIN_MUGSENSORRIGHT, INPUT);
	pinMode(PIN_SWITCH, INPUT_PULLDOWN_16);

	digitalWrite(PIN_RELAY, HIGH);		//turn off the relay before starting wifi to minimize current draw

	SPIFFS.begin();

	//code for connecting to best known network
	wifiMulti.addAP(NETWORK1_SSID, NETWORK1_PASS);
	wifiMulti.addAP(NETWORK2_SSID, NETWORK2_PASS);

	WiFi.hostname(host);

	while(wifiMulti.run() != WL_CONNECTED) {
		delay(500);
	}
  
	server.onNotFound([](){server.send(404, "text/html", "Command Not Found");});

	server.on("/make_coffee", HTTP_POST, []() {
		String value = server.arg("value");
		MakeCoffee(value.toInt());
	});

	server.on("/debug_data", HTTP_GET, []() {
		SendDebugData();
	});

	server.serveStatic("/index.html", SPIFFS, "/index.html");
	server.serveStatic("/script.js", SPIFFS, "/script.js");
	server.serveStatic("/stylesheet.css", SPIFFS, "/stylesheet.css");
	server.serveStatic("/favicon.png", SPIFFS, "/favicon.png");
	server.serveStatic("/", SPIFFS, "/index.html");
	server.serveStatic("/debug.html", SPIFFS, "/debug.html");
	server.serveStatic("/coffeecup.png", SPIFFS, "/coffeecup.png");

	server.begin();
}

void loop(void){
	server.handleClient();
}

void MakeCoffee(int amount) {
	server.send(200, "text/plain", "Making Coffee");
	delay(1000);
	server.send(200, "text/plain", "Finished");
}

void SendDebugData() {
	char temp[500];

	snprintf(temp, 500,
			"<?xml version = \"1.0\" ?>\n<debug_data>\n<water_level>%s</water_level>\n<dispenser>%s</dispenser>\n<mug_left>%s</mug_left>\n<mug_middle>%s</mug_middle>\n<mug_right>%s</mug_right>\n<switch>%s</switch>\n</debug_data>",
		digitalRead(PIN_WATERLEVEL) ? "Empty" : "Full",
		digitalRead(PIN_FILTERTRAY) ? "Not Placed" : "Placed",
		digitalRead(PIN_MUGSENSORLEFT) ? "Available" : "Missing",
		digitalRead(PIN_MUGSENSORMIDDLE) ? "Available" : "Missing",
		digitalRead(PIN_MUGSENSORRIGHT) ? "Available" : "Missing",
		digitalRead(PIN_SWITCH) ? "On" : "Off"
	);

	server.send(200, "text/xml", temp);
}
