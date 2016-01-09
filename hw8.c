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
ui bingen(ui[]);
int sigbit(ui, ui);
IP* insert_node(IP*, ui, ui);
IP* LPM_search(IP*, ui);
void IPdecode(IP*, FILE*);

//global variables
unsigned long long int begin, end;

int main() {
	FILE *origin, *search, *insert, *delete, *result;
	ui pow_two = 1, ip[5], ipbin, bin_12, bin_8;
	int i, sig;
	char temp[19];
	IP *matched;

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

	while(fgets(temp, 19, origin) != NULL) { //create data structure
		sscanf(temp, "%u.%u.%u.%u/%u", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4]);
		ipbin = bingen(ip);
		sig = sigbit(ipbin, ip[4]);
		if(ip[4] >= 8 && ip[4] < 16)
			seg_table1[sig] = insert_node(seg_table1[sig], ipbin, ip[4]);
		else if(ip[4] >= 16 && ip[4] < 25)
			seg_table2[sig] = insert_node(seg_table2[sig], ipbin, ip[4]);
		else if(ip[4] >= 25 && ip[4] <= 32)
			seg_table3[sig] = insert_node(seg_table3[sig], ipbin, ip[4]);
		else if(ip[4] != 0 && ipbin != 0) {
			printf("Error in input data.");
			exit(1);
		}
	}

	while(fgets(temp, 16, search) != NULL) {
		sscanf(temp, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);
		ipbin = bingen(ip);
		bin_12 = sigbit(ipbin, 16);
		bin_8 = sigbit(ipbin, 0);

		matched = LPM_search(seg_table3[bin_12], ipbin);
		if(matched == NULL)
			matched = LPM_search(seg_table2[bin_12], ipbin);
		if(matched == NULL)
			matched = LPM_search(seg_table1[bin_8], ipbin);

		IPdecode(matched, result);
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

ui bingen(ui ip[]) {
	ui out = 0;
	out |= (ip[0] << 24);
	out |= (ip[1] << 16);
	out |= (ip[2] << 8);
	out |= ip[3];
	
	return out;
}

int sigbit(ui ipbin, ui sig) {
	if(sig >= 16)
		return (ipbin >> 20);
	else
		return (ipbin >> 24);
}

IP* insert_node(IP *head, ui add, ui len) {
	IP *temp = head, *newnode = malloc(sizeof(IP));
	newnode -> address = add;
	newnode -> length = len;
	newnode -> next = NULL;
	if(head == NULL) {
		return newnode;
	}
	while(temp -> next != NULL) {
		if((temp -> next) -> length < len) {
			newnode -> next = temp -> next;
			temp -> next = newnode;
			return head;
		}
		else {
			temp = temp -> next;
		}
	}
	temp -> next = newnode;
	return head;
}

IP* LPM_search(IP *head, ui tosearch) {
	ui old, new;
	IP *temp = head;
	while(temp != NULL) {
		old = (temp -> address) >> (32 - temp -> length);
		new = tosearch >> (32 - temp -> length);
		if(old == new) {
			break;
		}
		else {
			temp = temp -> next;
		}
	}

	return temp;
}

void IPdecode(IP *match, FILE *result) {
	if(match != NULL) {
		ui bin = match -> address;
		ui ip[4], length;
		ip[0] = bin >> 24;
		bin &= 0x00FFFFFF;
		ip[1] = bin >> 16;
		bin &= 0x0000FFFF;
		ip[2] = bin >> 8;
		bin &= 0x000000FF;
		ip[3] = bin;
		length = match -> length;
		fprintf(result, "%u.%u.%u.%u/%u\n", ip[0], ip[1], ip[2], ip[3], length);
	}
	else {
		fprintf(result, "%s", "0.0.0.0/0\n");
	}
	return;
}
