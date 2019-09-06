#include <stdio.h>

int sqrt(int number) {
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
	int limit = sqrt(number);
	for (check = 3; check <= limit; check += 2) {
		if (number % check == 0) return 0;
	}
	
	return 1;
}

int getNextPrime(int start) {
	start++;
	while (1) {
		if (isPrimeNumber(start)) return start;
		
		start++;
		
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
