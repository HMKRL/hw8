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
ui bingen(ui[]); //convert IPv4 address into 32-bit unsigned int
ui sigbit(ui, ui); //return which element in array should the ip be stored
IP* insert_node(IP*, ui, ui); //insert a node
IP* LPM_search(IP*, ui); //search data, will return matched node address or NULL
void IPdecode(IP*, FILE*); //decode 32-bit unsigned int into IPv4 address
IP* delete_node(IP*, ui, ui); //delete node
void free_list(IP*); //free linked list after use
//void print_list(IP*, FILE*); //data structure checker

int main() {
	FILE *origin, *search, *insert, *delete, *result/*, *timing*/;
	ui pow_two = 1, ip[5], ipbin = 0, bin_12 = 0, bin_8 = 0, sig = 0;
	int i;
	char temp[21];
	IP *matched = NULL;

//open files
	origin = fopen("IPv4_400k.txt", "r");
	search = fopen("IPv4_search.txt", "r");
	insert = fopen("IPv4_insert.txt", "r");
	delete = fopen("IPv4_delete.txt", "r");
	result = fopen("result.txt", "w");
//	timing = fopen("timing.csv", "w");

//make segmentation tables
	IP *seg_table1[pow_two << 8];  //length 8-15
	IP *seg_table2[pow_two << 12]; //length 16-25
	IP *seg_table3[pow_two << 12]; //length 25-32
	
	for(i = 0;i < pow_two << 8;i++) {
		seg_table1[i] = NULL;
	}
	for(i = 0;i < pow_two << 12;i++) {
		seg_table2[i] = NULL;
		seg_table3[i] = NULL;
	}

	while(fgets(temp, 21, origin) != NULL) { //create data structure
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
/* used for check data structure
	for(i = 0; i < 1 << 8;i++) {
		print_list(seg_table1[i], timing);
		fprintf(timing, "==============================\n");
	}
	for(i = 0;i < 1 << 12;i++) {
		print_list(seg_table2[i], timing);
		fprintf(timing, "==============================\n");
		print_list(seg_table3[i], timing);
	}
*/

	while(fgets(temp, 21, search) != NULL) { //search
		sscanf(temp, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);
		ipbin = bingen(ip);
		bin_12 = sigbit(ipbin, 24);
		bin_8 = sigbit(ipbin, 8);

//serach from 25-32 lengthed group, if no match result, go search next
		matched = LPM_search(seg_table3[bin_12], ipbin);
		if(matched == NULL)
			matched = LPM_search(seg_table2[bin_12], ipbin);
		if(matched == NULL)
			matched = LPM_search(seg_table1[bin_8], ipbin);
		
		IPdecode(matched, result);
	}

	while(fgets(temp , 21, insert) != NULL) { //insert
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
		}
	}

	while(fgets(temp, 21, delete) != NULL) { //delete
		sscanf(temp, "%u.%u.%u.%u/%u", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4]);
		ipbin = bingen(ip);
		sig = sigbit(ipbin, ip[4]);
		if(ip[4] >= 8 && ip[4] < 16)
			seg_table1[sig] = delete_node(seg_table1[sig], ipbin, ip[4]);
		else if(ip[4] >= 16 && ip[4] < 25)
			seg_table2[sig] = delete_node(seg_table2[sig], ipbin, ip[4]);
		else if(ip[4] >= 25 && ip[4] <= 32)
			seg_table3[sig] = delete_node(seg_table3[sig], ipbin, ip[4]);
		else if(ip[4] != 0 && ipbin != 0) {
			printf("Error in input data.");
			exit(1);
		}
	}
	
//free total list
	for(i = 0;i < pow_two << 8;i++) {
		free_list(seg_table1[i]);
	}
	for(i = 0;i < pow_two << 12;i++) {
		free_list(seg_table2[i]);
		free_list(seg_table3[i]);
	}

	fclose(origin);
	fclose(insert);
	fclose(search);
	fclose(delete);
	fclose(result);
//	fclose(timing);
	return 0;
}

ui bingen(ui ip[]) {
	ui out = 0x00000000; 
	out |= (ip[0] << 24); 
	out |= (ip[1] << 16);
	out |= (ip[2] << 8);
	out |= ip[3];
	
	return out;
}

ui sigbit(ui ipbin, ui sig) {
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
	if(head -> length <= len) {
		newnode -> next = head;
		return newnode;
	}
	while(temp -> next != NULL) {
		if((temp -> next) -> length <= len) {
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
	else { //match == NULL means there are no matched prefix, use default
		fprintf(result, "%s", "0.0.0.0/0\n");
	}
	return;
}

IP* delete_node(IP *head, ui add, ui len) {
	IP *temp = head, *to_free;
	if(head == NULL)
		return head;
	if(head -> address == add && head -> length == len) {
		to_free = head;
		head = head -> next;
		free(to_free);
		return head;
	}
	while(temp -> next != NULL) {
		if((temp -> next) -> address == add && (temp -> next) -> length == len) {
			to_free = temp -> next;
			temp -> next = (temp -> next) -> next;
			free(to_free);
			return head;
		}
		else
			temp = temp -> next;
	}

	return head;
}

void free_list(IP *head) {
	IP *to_free = head;
	while(to_free != NULL) {
		head = head -> next;
		free(to_free);
		to_free = head;
	}
}
/* used for check data structure
void print_list(IP *head, FILE *timing) {
	IP *temp = head;
	while(temp != NULL) {
		IPdecode(temp, timing);
		temp = temp -> next;
	}
	return;
}
*/
