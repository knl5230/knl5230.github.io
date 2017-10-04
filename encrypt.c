#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#define KEYLENGTH 10 // Can be a ny t h i n g from 1 t o 10
int main() {
	unsigned char ch;
	FILE * fpIn, *fpOut;
	unsigned char key[KEYLENGTH] = { 0xaa, 0xab, 0xaa,0xab,0xaa,0xab,0xaa,0xab,0xaa,0xaa};
	
	fpIn = fopen("plaintext.txt","r");
	fpOut = fopen("hw1_input.txt","wb");
	for (int i = 0; fscanf(fpIn,"%c", &ch) != EOF; ++i) {
		ch ^= key[i % KEYLENGTH];
		fwrite(&ch, sizeof(ch), 1, fpOut);
	}
	fclose(fpIn);
	fclose(fpOut);
	return 0;
}