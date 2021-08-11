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
bool ERROR = false;
class tree
{
public:
	tree() {
		ascii_code = NULL;  freq = 0;
		leftnode = NULL;
		rightnode = NULL;
	}
	tree(unsigned char name1_, int freq_)
	{
		ascii_code = name1_;  freq = freq_;
		leftnode = NULL;
		rightnode = NULL;
	}
	//데이터

	unsigned char ascii_code;
	int freq;	//빈도수

	//자식노드

	tree* leftnode;
	tree* rightnode;
};



class code
{
public:

	//data
	unsigned char ascii_code;
	string huffcode;	//허프만부호화코드가 담길곳
};



tree* HuffmanTree_Insert(tree* u, tree* v) {
	tree* pt;
	pt = new tree;	//부모 노드 설정
	pt->ascii_code = NULL;
	pt->freq = u->freq + v->freq;	//빈도수를 합한다.
	pt->leftnode = u;			//좌, 우 노드 설정
	pt->rightnode = v;
	return pt;		//부모노드 반환
}

struct Mycomp_tree {
	bool operator()(const tree* l, const tree* r) {
		return l->freq > r->freq;	//오름차순
	}
};

struct Mycomp_code {
	bool operator()(const code* l, const code* r) {
		return l->huffcode.length() > r->huffcode.length();	//오름차순
	}
};

bool inorder(const tree* node, priority_queue<code*, vector<code*>, Mycomp_code>& huffcode, string cur_code)
{

	if (node->leftnode != NULL)
	{
		inorder(node->leftnode, huffcode, cur_code + "0");
	}

	if (node->leftnode == NULL && node->rightnode == NULL)
	{
		code* item = new code;
		item->ascii_code = node->ascii_code;
		item->huffcode = cur_code;
		huffcode.push(item);
	}
	if (node->rightnode != NULL)
	{
		inorder(node->rightnode, huffcode, cur_code + "1");
	}
	return true;
}

