/*

 Imgur Image Upload library for Arduino

 This library is an adopted version of the TwitPic library by arms22. 
 http://code.google.com/p/arms22/

 Copyright (C) 2010 Alexander Weber, tinkerlog.com

 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <string.h>
#include <stdlib.h>
#include "Imgur.h"

#define IMGUR_UPLOAD_API_URL "/api/upload.xml"
#define BOUNDARY             "637bc24727e2"
#define HEADER               "--" BOUNDARY
#define FOOTER               "--" BOUNDARY "--"
#define IMAGE_CONTENT_TYPE   "image/jpeg"

#define SERIAL_DEBUG 1
#if SERIAL_DEBUG
#include <HardwareSerial.h>
#define DEBUG_PRINT(c) Serial.print(c)
#define DEBUG_PRINTLN(c) Serial.println(c)
#else
#define DEBUG_PRINT(c)
#define DEBUG_PRINTLN(c)
#endif

#define IMGUR "imgur.com"

Imgur::Imgur(const char *appKey) : client(IMGUR, 80), appKey(appKey) {
}

int Imgur::upload(uint32_t imageLength, void (*imageTransfer)(Client*client)) {
  int ret = -1;

  DEBUG_PRINT("connecting ...");

  if (client.connect()) {
    DEBUG_PRINTLN("ok\r\nposting image ...");
    
    uint32_t length = 16 + 44 + 2 + 34 + 18 + 16 + 66 + 26 + 2 + imageLength + 2 + 18 + 2;

    println_P(PSTR("POST " IMGUR_UPLOAD_API_URL " HTTP/1.0") );
    println_P(PSTR("Content-Type: multipart/form-data; boundary=" BOUNDARY) );
    print_P(PSTR("Content-Length: ") );
    client.println(length);
    client.println();
    
    // multi-part key
    println_P(PSTR(HEADER));  // 14+2
    println_P(PSTR("Content-Disposition: form-data; name=\"key\"")); // 42+2
    client.println(); // 2
    client.println(appKey); // 32+2
    println_P(PSTR(FOOTER)); // 16+2

    // multi-part image
    println_P(PSTR(HEADER)); // 14+2

    println_P(PSTR("Content-Disposition: form-data; name=\"image\"; filename=\"pic.jpg\"")); // 64+2
    println_P(PSTR("Content-Type: image/jpeg")); // 24+2
    client.println(); // 2
    imageTransfer(&client); // image length
    client.println(); // 2
    println_P(PSTR(FOOTER)); // 16+2
    client.println();  // 2
    client.flush();
    
    DEBUG_PRINTLN("waiting for response ...");
    ret = readResponse();
    DEBUG_PRINTLN("connection closed");
    client.stop();
  }
  else{
    DEBUG_PRINTLN("failed");
  }
  return ret;
}


int Imgur::readResponse(void) {
  int ret = -1;
  skipUntil_P(PSTR("Status: "));
  read(statusCode, 3);
  statusCode[3] = '\0';
  skipUntil_P(PSTR("original_image>"));
  readToken(imgURL, '<');

  while (client.connected()) {    
    if (client.available()) {
      char c = client.read();
      DEBUG_PRINT(c);
    }
  }
  return atoi(statusCode);
}

int Imgur::readToken(char *buf, char delimiter) {
  int c = 0;
  while (client.connected()) {
    if (client.available()) {
      c = client.read();
      DEBUG_PRINT(c);
      if ((c == delimiter) || (c == -1)) {
	break;
      }
      *buf++ = c;
    }
  }
  *buf = '\0';
  return c;
}


int Imgur::read(char *buf, int size) {
  int ret = 0;
  while (client.connected() && (ret<size)) {
    while (client.available() && (ret<size)) {
      char c = client.read();
      DEBUG_PRINT(c);
      buf[ret++] = c;
    }
  }
  buf[ret] = 0;
  return ret;
}

int Imgur::skipUntil_P(const prog_char *str) {
  int pos = 0;
  char cc = pgm_read_byte(str+pos);
  while (client.connected() && cc) {
    while (client.available() && cc) {
      char c = client.read();
      DEBUG_PRINT(c);
      if (cc == c) {
	pos++;
      } 
      else {
	pos=0;
      }
      cc = pgm_read_byte(str+pos);
    }
  }
  return pos;
}

void Imgur::print_P(const prog_char *str) {
  char c;
  while(c = pgm_read_byte(str++)){
    client.print(c);
  }
}

void Imgur::println_P(const prog_char *str) {
  print_P(str);
  client.println();
}

