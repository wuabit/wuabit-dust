#include "DigiKeyboard.h"
#include "usha256.h"
#include <Entropy.h>

//Uncomment this to generate a continuous flow of keys to analyze:
#define TEST_PRNG

//Magic number to know if the device is initialized with a stored key. Change it to reset.
#define STATE_INITIALIZED 0xAC

Sha256Context ctx;
uint8_t hash[33];
#define HASH_SIZE sizeof(hash) - 1

#ifdef TEST_PRNG
#include <avr/io.h>
#define LED PB1
uint32_t TEST_ITERATIONS = 1024 * 1024 / HASH_SIZE;
#else
#include <avr/eeprom.h>
uint8_t EEMEM eHash[33];
#endif

void printNibble(uint8_t n)
{
  DigiKeyboard.print(n < 10 ? (char)(n + '0') : (char)((n - 10) + 'a'));
}

void sendHash()
{
  uint8_t h;
  for (uint8_t i = 0; i < HASH_SIZE; i++)
  {
    h = hash[i];
    printNibble(h >> 4);
    printNibble(h & 0xf);
  }
#ifdef TEST_PRNG
  DigiKeyboard.print('\n');
#endif
}

void setup()
{
  Entropy.initialize();
#ifdef TEST_PRNG
  //setup LED pin for output in port B's direction register
  DDRB |= (1 << LED);  
#else
  uint8_t state;
  eeprom_read_block((uint8_t *)hash, (const void *)&eHash, sizeof(hash));
  state = hash[HASH_SIZE];
  if (state != STATE_INITIALIZED)
  {
    sha256_init(&ctx);
    for (uint8_t i = 0; i < HASH_SIZE; i += sizeof(uint32_t))
    {
      *(uint32_t *)(hash + i) = Entropy.random();
    }
    sha256_update(&ctx, (uint8_t *)hash, HASH_SIZE);
    sha256_final(&ctx, hash);
    hash[HASH_SIZE] = STATE_INITIALIZED;
    eeprom_write_block((uint8_t *)hash, (void *)eHash, sizeof(hash));
  }
  sendHash();
#endif
}

void loop()
{
#ifdef TEST_PRNG
  if (--TEST_ITERATIONS >= 0)
  {
    PORTB |= (1 << LED);
    sha256_init(&ctx);
    for (uint8_t i = 0; i < HASH_SIZE; i += sizeof(uint32_t))
    {
      *(uint32_t *)(hash + i) = Entropy.random();
    }
    sha256_update(&ctx, (uint8_t *)hash, HASH_SIZE);
    sha256_final(&ctx, hash);
    PORTB &= ~(1 << LED);
    sendHash();
  }
#endif
}
