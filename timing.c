#include <stdio.h>

int main() {
	FILE *search, *insert, *delete, *output;
	int temp = 0, i, j;
	search = fopen("search.csv", "r");
	insert = fopen("insert.csv", "r");
	delete = fopen("delete.csv", "r");
	output = fopen("out.csv", "w");
	int out[3][1000] = {{0}};
	while(fscanf(search, "%d", &temp) != EOF) {
		out[0][temp / 200]++;
	}
	while(fscanf(insert, "%d", &temp) != EOF) {
		out[1][temp / 200]++;
	}
	while(fscanf(delete, "%d", &temp) != EOF) {
		out[2][temp / 200]++;
	}
	for(i = 0;i < 1000;i++) {
		for(j = 0;j < 3;j++) {
			fprintf(output, "%d,", out[j][i]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
}
