#include <stdio.h>
#include <stdlib.h>

//prototype
inline unsigned long long int rdtsc();

//global variables
unsigned long long int begin, end;

int main() {
	int i;
	begin = rdtsc();
	for(i = 0;i < 1000;i++) {
		printf("%d\n", i);
	}
	end = rdtsc();
	printf("%lld", end - begin);
	return 0;
}

inline unsigned long long int rdtsc() {
	unsigned long long int x;
	asm volatile ("rdtsc" : "=A" (x));
	return x;
} 
