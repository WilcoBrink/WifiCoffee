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

enum StatusCodes
{
	WaitingForCommand,
	BrewingOneCup,
	BrewingTwoCup,
	BrewingFinished,
	WaterEmpty,
	FilterNotAvailable,
	FilterNotRefilled,
	MugMissingLeft,
	MugMissingMiddle,
	MugMissingRight,
	MugMissingBoth,
	InvalidCommand
};

StatusCodes brewingStatus = WaitingForCommand;

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

	server.on("/brewing_status", HTTP_GET, []() {
		SendBrewingStatus();
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
	if (amount == 1)
		brewingStatus = BrewingOneCup;
	else if (amount == 2)
		brewingStatus = BrewingTwoCup;
	else
		brewingStatus = InvalidCommand;
	yield();
	//delay(1000);
	brewingStatus = BrewingFinished;
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

void SendBrewingStatus() {
	switch (brewingStatus) {
	case WaitingForCommand:server.send(200, "text/plain", "How many cups do you want to brew?"); break;
	case BrewingOneCup:server.send(200, "text/plain", "Brewing one cup of coffee"); break;
	case BrewingTwoCup:server.send(200, "text/plain", "Brewing two cups of coffee"); break;
	case BrewingFinished:server.send(200, "text/plain", "Brewing has finished"); break;
	case WaterEmpty:server.send(200, "text/plain", "Please refill water reservoir"); break;
	case FilterNotAvailable:server.send(200, "text/plain", "Please place the filter tray"); break;
	case FilterNotRefilled:server.send(200, "text/plain", "Please refill the filter tray"); break;
	case MugMissingLeft:server.send(200, "text/plain", "Please place a mug on the left"); break;
	case MugMissingMiddle:server.send(200, "text/plain", "Please place a mug in the middle"); break;
	case MugMissingRight:server.send(200, "text/plain", "Please place a mug on the right"); break;
	case MugMissingBoth:server.send(200, "text/plain", "Please place both mugs"); break;
	default:server.send(200, "text/plain", "Invalid brewing status");break;
	}
}
