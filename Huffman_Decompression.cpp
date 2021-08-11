#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <list>

using namespace std;
typedef unsigned char BYTE;

bool Bitstream_Decoder(string file)
{
	FILE* filename = fopen(file.c_str(), "rb");
	ofstream output;
	output.open("output.htd");
	unsigned char bitstream = 0;
	int num = 0, MSB = 0;;
	
	fread(&num, sizeof(BYTE), 1, filename);
	fread(&MSB, sizeof(BYTE), 1, filename);

	vector<pair<BYTE, BYTE>> code;
	BYTE temp_symbol,temp_len;

	//symbol�� huffman�ڵ��� ���� ��������
	for (int i = 0; i < num; i++)
	{
		fread(&temp_symbol, sizeof(BYTE), 1, filename);
		fread(&temp_len, sizeof(BYTE), 1, filename);
		code.push_back(make_pair(temp_symbol, temp_len));
	}



	//huffman code�� ��������
	int n = 0;
	BYTE temp = 0;
	BYTE check = 128;
	int huff_code = 0;
	vector<int> huff_code_vec;




	fread(&bitstream, sizeof(BYTE), 1, filename);
	//������� ��������
	for (int i = 0; i < code.size(); i++) {
		for (int j = 0; j < code[i].second; j++)
		{

			if (n == 8)
			{
				fread(&bitstream, sizeof(BYTE), 1, filename);
				n = 0;
			}
			temp = (bitstream & check ? 1 : 0);
			bitstream <<= 1;
			if (temp == 1)
			{
				huff_code <<= 1;
				huff_code = huff_code | 1;
			}
			else
			{
				huff_code <<= 1;
				huff_code = huff_code | 0;
			}
			n++;

		}
		
		huff_code_vec.push_back(huff_code);
		huff_code = 0;
	}



	//����Ǯ��
	int len = 0;
	unsigned char ch = 0;
	while (feof(filename) == 0)
	{
		
		

		if (n == 8)
		{

			fread(&bitstream, sizeof(BYTE), 1, filename);
			n = 0;
			fread(&ch, sizeof(BYTE), 1, filename);
			
			if (feof(filename) != 0)//���� ������ ������ �� �о��ٸ�
			{
				for (int i = 0; i < 8 - MSB; i++)
				{
					bitstream <<= 1;
				}

				

				for (int j = 0; j < MSB; j++)
				{
					temp = (bitstream & check ? 1 : 0);
					bitstream <<= 1;
					if (temp == 1)
					{
						huff_code <<= 1;
						huff_code = huff_code | 1;
					}
					else
					{
						huff_code <<= 1;
						huff_code = huff_code | 0;
					}
					len++;

					for (int i = 0; i < code.size(); i++) //symbol ã��
					{

						if (code[i].second == len)//���� üũ
						{
							if (huff_code_vec[i] == huff_code)
							{
								output << code[i].first;
								len = 0; //huffman code len ����
								huff_code = 0;
								break;
							}
						}
					}
				}

				return 0;
			}
			fseek(filename, -1, SEEK_CUR);
			
		
			
		}
	
		temp = (bitstream & check ? 1 : 0);
		bitstream <<= 1;

		if (temp == 1)
		{
			huff_code <<= 1;
			huff_code = huff_code | 1;
		}
		else
		{
			huff_code <<= 1;
			huff_code = huff_code | 0;
		}
		len++;

		for (int i = 0; i < code.size(); i++) //symbol ã��
		{
			
			if (code[i].second == len)//���� üũ
			{
				if (huff_code_vec[i] == huff_code)//�ڵ� üũ
				{
					output << code[i].first;
					len = 0; //huffman code len ���� �ʱ�ȭ
					huff_code = 0;
					break;
				}
			}
		}
		n++;

		

	}
		


	return 0;
	

	

}

int main()
{
	string input;
	cout << "���������� htb���ϸ��� �Է����ּ���" << '\n';
	cin >> input;
	input = input + ".htb";
	Bitstream_Decoder(input);

	
	return 0;

}