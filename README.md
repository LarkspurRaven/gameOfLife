# gameOfLife

# To Build and Run
## Notes on Build Environment
* This code was built and tested on an Macbook, running MacOS Mojave 10.14.4
* 2.4 GHz Intel Core i5 (x86_64)

## To compile with reduced memory (default), bitfields are used to build the game
gcc game.c LargeGameOps.c BitfieldGameOps.c -o game

## To compile with large memory model, pass flag GAME_LARGE to compiler

gcc game.c LargeGameOps.c BitfieldGameOps.c -DGAME_LARGE -o game

## To build with reduced logging, add this compiler flag
-DREDUCED_LOG

## To Test One File
./game input 1

## To Build and Test with Test Runner
gcc tester.c -o tester

./tester

# Caveats
* Input file does accept non-number values. Charcters or strings are
  interpreted as 0.
* Input values are expected to be uint16_t, but if one enters
   65536 (2^16), this would roll over and be interpreted as 0
   65537 (2^16+1) is interpreted as 1
   65538 is interpreted as 2, so is invalid
