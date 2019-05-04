# gameOfLife

# To Build and Run
gcc game.c -o game

gcc tester.c -o tester

./tester

# Caveats
* Input file does accept non-number values. Charcters or strings are
  interpreted as 0.
* Input values are expected to be uint16_t, but if one enters
   65536 (2^16), this would roll over and be interpreted as 0
   65537 (2^16+1) is interpreted as 1
   65538 is interpreted as 2, so is invalid
