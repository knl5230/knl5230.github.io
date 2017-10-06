#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 5001			//���� ũ���� �ִ�� MAX(5000����Ʈ)
#define Eng_I 0.0656010		//���� ���� ������ �󵵼�

double min_res;
unsigned char min_ind;
double original[26] = {
	0.08167,
	0.01492,
	0.02782,
	0.04253,
	0.12702,
	0.02228,
	0.02015,
	0.06094,
	0.06966,
	0.00153,
	0.00772,
	0.04025,
	0.02406,
	0.06749,
	0.07507,
	0.01929,
	0.00950,
	0.05987,
	0.06327,
	0.09056,
	0.02758,
	0.00978,
	0.02360,
	0.00160,
	0.01974,
	0.00074
};	//�� ���ĺ��� �󵵼�

int repeat[11][4] = { { 0 },{ 0 },{ 1,1 },{ 1,1 },{ 2,1,2 },{ 1,1 },{ 3,1,2,3 },{ 1,1 },{ 3,1,2,4 },{ 2,1,3 },{ 3,1,2,5 } };
//�ݺ��Ǵ� ���� �����ϱ� ���� �迭
//n��° �迭�� ùĭ���� n�� ����� ���� m��, �� �ڷ� mĭ���� 1�� �����ϰ� �ڽ��� �������� �ʴ� ����� ����ִ�.

void clear(int* prop)	//�󵵼��� �����ϴ� prop�迭�� �ʱ�ȭ
{
	int i;
	for (i = 0; i < 256; i++)
		prop[i] = 0;
}

double calc_prop(int* prop, int total, int len)	//prop �迭�� ����� �󵵼��� ������ ���
{
	int i;
	double result = 0.0;

	for (i = 0; i < len; i++)
		result += ((double)prop[i] / (double)total)*((double)prop[i] / (double)total);

	return result;
}

int check(int* prop, int count, unsigned char now_key)	//���� Ű��(now_key)���� Ÿ������ Ȯ���ϴ� �Լ�
{
	int i;
	int total = 0;
	double res = 0.0;

	for (i = 97; i < 123; i++)	 //���ĺ� �ҹ��� ������ ����
		total += prop[i];

	if (total == 0)	//���ĺ� �ҹ��� ���� ���� ���ڰ� �ϳ��� ������
		return 0;

	//���� ���ĺ��� �󵵼��� key������ decrypt�ؼ� ���� �󵵼��� ���Ѵ�
	for (i = 97; i < 123; i++)
		res += ((double)prop[i] / (double)total)*original[i - 'a'];



	if ((double)count*0.5>(double)total)
		return 0;


	//������ ���� ���� ���� ���ĺ� �󵵼������� ���� ����ϸ� �װ��� key�� ���̴�
	if (res > 0.06 && res < 0.8)
	{
		if (res < Eng_I)
			res = Eng_I - res;
		else
			res = res - Eng_I;
		if (res < 0.0)
			res *= -1.0;
		if (res <= min_res)
		{
			min_res = res;
			min_ind = now_key;
		}
		return 1;
	}
	else
		return 0;

}

int main()
{
	int Tcase;	//Ű�� ����
	int i, j, k, candi, count;
	int input_len;	//������ ����
	int flag = 0;

	double max = -1.0;
	double tmp;
	double min = 1.0;
	int max_index;

	int prop[256] = { 0 };

	FILE* fIn;
	FILE* fOut;

	unsigned char ch;
	unsigned char tmp2;

	unsigned char* key;
	unsigned char input[MAX] = { 0 };

	fIn = fopen("hw1_input.txt", "r");
	fOut = fopen("hw1_output.txt", "wb");

	//������ ���� ���� �� ������ ���� �о�� input�� ����
	fseek(fIn, 0L, SEEK_END);
	input_len = ftell(fIn);
	fseek(fIn, 0L, SEEK_SET);
	fread(input, sizeof(unsigned char), input_len, fIn);



	//key�� length�� ����
	for (Tcase = 1; Tcase <= 10; Tcase++)
	{
		k = 0;
		min = 0.0;
		for (k = 0; k < Tcase; k++)	//key�� ���̸� ���ϱ� ���� k���� �����ؼ� key�� ���� ��° �ڸ����� ���ڸ� ����
		{
			i = 0;
			j = 0;
			clear(prop);
			count = 0;

			while (i + k < input_len)
			{
				prop[(int)input[i + k]]++;
				i += Tcase;
				count++;
				j++;
			}

			tmp = calc_prop(prop, count, 256);	//������ ���ڵ��� �󵵼� ������ ���Ѵ�

			min += tmp;
		}

		min = min / Tcase;	//���� �󵵼� ������ ���� ����� ���Ѵ�

		if (max < min)
		{
			max = min;
			max_index = Tcase;
		}
	}
	Tcase = max_index;	//���ڵ��� �󵵼� ������ ����� ���� ū ���� key�� ���̷� ��´�
	key = (unsigned char*)malloc(sizeof(unsigned char)*Tcase);	//key���� ����� �迭�Ҵ�

	for (i = 0; i < Tcase; i++)	//key�� i��° �ڸ��� ����
	{
		max = -1.0;
		min_res = 1.0;
		min_ind = 0;
		for (candi = 0; candi < 256; candi++)	//key���� �ĺ���. 0~255���� �� 256����
		{
			key[i] = candi;
			j = i;
			clear(prop);
			count = 0;
			flag = 0;
			while (j < input_len)
			{
				tmp2 = (key[i] ^ input[j]);

				if ((tmp2 < 0) || (tmp2 > 127))	//input�� ���ڿ� key���� �ĺ��� XOR �������� ����Ͽ� ���� �ƽ�Ű�ڵ� ������ ����� �ĺ����� ����
				{
					flag++;
					break;
				}
				else
				{
					prop[(int)tmp2]++;
					count++;
				}
				j += Tcase;
			}
			if (flag)
				continue;
			else	//������ key���� Ÿ������ check�Լ��� ���� Ȯ��
				check(prop, count, key[i]);
		}
		key[i] = min_ind;
	}

	//key�� ���̰� n�ϰ�� n�� ����̸鼭 ���� ������ �ݺ��ϴ� ��찡 ������ �� ����
	//�׷���� key�� ���� �ݺ��ϴ� ������ �ƴ� �ּ� ������ �������ش�.
	for (i = 1; i <= repeat[Tcase][0]; i++)		//���� key ������ ����� ���� ��ŭ ����
	{
		flag = 0;
		for (j = 0; j + repeat[Tcase][i] <Tcase; j++)	//���� ��� key[i]�� key[i+key������ ���]�� ������ �ݺ��Ǵ� ����
			if (key[j] != key[j + repeat[Tcase][i]])
			{
				flag = 1;
				break;
			}
		if (flag == 1)
			continue;
		else
			Tcase = repeat[Tcase][i];	//�ݺ��Ǵ� �����̶�� key�� ���̸� ���δ�.
	}

	for (i = 0; i < Tcase; i++)	//key�� ���
		fprintf(fOut, "0x%02x ", key[i]);
	fprintf(fOut, "\n");

	for (i = 0; i<input_len; ++i)	//��ȣȭ ����
	{
		input[i] ^= key[i % Tcase];
		ch = input[i];
		fwrite(&ch, sizeof(input[i]), 1, fOut);
	}

	free(key);

	fclose(fIn);
	fclose(fOut);


}