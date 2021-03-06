/*
 *  CommManager.cpp
 * 
 *  This file is part of CommandStation.
 *
 *  CommandStation is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CommandStation is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "CommManager.h"

#include <Arduino.h>

#if defined(ARDUINO_ARCH_SAMD)
#include <cstdarg>
#endif

CommInterface *CommManager::interfaces[5] = {NULL, NULL, NULL, NULL, NULL};
int CommManager::nextInterface = 0;

void CommManager::update() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->process();
		}
	}
}

void CommManager::registerInterface(CommInterface *interface) {
	if(nextInterface < 5) {
		interfaces[nextInterface++] = interface;
	}
}

void CommManager::showConfiguration() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->showConfiguration();
		}
	}
}

void CommManager::showInitInfo() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->showInitInfo();	
		}
	}
}

void CommManager::printf(const char *fmt, ...) {
	char buf[256] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->send(buf);
		}
	}
}

void CommManager::printf(const __FlashStringHelper *fmt, ...) {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			Stream* mStream = interfaces[i]->getStream();
			va_list args;
			va_start(args, fmt);

			char* flash = (char*)fmt;
			for(int i=0; ; ++i) {
				char c=pgm_read_byte_near(flash+i);
				if (c=='\0') return;
				if(c!='%') { 
					mStream->print(c);
					continue; 
				}
				i++;
				c=pgm_read_byte_near(flash+i);
				switch(c) {
					case '%': mStream->print('%'); break;
					case 's': mStream->print(va_arg(args, char*)); break;
					case 'd': mStream->print(va_arg(args, int), DEC); break;
					case 'b': mStream->print(va_arg(args, int), BIN); break;
					case 'o': mStream->print(va_arg(args, int), OCT); break;
					case 'x': mStream->print(va_arg(args, int), HEX); break;
					case 'f': mStream->print(va_arg(args, double), 2); break;
				}	
			}
			va_end(args);
		}
	}
}