bool huffman_encode(string file)
{
	int freq_ascii[128] = { 0 };

	int encoded_num = 0;

	priority_queue<tree*, vector<tree*>, Mycomp_tree>  pq;
	priority_queue<code*, vector<code*>, Mycomp_code>  pq_huff;
	priority_queue<code*, vector<code*>, Mycomp_code>  pq_huff_encoding;
	priority_queue<code*, vector<code*>, Mycomp_code>  pq_huff_hft;

	//빈도수 계산

	FILE* filename = fopen(file.c_str(), "rt");
	if (filename == NULL)
	{
		cout << "File open fail" << "\n";
		return ERROR;
	}
	unsigned char index = 0;

	while (fscanf(filename, "%c", &index) != EOF)
	{
		++freq_ascii[index];
	}

	fclose(filename);
	////노드 만들기


	for (unsigned char i = 0; i < 128; i++)
	{
		if (freq_ascii[i] != 0) {
			encoded_num++;

			tree* item = new tree;
			tree* ptr;
			item->ascii_code = i;
			item->freq = freq_ascii[i];
			item->rightnode = NULL;
			item->leftnode = NULL;
			ptr = item;
			pq.push(ptr);
		}
	}


	////트리만들기

	tree* root;

	while (1)
	{
		tree* temp;
		if (pq.size() == 1)
		{
			temp = pq.top();
			pq.pop();
			root = temp;
			break;
		}

		temp = pq.top();
		pq.pop();
		tree* u, * v, * p;
		u = new tree;
		u->ascii_code = temp->ascii_code;
		u->freq = temp->freq;
		u->leftnode = temp->leftnode;
		u->rightnode = temp->rightnode;

		temp = pq.top();
		pq.pop();
		v = new tree;
		v->ascii_code = temp->ascii_code;
		v->freq = temp->freq;
		v->leftnode = temp->leftnode;
		v->rightnode = temp->rightnode;

		p = HuffmanTree_Insert(u, v);
		pq.push(p);

	}
	//허프만 코드 만들기

	inorder(root, pq_huff, "");
	pq_huff_encoding = pq_huff; //인코딩을 위한 카피
	pq_huff_hft = pq_huff; //hft를 위한 카피



	//압축시작 
	ofstream write_hft("output.hft");
	FILE* write_hfb = fopen("output.htb", "wb");

	int n = 0;
	unsigned char ascii_code;
	unsigned char ascii_len;
	int ascii_huff_code;
	unsigned char bitstream = 0;

	//인코딩을 위한 벡터
	vector<unsigned char> encoding_ascii_code;
	vector<int> encoding_ascii_len;
	vector<int> encoding_huff_code_int;


	//hft 파일
	write_hft << "인코딩된 문자의 갯수 = " << encoded_num << endl;//인코딩된 문자 입력
	for (int i = 0; i < encoded_num; i++)
	{


		ascii_code = pq_huff_hft.top()->ascii_code; //입력문자
		ascii_len = pq_huff_hft.top()->huffcode.length();//허프만코드 길이

		write_hft << ascii_code << " => " << "허프만 코드 : " << pq_huff_hft.top()->huffcode << " , 길이 = " << (int)ascii_len << endl;
		pq_huff_hft.pop();
	}
	write_hft << "바이너리로 변환된 텍스트파일 : " ;


	//헤더만들기
	fwrite(&encoded_num, sizeof(unsigned char), 1, write_hfb); //인코딩된 갯수
	fwrite(&encoded_num, sizeof(unsigned char), 1, write_hfb); //마지막 MSB를 위한 임시 저장
	for (int i = 0; i < encoded_num; i++)
	{
		ascii_code = pq_huff.top()->ascii_code; //헤더에 입력문자
		ascii_len = pq_huff.top()->huffcode.length();//헤더에 허프만코드 길이

		//헤더에 입력문자와 길이 입력
		fwrite(&ascii_code, sizeof(unsigned char), 1, write_hfb);
		fwrite(&ascii_len, sizeof(unsigned char), 1, write_hfb);
		pq_huff.pop();
	}

	int huff_code_int = 0;

	//비트스트림
	while (!pq_huff_encoding.empty())
	{
		ascii_code = pq_huff_encoding.top()->ascii_code; //데이터 입력문자
		ascii_len = pq_huff_encoding.top()->huffcode.length();//데이터 허프만코드 길이
		string huff_code = pq_huff_encoding.top()->huffcode;//허프만 코드 string

		huff_code_int = 0;
		for (int k = 0; k < ascii_len; k++)//stirng huffman 을 int huffman로 변환
		{

			if (49 == huff_code[k])
			{
				huff_code_int <<= 1;
				huff_code_int += 1;
			}
			else
			{
				huff_code_int <<= 1;
			}
		}

		//압축을 위한 정보 벡터 입력
		encoding_ascii_code.push_back(ascii_code);
		encoding_ascii_len.push_back((int)ascii_len);
		encoding_huff_code_int.push_back(huff_code_int);


		//비트스트림을 이용한 압축
		for (int j = ascii_len - 1; j >= 0; j--) //허프만 코드 길이만큼 진행
		{
			if (n == 8) {
				fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb);
				bitstream = 0; //bitstream 초기화
				n = 0;
			}
			int temp = 0;

			temp = (huff_code_int >> j & 1 ? 1 : 0);
			if (temp == 1)
			{
				bitstream <<= 1;
				bitstream = bitstream | 1;
			}
			else
			{
				bitstream <<= 1;
				bitstream = bitstream | 0;
			}
			n++;
		}
		pq_huff_encoding.pop();



	}

	int location = 0;
	//텍스트 내용 

	
	unsigned char tem;
	filename = fopen(file.c_str(), "rt");
	while (fscanf(filename, "%c", &index) != EOF)
	{
		//입력받은 ascii의 허프만 코드 찾기
		for (int i = 0;; i++) {
			ascii_code = encoding_ascii_code[i];
			if (ascii_code == index)
			{
				ascii_len = encoding_ascii_len[i];
				huff_code_int = encoding_huff_code_int[i];
				break;
			}
		}


		for (int j = ascii_len - 1; j >= 0; j--) //허프만 코드 길이만큼 진행
		{
			if (n == 8) {
				fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb);
				
				bitstream = 0; //bitstream 초기화
				n = 0;
			}
			int temp = 0;

			temp = (huff_code_int >> j & 1 ? 1 : 0);
			if (temp == 1)
			{
				bitstream <<= 1;
				bitstream = bitstream | 1;
				write_hft << 1;
			}
			else
			{
				bitstream <<= 1;
				bitstream = bitstream | 0;
				write_hft << 0;
			}
			n++;
		}
	}


	fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb); //마지막에 남은 비트스트림 저장

	fseek(write_hfb, 1, SEEK_SET);
	fwrite(&n, sizeof(unsigned char), 1, write_hfb);





	return true;

}
int main()
{

	string input;
	cout << "압축할 텍스트 파일명을 입력해주세요" << '\n';
	cin >> input;
	input = input + ".txt";
	huffman_encode(input);
	

}
