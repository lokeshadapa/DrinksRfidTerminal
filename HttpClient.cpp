/*
* "Drinks" RFID Terminal
* Buy your sodas with your company badge!
*
* Benoit Blanchon 2014 - MIT License
* https://github.com/bblanchon/DrinksRfidTerminal
*/

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Dns.h>

#include "Configuration.h"
#include "HttpClient.h"

#define xstr(s) str(s)
#define str(s) #s

void HttpClient::begin()
{
	delay(100);

	byte mac[6] = MAC_ADDRESS;

	Serial.println("DHCP...");

	// start the Ethernet connection:
	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		// no point in carrying on, so do nothing forevermore:
		for (;;)
			;
	}

	Serial.print("Address=");
	Serial.println(Ethernet.localIP());
	
	Serial.print("Subnet=");
	Serial.println(Ethernet.subnetMask());
	
	Serial.print("DNS=");
	Serial.println(Ethernet.dnsServerIP());

	Serial.println("Resolve " SERVER_NAME "...");
	
	DNSClient dns;

	dns.begin(Ethernet.dnsServerIP());

	while (1 != dns.getHostByName(SERVER_NAME, serverIp));

	Serial.print("Address=");
	Serial.println(serverIp);

}

bool HttpClient::connect()
{
	if (!client.connect(serverIp, SERVER_PORT))
	{
		Serial.println("Connect failed");
		return false;
	}

	return true;
}

void HttpClient::disconnect()
{
	client.stop();
}

void HttpClient::println()
{
	client.println();
}

void HttpClient::println(char* s)
{
	client.println(s);
}

void HttpClient::readln(char* buffer, int size)
{
	readblock(buffer, size, '\n');
}

bool HttpClient::readblock(char* buffer, int size, char terminator)
{
	int i;

	for (i = 0; i < size - 1; i++)
	{
		while (!client.available())
		{
			if (!client.connected())
			{
				buffer[i] = 0;
				return false;
			}
		}

		buffer[i] = client.read();
		//Serial.print(buffer[i]);

		if (buffer[i] == terminator)
			break;
	}

	buffer[i] = 0;
	return true;
}


void HttpClient::skipHeaders()
{
	char line[32];
	do
	{
		readln(line, sizeof(line));
	} while (line[0] != '\r' && line[0] != '\n');
}

void HttpClient::sendCommonHeader(char* verb, char* path)
{
	client.print(verb);
	client.print(" ");
	client.print(path);
	client.println(" HTTP/1.1");
	client.println("Host: " SERVER_NAME ":" xstr(SERVER_PORT));
	client.println("Accept: application/json");
	client.println("Connection: close");
}

bool HttpClient::performGetRequest(char* path, char* content, int maxContentSize)
{
	Serial.print("GET ");
	Serial.println(path);

	if (!connect()) return false;

	sendCommonHeader("GET", path);
	client.println();

	skipHeaders();
	readln(content, maxContentSize);

	Serial.println(content);

	disconnect();

	return true;
}

bool HttpClient::performPostRequest(char* path, char* content, int maxContentSize)
{
	Serial.print("POST ");
	Serial.println(path);
	Serial.println(content);

	if (!connect()) return false;

	sendCommonHeader("POST", path);
	client.println("Content-Type: application/json");
	client.print("Content-Length: ");
	client.println(strlen(content));
	client.println();
	client.println(content);

	skipHeaders();
	readln(content, maxContentSize);

	Serial.println(content);

	disconnect();

	return true;
}


