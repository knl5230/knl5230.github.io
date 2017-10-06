#define _CRT_SECURE_NO_WARNINGS
#define aes_enc 1	//AES_encrypt����� ��� 1
#define aes_dec 0	//AES_decrypt ����� ��� 0
#define base64_enc 1	//base64 encoding�� ��� 1
#define base64_dec 0	//base64 decoding�� ��� 0


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/aes.h>
#include<openssl/des.h>

#define ALL_PW 184390

unsigned char base64[64];

//passwords.txt���� key�̸�(name)�� password��(key)�� ��� ���� ����ü
struct PWKEY	
{
	unsigned char key[16];
	unsigned char name[20];
};

//ciphertext(midstr)�� � key(key_name)�� ����� ����ϱ� ���� ����ü
struct middlestring	
{
	unsigned char midstr[17];
	unsigned char key_name[20];
};

//strcmp�� �� ��� '\0'�κб����� ���ϹǷ� ������ 16�ڸ��� ���ϱ� ���� �Լ�
//�� ���ڿ��� �� ũ�� 1, �� ������ -1, ������ 0�� ��ȯ�Ѵ�.
int new_strcpr(unsigned char* s1, unsigned char* s2)	
{
	int i;
	for (i = 0; i < 16; i++)
	{
		if (s1[i] > s2[i])
			return 1;
		else if(s1[i] < s2[i])
			return -1;
	}
	return 0;
}

//stdlib�� �ִ� qsort�Լ��� ����ϱ� ���� compare�Լ�
//�� ���� ���ڿ��� �տ� ������ �������� ����
int compare_func(const void* v1, const void* v2)
{
	return (new_strcpr(((struct middlestring*)v1)->midstr, ((struct middlestring*)v2)->midstr));
}

//base[64]�� ġȯ�Ǵ� ���ڸ� �ֱ� ���� �Լ�
void set_base64()
{
	int i;

	for (i = 0; i < 26; i++)
		base64[i] = 'A' + i;
	for (i = 0; i < 26; i++)
		base64[26 + i] = 'a' + i;
	for (i = 0; i < 10; i++)
		base64[52 + i] = '0' + i;
	base64[62] = '+';
	base64[63] = '/';
}

//base64�� �̿��Ͽ� encoding (enc_dec == 1) �Ǵ� decoding (enc_dec == 0)�� ���� �Լ�
void change_base64(unsigned char* input, unsigned char* output, unsigned int enc_len, unsigned int enc_dec)
{
	int i;
	int carry;
	int idx;
	unsigned char tmp;
	unsigned char j;


	idx = 0;
	carry = 0;
	if (enc_dec)	//encoding�� ��� 6��Ʈ�� ��� base64�迭�� �̿��Ͽ� ġȯ
	{
		for (i = 0; i < enc_len; i++)
		{
			if (i % 3 == 0)
			{
				output[idx] = input[i] / 4;
				output[idx] = base64[output[idx]];
				idx++;
				output[idx] = ((input[i] % 4) << 4);
			}
			if (i % 3 == 1)
			{
				output[idx] += input[i] / 16;
				output[idx] = base64[output[idx]];
				idx++;
				output[idx] += (input[i] % 16) * 4;
			}
			if (i % 3 == 2)
			{
				output[idx] += input[i] / 64;
				output[idx] = base64[output[idx]];
				idx++;
				output[idx] += (input[i] % 64);
				output[idx] = base64[output[idx]];
				idx++;
			}
		}
	}
	else	//decoding�� ��� 6��Ʈ�� �������� 8��Ʈ�� ��� base[64]�� �̿��Ͽ� ġȯ
	{
		for (i = 0; i < strlen(input); i++)
		{
			for (j = 0; j < 64; j++)
				if (base64[j] == input[i])
					tmp = j;
			
			if (i % 4 == 0)
			{
				output[idx] = tmp * 4;
			}
			if (i % 4 == 1)
			{
				output[idx] += tmp / 16;
				idx++;
				output[idx] += (tmp % 16) * 16;
			}
			if (i % 4 == 2)
			{
				output[idx] += (tmp / 4);
				idx++;
				output[idx] += (tmp % 4) * 64;
			}
			if (i % 4 == 3)
			{
				output[idx] += tmp;
				idx++;
			}
		}
	}
}

//FILE* table���� password(key)�� key(name)�� �о���� �Լ�
void get_password(unsigned char* key, unsigned char* name, FILE* table)
{
	int i;
	for (i = 0; i < 16; i++)
		fscanf(table,"%2x", (unsigned int*)&key[i]);
	fscanf(table, "%s", name);
	return;
}

//FILE* txt���� plaintext(plain)�� ciphertext(cipher)�� �о���� �Լ�
void get_text(unsigned char* plain, unsigned char* cipher, FILE* txt)
{
	int len;
	fgets(plain, 140000, txt);
	len = strlen(plain);
	plain[len - 1] = '\0';
	fgets(cipher, 140000, txt);
	return;
}

