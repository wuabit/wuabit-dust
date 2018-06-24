//
// Wuabit Dust vSHA-256 is a 256bits password/key usb generator.
//
// Copyright 2018 by Manuel Polo (https://about.me/mrmx)
//
// This file is part of Wuabit Dust vSHA-256 a 256bits password/key usb generator.
// Wuabit Dust vSHA-256  is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wuabit Dust vSHA-256 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Wuabit Dust vSHA-256.  If not, see <http://www.gnu.org/licenses/>.
//
#include "DigiKeyboard.h"
#include <usha256.h>
#include <Entropy.h>
#include <avr/io.h>

#define LED PB1

//Uncomment this to generate a continuous flow of keys to analyze:
//#define TEST_PRNG

//Magic number to know if the device is initialized with a stored key. Change it to reset.
#define STATE_INITIALIZED 0xAC

Sha256Context ctx;
uint8_t keyData[33];

#define KEY_SIZE sizeof(keyData) - 1
#define REGENERATE_COUNTDOWN 1 * 60 //seconds
#define REGENERATE_LED_WARN_COUNT 3 * REGENERATE_COUNTDOWN >> 2

#ifdef TEST_PRNG
uint32_t TEST_ITERATIONS = 512;
#else
#include <avr/eeprom.h>
uint8_t EEMEM eepromData[33];
int8_t REGENERATE_KEY_COUNTDOWN = REGENERATE_COUNTDOWN;
uint16_t REGENERATE_KEY_DELAY = 1024; //ms
#endif

void printNibble(uint8_t n)
{
  DigiKeyboard.print(n < 10 ? (char)(n + '0') : (char)((n - 10) + 'a'));
}

void sendKey()
{
  uint8_t h;
  for (uint8_t i = 0; i < KEY_SIZE; i++)
  {
    h = keyData[i];
    printNibble(h >> 4);
    printNibble(h & 0xf);
  }
#ifdef TEST_PRNG
  DigiKeyboard.print('\n');
#endif
}

void generateRandomKey()
{
  for (uint8_t i = 0; i < KEY_SIZE; i += sizeof(uint32_t))
  {
    *(uint32_t *)(keyData + i) = Entropy.random();
  }
}

void hashSeed()
{
  sha256_init(&ctx);
  sha256_update(&ctx, (uint8_t *)keyData, KEY_SIZE);
  sha256_final(&ctx, keyData);
}

void led(bool on)
{
  if (on)
  {
    PORTB |= (1 << LED);
  }
  else
  {
    PORTB &= ~(1 << LED);
  }
}

void eepromSaveKeyData()
{
  eeprom_write_block((uint8_t *)keyData, (void *)eepromData, sizeof(keyData));
}

void setup()
{
  //setup LED pin for output in port B's direction register
  DDRB |= (1 << LED);
#ifdef TEST_PRNG
  //DigiKeyboard.print("Wuabit Dust test mode will output ");
  //DigiKeyboard.print(TEST_ITERATIONS, DEC);
  //DigiKeyboard.println(" keys");
  Entropy.initialize();
#else
  uint8_t state;
  led(true);
  eeprom_read_block((uint8_t *)keyData, (const void *)&eepromData, sizeof(keyData));
  state = keyData[KEY_SIZE]; //State is at the end of keyData
  if (state != STATE_INITIALIZED)
  {
    Entropy.initialize();
    generateRandomKey();
    hashSeed();
    keyData[KEY_SIZE] = STATE_INITIALIZED;
    eepromSaveKeyData();
  }
  sendKey();
  led(false);
#endif
}

void loop()
{
#ifdef TEST_PRNG
  if (--TEST_ITERATIONS >= 0)
  {
    led(true);
    generateRandomKey();
    hashSeed();
    led(false);
    sendKey();
  }
#else
  if (REGENERATE_KEY_COUNTDOWN >= 0)
  {
    REGENERATE_KEY_COUNTDOWN--;
    if (REGENERATE_KEY_COUNTDOWN < REGENERATE_LED_WARN_COUNT)
    {
      led(false);
    }
    DigiKeyboard.delay(REGENERATE_KEY_DELAY >> 1);
    if (REGENERATE_KEY_COUNTDOWN < REGENERATE_LED_WARN_COUNT)
    {
      led(true);
      if (REGENERATE_KEY_DELAY > 120)
        REGENERATE_KEY_DELAY -= 20; //Blink faster
    }
    DigiKeyboard.delay(REGENERATE_KEY_DELAY >> 1);
  }
  else if (REGENERATE_KEY_COUNTDOWN == -1)
  {
    REGENERATE_KEY_COUNTDOWN--; //Use it as a flag for next blinking loop
    //Generate new key
    Entropy.initialize();
    generateRandomKey();
    hashSeed();
    eepromSaveKeyData();
  }
  //Loop blink signal that key was reset
  if (REGENERATE_KEY_COUNTDOWN == -2)
  {
    led(true);
    DigiKeyboard.delay(50);
    led(false);
    DigiKeyboard.delay(50);
  }
#endif
}
