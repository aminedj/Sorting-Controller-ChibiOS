#include <stdint.h>
#ifndef MAX7300_HPP
#define MAX7300_HPP

class MAX7300
{
private:
 uint8_t error;
 uint8_t state;
 uint8_t address;
 int devices = 0;
 unsigned int pins = 0;
 uint8_t addresses[16];
 uint8_t port_addresses[20];
 uint8_t i2cWrite(uint8_t address, uint8_t registerAddress, uint8_t data);
 void statusUpdate(uint8_t state, char operation, uint8_t ports);

public:
   MAX7300();
   void begin();
   void setHigh(uint8_t port);
   void setLow(uint8_t port);
   void setAll(uint8_t state);
   void setAllPortofDevice(uint8_t device, uint8_t state);
};

#endif