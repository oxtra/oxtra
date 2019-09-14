long _syscall(long number, long param1, long param2, long param3) {
	long ret;

	asm("mov rax, %1;"
	"mov rdi, %2;"
	"mov rdx, %4;"
	"mov rsi, %3;"
	"syscall;"
	"mov %0, rax;"
	: "=r" (ret)
	: "r" (number), "r" (param1), "r" (param2), "r" (param3)
	);

	return ret;
}

void _write (int fd, const void *buf, long count) {
	_syscall(1, fd, buf, count);
}

void print(const void *buf) {
	if (buf == 0) return;
	int length;
	for (length = 0; ((char*) buf)[length] != '\0'; length++);
	_write(1, buf, length);
}

long read(int fd, void *buf, long count) {
	return _syscall(0, fd, buf, count);
}

int digits(int number) {
    if (number == 0) return 1;

    int count = 0;
    while (number != 0) {
	count++;
	number /= 10;
    }

    return count;
}

int setNumber(char* text, int number) {
    int length = digits(number);

    for (int i = length - 1; i >= 0; i--) {
	text[i] = (number % 10) + '0';
	number /= 10;
    }

    text[length] = '\n';
    text[length + 1] = '\0';

    return length;
}

int isqrt(int number) {
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
    int limit = isqrt(number);
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
	int primeCount = 0;
    char buffer[32];

    while (currentPrime < 1000) {
		setNumber(buffer, currentPrime);
		print(buffer);
		currentPrime = getNextPrime(currentPrime);
		primeCount++;
    }

    return primeCount;
}

void _start() {
	_syscall(60, main(), 0, 0);
}
