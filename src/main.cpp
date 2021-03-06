
// Teensy4_WebClientRepeatingTLS
// Teensy 4.1 repeatedly downloads a website via ssl secured transmission

// This is an adapted version for Platformio of:
// https://github.com/vjmuzik/NativeEthernet/tree/master/examples/WebClientRepeatingTLS


// Make sure that the latest library version of NativeEthernet is included (see platformio.ini)
// The version actually (2021/03/14) included in .platformio/packages/framework-arduinoteensy
// doesnt't work

// https://github.com/vjmuzik/NativeEthernet
// https://github.com/vjmuzik/FNET

#include <Arduino.h>

#include <NativeEthernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
uint8_t mac[6];
void teensyMAC(uint8_t *mac) {
    for(uint8_t by=0; by<2; by++) mac[by]=(HW_OCOTP_MAC1 >> ((1-by)*8)) & 0xFF;
    for(uint8_t by=0; by<4; by++) mac[by+2]=(HW_OCOTP_MAC0 >> ((3-by)*8)) & 0xFF;
    Serial.printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// initialize the library instance:
EthernetClient client;

char server[] = "pjrc.com";  // also change the Host line in httpRequest()

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

//Forward declarations
void httpRequest();

void setup() {
  teensyMAC(mac);
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // some interesting links
  //-https://forum.pjrc.com/threads/63373-Teensy-4-1-ethernet-webserver-problem 
  //-https://forum.pjrc.com/threads/60857-T4-1-Ethernet-Library
  //-https://github.com/khoih-prog/EthernetWebServer_SSL#read-buffer-overflow


  // Eventually make changes of the Ethernet parameter for large websites
  //Ethernet.setStackHeap(1024 * 128);
  //Ethernet.setSocketSize(1024 * 6);
  //Ethernet.setSocketNum(2);
  
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  
  // you're connected now, so print out the data
  Serial.print(F("You're connected to the network, IP = "));
  Serial.println(Ethernet.localIP());
  delay(1000);
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
    
    if (client.connect(server, 443, true)) {  //Uncomment for encrypted connection
    //if (client.connect(server, 80))  {  // Uncomment for unencrypted connection
     
    Serial.println("connecting...");
  
    // send the HTTP GET request:
    
    client.println("GET /about/contact.html HTTP/1.1");
    ///about/contact.html
    client.println("Host: www.pjrc.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    client.close(); //If "Connection: close" make sure to close after printing and before stop to avoid harsh reset
    
    // note the time that the connection was made:
    lastConnectionTime = millis();  
  } else 
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  } 
}
