// DISCLAIMER: This program is not meant to be used in any other scenario than testing many different conditional jumps

#include <stdio.h>

int sqrti(int number) {
	if (number == 0) return 0;
	if (number == 1) return 1;

	int current = 2;
	while (current * current <= number) current++;

	return current - 1;
}

int isPrimeNumber(int number) {
	if (number < 2) return 0;
	if (number == 2) return 1;
	if (number % 2 == 0) return 0;

	int check;
	int limit = sqrti(number);
	for (check = 3; check <= limit; check += 2) {
		if (number % check == 0) return 0;
	}

	return 1;
}

int getNextPrime(int start) {
	if (start < 2) return 2;
	if (start == 2) return 3;

	start += 2;
	while (1) {
		if (isPrimeNumber(start)) return start;

		start += 2;

		// detect an int overflow
		if (start < 0) return -1;
	}
}

int main() {
	int currentPrime = 2;

	while (currentPrime < 1000000) {
		printf("%d\n", currentPrime);
		currentPrime = getNextPrime(currentPrime);
	}

	return currentPrime;
}