//DES�� �����ϴ� �Լ�
int do_des(unsigned char* plaintxt, unsigned char* ciphertxt, unsigned int msg_len, unsigned char* key)
{
	int i;

	int check;

	unsigned int enc_len;

	DES_key_schedule des;

	//msg_len('\0'����)�� ���̿� ���� enc_len�� ����
	//�ٸ� msg_len���� 16�ۿ� �Ѱ����� �ʱ� ������
	//���� ������ ��Ȳ�� ���� ���ܵ� �ڵ�
	if (msg_len % 16)
		enc_len = (msg_len / 16 + 1) * 16;
	else
		enc_len = msg_len;

	check = DES_set_key((C_Block*)key, &des);

	if (check < 0)
	{
		printf("Error Occured during DES\n");
		return -1;
	}

	memset(ciphertxt, 0, 17);
	
	//ciphertext�� key������ des encrypt
	for (i = 0; i < enc_len; i += 8)
		DES_ecb_encrypt((C_Block*)(plaintxt + i), (C_Block*)(ciphertxt + i), &des, DES_ENCRYPT);

	return enc_len;
}

//AES�� �����ϴ� �Լ�
void do_aes(unsigned char* plaintxt, unsigned char* ciphertxt, unsigned char* key, unsigned int enc_len, unsigned int enc_dec)
{
	int i;

	int check;

	unsigned char iv[16];

	AES_KEY aes;

	if (enc_dec == 1)
		check = AES_set_encrypt_key(key, 128, &aes);
	else
		check = AES_set_decrypt_key(key, 128, &aes);

	if (check < 0)
	{
		printf("Error Occured during AES\n");
		return;
	}

	memset(iv, 0, 16);
	memset(ciphertxt, 0, enc_len);
	
	//plaintxt�� key������ encrypt �Ǵ� decrypt
	AES_cbc_encrypt(plaintxt, ciphertxt, enc_len, &aes, iv, enc_dec);
}

int main()
{
	FILE* hashtable;
	FILE* plcitxt;
	FILE* result;

	struct PWKEY enc_key;
	struct middlestring* middes;
	struct middlestring* midaes;

	unsigned int enc_len;
	unsigned char* plntxt;
	unsigned char* outbase;
	unsigned char* cphtxt;

	int i, j, k;
	int index1;
	int index2;
	int check;

	AES_KEY aes;

	middes = (struct middlestring*)malloc(sizeof(struct middlestring)*ALL_PW);	//des������� ����
	midaes = (struct middlestring*)malloc(sizeof(struct middlestring)*ALL_PW);	//aes������� ����

	plntxt = (char*)malloc(sizeof(char) * 100100);	//plaintext ����
	outbase = (char*)malloc(sizeof(char) * 100100);	//base64�� decoding�� ��������
	cphtxt = (char*)malloc(sizeof(char) * 140000);	//ciphertext ����

	hashtable = fopen("passwords.txt", "r");
	plcitxt = fopen("PlaintextCiphertext.txt", "r");
	result = fopen("keys.txt", "wb");

	//plaintext,ciphertext�� �о��
	get_text(plntxt, cphtxt, plcitxt);	

	//ciphertext�� base64�� decoding
	memset(outbase, 0, 100100);
	set_base64();
	change_base64(cphtxt, outbase, 100100, base64_dec);

	//��� key������ plaintext�� �� 16����Ʈ�� ���Ͽ� DES encrypt�� ����
	for (i = 0; i < ALL_PW; i++)
	{
		memset(middes[i].midstr, 0, 17);
		memset(enc_key.name, 0, 20);
		get_password(enc_key.key, enc_key.name, hashtable);
		strcpy(middes[i].key_name, enc_key.name);
		do_des(plntxt, middes[i].midstr, 16, enc_key.key);
	}
	//����
	qsort(middes, ALL_PW, sizeof(struct middlestring), compare_func);

	fseek(hashtable, 0L, SEEK_SET);
	
	//enc_len�� ����
	enc_len = strlen(plntxt) + 1;
	if (enc_len % 16)
		enc_len = (enc_len / 16 + 1) * 16;

	//��� key������ outbase�� ���Ͽ� AES Decrypt�� ������ �� �� 16����Ʈ�� ����
	for (i = 0; i < ALL_PW; i++)
	{
		memset(midaes[i].midstr, 0, 17);
		get_password(enc_key.key, enc_key.name, hashtable);
		strcpy(midaes[i].key_name, enc_key.name);
		do_aes(outbase, cphtxt, enc_key.key, enc_len, aes_dec);
		for (j = 0; j < 16; j++)
			midaes[i].midstr[j] = cphtxt[j];
	}
	//����
	qsort(midaes, ALL_PW, sizeof(struct middlestring), compare_func);


	index1 = 0;
	index2 = 0;
	check = 0;
	
	//DES encrypt ��� ���� AES decrypt ��� �� �� ������ ã�´�
	while (index1 < ALL_PW && index2 < ALL_PW)
	{
		check = new_strcpr(middes[index1].midstr, midaes[index2].midstr);
		if (check == 0)
		{
			//ã���� keys.txt�� key1�� key2�� ���
			fprintf(result, "%s\n%s", middes[index1].key_name, midaes[index2].key_name);
			break;
		}
		else if (check > 0)
		{
			index2++;
		}
		else if (check < 0)
		{
			index1++;
		}
	}
	
	//���� ��Ʈ�� �ݱ�
	fclose(hashtable);
	fclose(plcitxt);
	fclose(result);

	//�����Ҵ� ����
	free(middes);
	free(midaes);

	free(plntxt);
	free(cphtxt);
}









