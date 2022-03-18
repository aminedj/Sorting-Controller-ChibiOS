#include "MAX7300.h"
#include <Wire.h>
#include <Arduino.h>

MAX7300::MAX7300()
{
}
uint8_t MAX7300::i2cWrite(uint8_t device_address, uint8_t registerAddress, uint8_t data)
{
    uint8_t state;
    Wire.beginTransmission(address);
    Wire.write(registerAddress);
    Wire.write(data);
    state = Wire.endTransmission();
    return state;
}
void MAX7300::statusUpdate(uint8_t state, char operation, uint8_t ports = 0x00)
{
    switch (operation)
    {
    case 'm':
        Serial.print("Mode Set Up: ");
        break;
    case 'p':
        Serial.print("Port Configuration Bank at Address ");
        Serial.print(ports);
        Serial.print(" Output Set Up: ");
        break;
    case 'r':
        Serial.print("Port address at ");
        Serial.print(ports);
        break;
    default:
        Serial.print("Unknown Mode: ");
        break;
    }
    if (state == 0)
    {
        Serial.println("Sucessfully");
    }
    else if (state == 1)
    {
        Serial.println("ERROR 1 Data Exceed Buffer Length of 32 bits");
    }
    else if (state == 2)
    {
        Serial.println("ERROR 2 NACK On Address Transmit");
    }
    else if (state == 3)
    {
        Serial.println("ERROR 3 NACK On Transmit Data");
    }
    else
    {
        Serial.println("ERROR 4 I2C Write Failed! Unknown Error");
    }
}
void MAX7300::begin()
{
    // Init WIRE library
    Serial.println();
    Serial.println("-----------------------------------");
    Serial.println("Port Expanders Set Up");
    Wire.begin();
    Wire.setClock(400000);
    // scan for available MAX7300 devices and set up the addreses array
    for (address = 0x40; address <= 0x4F; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            Serial.print("Setting up port expander device: ");
            Serial.println(address, HEX);
            // Store devices adress in array
            addresses[devices] = address;
            // set up the mode as normal operation
            state = i2cWrite(address, 0x04, 1);
            statusUpdate(state, 'm');
            // Set up all ports as output
            for (uint8_t j = 0x09; j <= 0x0F; j++)
            {
                state = i2cWrite(address, j, 0x55); // Set up all ports to output
                statusUpdate(state, 'p', j);
            }
            // Reset all ports to off state on the port expander
            for (uint8_t ports = 0x2C; ports <= 0x3F; ports += 1)
            {
                state = i2cWrite(address, ports, 0x00);
                statusUpdate(state, 'r', ports);
                pins++;
            }
            Serial.println();
            devices++;
        }
    }
    if (devices < 1)
    {
        Serial.println("No port exapander found! Please Check connection");
        Serial.print("Port iteration: ");
        for (uint8_t ports = 0x4C; ports <= 0x5C; ports += 8)
        {
            Serial.print("0x");
            Serial.print(ports, HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    else
    {
        Serial.print("Total Devices: ");
        Serial.println(devices);
        Serial.print("Total Pins Available: ");
        Serial.print(pins);
        Serial.print("/");
        Serial.println(devices * 20);
        // set up port addresses array starting from 0x2C (port 12 in datasheet) --> 0x3F (port 31 in datasheet)
        for (uint8_t i = 0x2C; i <= 0x3F; i++)
        {
            port_addresses[i - 0x2C] = i;
        }
    }
}
void MAX7300::setHigh(uint8_t port)
{
    if (port >= 1 && devices > 0)
    {
        Wire.beginTransmission(addresses[(port - 1) / 20]);
        Wire.write(port_addresses[(port - 1) % 20]);
        Wire.write(0x01);
        // state =
        Wire.endTransmission();
        // Serial.println(state);
        // i2cWrite(addresses[(port-1)/20],port_addresses[(port-1)%20],0x00);
    }
}
void MAX7300::setLow(uint8_t port)
{
    if (port >= 1 && devices > 0)
    {
        Wire.beginTransmission(addresses[(port - 1) / 20]);
        Wire.write(port_addresses[(port - 1) % 20]);
        Wire.write(0x00);
        // state =
        Wire.endTransmission();
        // Serial.println(state);

        // i2cWrite(addresses[(port-1)/20],port_addresses[(port-1)%20],0x01);
    }
}
void MAX7300::setAll(uint8_t state)
{
    if (devices > 0)
    {
        uint8_t availableDevices = 0x40 + (devices - 1);
        for (address = 0x40; address <= availableDevices; address++)
        {
            for (uint8_t ports = 0x2C; ports <= 0x3F; ports += 1)
            {
                state = i2cWrite(address, ports, state);
            }
        }
    }
}
void MAX7300::setAllPortofDevice(uint8_t device, uint8_t state)
{
    for (uint8_t ports = 0x2C; ports <= 0x3F; ports++)
    {
        Wire.beginTransmission(addresses[device - 1]);
        Wire.write(ports);
        Wire.write(!state);
        Wire.endTransmission();
    }
}

// void MAX7300::setGroup(int startPort,int endPort,String state){
//     //In development
// }
