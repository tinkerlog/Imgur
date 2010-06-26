//#include <Ethernet.h>
#include <WiFly.h>
#include <Imgur.h>
#include "SampleImage.h"

// Ethernet parameters
// byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// byte ip[] = {10, 0, 0, 177};
// byte server[] = {0, 0, 0, 0};  // should point to imgur.com

// Wifi parameters
char passphrase[] = "password";
char ssid[] = "wifi-ssid";

char imgurAppKey[] = "insert your imgur app key here";
Imgur imgur(imgurAppKey);

void sampleImageTransfer(Client *client) {
  for (uint32_t i = 0; i < imageSize; i++) {
    client->print(pgm_read_byte(sampleImage+i));
  }
}

void setup() {
  delay(5000);
  
  Serial.begin(9600);  

  // Ethernet.begin(mac, ip);  
  WiFly.begin();

  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }  
  Serial.println("waiting ...");
  delay(10000);
  
  Serial.println("start uploading ...");
  int ret = imgur.upload(imageSize, &sampleImageTransfer);
  
  Serial.print("status: ");
  Serial.print(ret);
  Serial.print(", ");
  Serial.println(imgur.getStatus());
  Serial.print("url: ");
  Serial.println(imgur.getImgURL());
  Serial.println("done.");
}

void loop() {
}

