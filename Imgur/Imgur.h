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

#ifndef imgur_h
#define imgur_h

//#include <Ethernet.h>
#include <WiFly.h>
#include <avr/pgmspace.h>

class Imgur {
public:
  Imgur(const char *appKey);
  int upload(uint32_t imageSize, void (*imageTransfer)(Client*client));
  char* getStatus(void) {
    return statusCode;
  }
  char* getImgURL(void) {
    return imgURL;
  }
private:
  int readResponse(void);
  int read(char *buf, int size);
  int readToken(char *buf, char delimiter);
  int skipUntil_P(const prog_char *str);
  void print_P(const prog_char *str);
  void println_P(const prog_char *str);
  const char *appKey;
  char statusCode[4];   // 200
  char imgURL[40];      // http://i.imgur.com/xxxxx.jpg
  Client client;	
};

#endif
