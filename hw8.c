#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct ip {
	unsigned int address;
	unsigned int length;
	struct ip *next;
};

typedef struct ip IP;
typedef unsigned int ui;

//prototype
inline unsigned long long int rdtsc();

//global variables
unsigned long long int begin, end;
ui bingen(ui[]);
int sigbit(ui, ui);

int main() {
	FILE *origin, *search, *insert, *delete, *result;
	ui pow_two = 1, ip[5], ipbin;
	int i;
	char temp[19];

//open files
	origin = fopen("IPv4_400k.txt", "r");
	search = fopen("IPv4_search.txt", "r");
	insert = fopen("IPv4_insert.txt", "r");
	delete = fopen("IPv4_delete.txt", "r");
	result = fopen("result.txt", "w");

//make segmentation tables
	IP *seg_table1[pow_two << 8];
	IP *seg_table2[pow_two << 12];
	IP *seg_table3[pow_two << 12];
	
	for(i = 0;i < pow_two << 8;i++) {
		seg_table1[i] = NULL;
	}
	for(i = 0;i < pow_two << 12;i++) {
		seg_table2[i] = NULL;
		seg_table3[i] = NULL;
	}

	while(fgets(temp, 19, origin) != NULL) {
		sscanf(temp, "%u.%u.%u.%u/%u", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4]);
		ipbin = bingen(ip);
		printf("%u.%u.%u.%u/%u  %u\n",ip[0], ip[1], ip[2], ip[3], ip[4], ipbin);
	}

	fclose(origin);
	fclose(insert);
	fclose(search);
	fclose(delete);
	fclose(result);
	return 0;
}
	
inline unsigned long long int rdtsc() {
	unsigned long long int x;
	asm volatile ("rdtsc" : "=A" (x));
	return x;
}

ui bingen(int ip[]) {
	ui out = 0;
	out |= (ip[0] << 24);
	out |= (ip[1] << 16);
	out |= (ip[2] << 8);
	out |= ip[3];
	
	return out;
}
