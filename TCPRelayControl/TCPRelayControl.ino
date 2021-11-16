/*
 DHCP Chat  Server

 A simple server that distributes any incoming messages to all
 connected clients.  To use, telnet to your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield.

 THis version attempts to get an IP address using DHCP

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 21 May 2011
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 Based on ChatServer example by David A. Mellis

 */

#include <SPI.h>
#include <EthernetENC.h>

// Buffer to hold commands. 
// Commands are 4 characters. 
// NN:S where NN is the relay number (01 - 16) and S is the state (1 for on and 0 for off)
// "\n" sends the command
char buffer[80];

// Holds pointer to where in the buffer to write
int bufferLoc = 0;

// Maps position in array to pin
const int pinMapping[] = {2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 19, 18, 17, 16, 15, 14};

// Pin numbers corresponding to 

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0x00, 0xAA, 0xC4, 0x34, 0x12, 0x51
};

/* Add this if don't want to use DHCP
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
*/

// telnet defaults to port 23
EthernetServerPrint server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void setAllPins(int state){
  for(int n = 0; n < (sizeof(pinMapping) / sizeof(pinMapping[0])); n++){
          digitalWrite(pinMapping[n], state);
   }
}


void setup() {

  
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  //Ethernet.init(13);

  // Open serial communications and wait for port to open:
 // Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // start the Ethernet connection:
//  Serial.println("Trying to get an IP address using DHCP!");
  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP!");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
 //     Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
 //   Serial.println("Trying to get an IP address using DHCP!");
    if (Ethernet.linkStatus() == LinkOFF) {
 //     Serial.println("Ethernet cable is not connected.");
    }
    // initialize the Ethernet device not using DHCP:
    // Ethernet.begin(mac, ip, myDns, gateway, subnet);
  } 
  // print your local IP address:

  // start listening for clients
  server.begin();
    // Set pins to output and low
  for(int i = 0; i < (sizeof(pinMapping) / sizeof(pinMapping[0])); i++){
    pinMode(pinMapping[i], OUTPUT);
    digitalWrite(pinMapping[i], HIGH);
  }
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {

    // read the bytes incoming from the client:
    char thisChar = client.read();

    // Note: char == 10 is "\n"
    // Check if we've filled the buffer and hit a newline
    if (thisChar == 10 && (((bufferLoc + 1) % 5) == 0) && bufferLoc < 80 && bufferLoc >= 4) {
      
      String firstCommand = String(buffer).substring(0,4);
      int i = 0; // Counter used for looping through commands

      // Check for initial all on or off command
      if(firstCommand.equalsIgnoreCase("All0")){
        i = i + 4; // Update starting point for later loop
        setAllPins(HIGH);
      } else if (firstCommand.equalsIgnoreCase("All1")){
        i = i + 4;
        setAllPins(LOW);
      }

      // Loops through remaining commands
      for(int i = 0; i < bufferLoc; i=i+5)
      {
        int relay = String(buffer).substring(i, i+2).toInt();
        
        if (buffer[i + 3] == '1'){ // Turn on
          digitalWrite(pinMapping[relay], LOW);
        } else if (buffer[i + 3] == '0') { // Turn off
          digitalWrite(pinMapping[relay], HIGH);
        }
      }
      bufferLoc = 0;
      client.println("OK");

    } else if (thisChar == 10) {   // Did we overfill the before or hit newline before hitting buffer?
      client.print("Invalid command: ");
      client.println(String(buffer));
      bufferLoc = 0;
    } else { // Continue to fill buffer
      buffer[bufferLoc] = thisChar;
      bufferLoc++;
    }
//    // echo the bytes back to the client:
//    server.write(thisChar);
//    // echo the bytes to the server as well:
    Ethernet.maintain();
  }
}
