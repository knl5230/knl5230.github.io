#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 5001			//파일 크기의 최대는 MAX(5000바이트)
#define Eng_I 0.0656010		//실제 영어 글자의 빈도수

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
};	//각 알파벳의 빈도수

int repeat[11][4] = { { 0 },{ 0 },{ 1,1 },{ 1,1 },{ 2,1,2 },{ 1,1 },{ 3,1,2,3 },{ 1,1 },{ 3,1,2,4 },{ 2,1,3 },{ 3,1,2,5 } };
//반복되는 답을 제거하기 위한 배열
//n번째 배열의 첫칸에는 n의 약수의 개수 m이, 그 뒤로 m칸에는 1을 포함하고 자신을 포함하지 않는 약수가 들어있다.

void clear(int* prop)	//빈도수를 저장하는 prop배열의 초기화
{
	int i;
	for (i = 0; i < 256; i++)
		prop[i] = 0;
}

double calc_prop(int* prop, int total, int len)	//prop 배열에 저장된 빈도수의 제곱을 계산
{
	int i;
	double result = 0.0;

	for (i = 0; i < len; i++)
		result += ((double)prop[i] / (double)total)*((double)prop[i] / (double)total);

	return result;
}

int check(int* prop, int count, unsigned char now_key)	//현재 키값(now_key)값이 타당한지 확인하는 함수
{
	int i;
	int total = 0;
	double res = 0.0;

	for (i = 97; i < 123; i++)	 //알파벳 소문자 개수를 센다
		total += prop[i];

	if (total == 0)	//알파벳 소문자 범위 내의 문자가 하나도 없으면
		return 0;

	//원래 알파벳의 빈도수와 key값으로 decrypt해서 얻은 빈도수를 곱한다
	for (i = 97; i < 123; i++)
		res += ((double)prop[i] / (double)total)*original[i - 'a'];



	if ((double)count*0.5>(double)total)
		return 0;


	//위에서 구한 값과 실제 알파벳 빈도수제곱이 거의 비슷하면 그것이 key의 값이다
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
	int Tcase;	//키의 길이
	int i, j, k, candi, count;
	int input_len;	//파일의 길이
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

	//파일의 길이 측정 및 파일의 값을 읽어와 input에 저장
	fseek(fIn, 0L, SEEK_END);
	input_len = ftell(fIn);
	fseek(fIn, 0L, SEEK_SET);
	fread(input, sizeof(unsigned char), input_len, fIn);



	//key의 length를 결정
	for (Tcase = 1; Tcase <= 10; Tcase++)
	{
		k = 0;
		min = 0.0;
		for (k = 0; k < Tcase; k++)	//key의 길이를 구하기 위해 k부터 시작해서 key의 길이 번째 자리마다 글자를 수집
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

			tmp = calc_prop(prop, count, 256);	//수집한 글자들의 빈도수 제곱을 구한다

			min += tmp;
		}

		min = min / Tcase;	//글자 빈도수 제곱의 합의 평균을 구한다

		if (max < min)
		{
			max = min;
			max_index = Tcase;
		}
	}
	Tcase = max_index;	//글자들의 빈도수 제곱의 평균이 가장 큰 것을 key의 길이로 잡는다
	key = (unsigned char*)malloc(sizeof(unsigned char)*Tcase);	//key값을 기록할 배열할당

	for (i = 0; i < Tcase; i++)	//key의 i번째 자리를 구함
	{
		max = -1.0;
		min_res = 1.0;
		min_ind = 0;
		for (candi = 0; candi < 256; candi++)	//key값의 후보들. 0~255까지 총 256가지
		{
			key[i] = candi;
			j = i;
			clear(prop);
			count = 0;
			flag = 0;
			while (j < input_len)
			{
				tmp2 = (key[i] ^ input[j]);

				if ((tmp2 < 0) || (tmp2 > 127))	//input의 글자와 key값의 후보를 XOR 연산으로 계산하여 만약 아스키코드 범위를 벗어나면 후보에서 제외
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
			else	//현재의 key값이 타당한지 check함수를 통해 확인
				check(prop, count, key[i]);
		}
		key[i] = min_ind;
	}

	//key의 길이가 n일경우 n의 배수이면서 같은 패턴을 반복하는 경우가 구해질 수 있음
	//그럴경우 key의 값을 반복하는 패턴이 아닌 최소 단위로 변경해준다.
	for (i = 1; i <= repeat[Tcase][0]; i++)		//현재 key 길이의 약수의 개수 만큼 실행
	{
		flag = 0;
		for (j = 0; j + repeat[Tcase][i] <Tcase; j++)	//만약 모든 key[i]가 key[i+key길이의 약수]와 같으면 반복되는 패턴
			if (key[j] != key[j + repeat[Tcase][i]])
			{
				flag = 1;
				break;
			}
		if (flag == 1)
			continue;
		else
			Tcase = repeat[Tcase][i];	//반복되는 패턴이라면 key의 길이를 줄인다.
	}

	for (i = 0; i < Tcase; i++)	//key값 출력
		fprintf(fOut, "0x%02x ", key[i]);
	fprintf(fOut, "\n");

	for (i = 0; i<input_len; ++i)	//암호화 해제
	{
		input[i] ^= key[i % Tcase];
		ch = input[i];
		fwrite(&ch, sizeof(input[i]), 1, fOut);
	}

	free(key);

	fclose(fIn);
	fclose(fOut);


}