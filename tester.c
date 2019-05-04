#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const int ERROR = 256;
static const int PASS = 0;

static uint16_t passCount = 0;
static uint16_t failCount = 0;
static uint16_t testCount = 0;

void expect(int expected, int actual) {
	if (expected != actual) {
		printf("\nERROR, expected=%d, actual=%d\n", expected, actual);
		failCount++;
		exit(0);
	} else {
		passCount++;
	}
	testCount++;
}

int main(int argc, char ** argv) {
	// Input grid supplied in example
	expect(PASS, system("./game input 1"));
	// Grid mising one cell
	expect(ERROR, system("./game invalid_game_1 1"));
	// Grid has invalid cell value of 2
	expect(ERROR, system("./game invalid_game_2 1"));
	// Grid has invalid cell value of -1
	expect(ERROR, system("./game invalid_game_3 1"));
	// Grid of size 1, set to 1, dies after one round
	expect(PASS, system("./game tc_1 3"));
	// Grid of size 1, set to 0, never breeds
	expect(PASS, system("./game tc_1b 1"));
	// 2x2 with all 1's -- Cells perist forever
	expect(PASS, system("./game tc_2 1"));
	// 3x3 with all 1's -- All Cells die after two rounds
	expect(PASS, system("./game tc_3 3"));
	// 3x3 with that never dies
	expect(PASS, system("./game tc_3_neverDie 100"));
	// 3x3 with that never dies
	expect(PASS, system("./game tc_3_neverDie2 100"));	
	// 4x4 with all 1's -- All Cells die after two rounds
	expect(PASS, system("./game tc_4 3"));
	// 5x5 with that never dies
	expect(PASS, system("./game tc_5 100"));

	printf("\npassCount=%d", passCount);
	printf("\nfailCount=%d", failCount);
	printf("\npassCount=%d", testCount);
}