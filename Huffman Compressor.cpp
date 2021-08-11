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
	//������

	unsigned char ascii_code;
	int freq;	//�󵵼�

	//�ڽĳ��

	tree* leftnode;
	tree* rightnode;
};



class code
{
public:

	//data
	unsigned char ascii_code;
	string huffcode;	//��������ȣȭ�ڵ尡 ����
};



tree* HuffmanTree_Insert(tree* u, tree* v) {
	tree* pt;
	pt = new tree;	//�θ� ��� ����
	pt->ascii_code = NULL;
	pt->freq = u->freq + v->freq;	//�󵵼��� ���Ѵ�.
	pt->leftnode = u;			//��, �� ��� ����
	pt->rightnode = v;
	return pt;		//�θ��� ��ȯ
}

struct Mycomp_tree {
	bool operator()(const tree* l, const tree* r) {
		return l->freq > r->freq;	//��������
	}
};

struct Mycomp_code {
	bool operator()(const code* l, const code* r) {
		return l->huffcode.length() > r->huffcode.length();	//��������
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

	//�󵵼� ���

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
	////��� �����


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


	////Ʈ�������

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
	//������ �ڵ� �����

	inorder(root, pq_huff, "");
	pq_huff_encoding = pq_huff; //���ڵ��� ���� ī��
	pq_huff_hft = pq_huff; //hft�� ���� ī��



	//������� 
	ofstream write_hft("output.hft");
	FILE* write_hfb = fopen("output.htb", "wb");

	int n = 0;
	unsigned char ascii_code;
	unsigned char ascii_len;
	int ascii_huff_code;
	unsigned char bitstream = 0;

	//���ڵ��� ���� ����
	vector<unsigned char> encoding_ascii_code;
	vector<int> encoding_ascii_len;
	vector<int> encoding_huff_code_int;


	//hft ����
	write_hft << "���ڵ��� ������ ���� = " << encoded_num << endl;//���ڵ��� ���� �Է�
	for (int i = 0; i < encoded_num; i++)
	{


		ascii_code = pq_huff_hft.top()->ascii_code; //�Է¹���
		ascii_len = pq_huff_hft.top()->huffcode.length();//�������ڵ� ����

		write_hft << ascii_code << " => " << "������ �ڵ� : " << pq_huff_hft.top()->huffcode << " , ���� = " << (int)ascii_len << endl;
		pq_huff_hft.pop();
	}
	write_hft << "���̳ʸ��� ��ȯ�� �ؽ�Ʈ���� : " ;


	//��������
	fwrite(&encoded_num, sizeof(unsigned char), 1, write_hfb); //���ڵ��� ����
	fwrite(&encoded_num, sizeof(unsigned char), 1, write_hfb); //������ MSB�� ���� �ӽ� ����
	for (int i = 0; i < encoded_num; i++)
	{
		ascii_code = pq_huff.top()->ascii_code; //����� �Է¹���
		ascii_len = pq_huff.top()->huffcode.length();//����� �������ڵ� ����

		//����� �Է¹��ڿ� ���� �Է�
		fwrite(&ascii_code, sizeof(unsigned char), 1, write_hfb);
		fwrite(&ascii_len, sizeof(unsigned char), 1, write_hfb);
		pq_huff.pop();
	}

	int huff_code_int = 0;

	//��Ʈ��Ʈ��
	while (!pq_huff_encoding.empty())
	{
		ascii_code = pq_huff_encoding.top()->ascii_code; //������ �Է¹���
		ascii_len = pq_huff_encoding.top()->huffcode.length();//������ �������ڵ� ����
		string huff_code = pq_huff_encoding.top()->huffcode;//������ �ڵ� string

		huff_code_int = 0;
		for (int k = 0; k < ascii_len; k++)//stirng huffman �� int huffman�� ��ȯ
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

		//������ ���� ���� ���� �Է�
		encoding_ascii_code.push_back(ascii_code);
		encoding_ascii_len.push_back((int)ascii_len);
		encoding_huff_code_int.push_back(huff_code_int);


		//��Ʈ��Ʈ���� �̿��� ����
		for (int j = ascii_len - 1; j >= 0; j--) //������ �ڵ� ���̸�ŭ ����
		{
			if (n == 8) {
				fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb);
				bitstream = 0; //bitstream �ʱ�ȭ
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
	//�ؽ�Ʈ ���� 

	
	unsigned char tem;
	filename = fopen(file.c_str(), "rt");
	while (fscanf(filename, "%c", &index) != EOF)
	{
		//�Է¹��� ascii�� ������ �ڵ� ã��
		for (int i = 0;; i++) {
			ascii_code = encoding_ascii_code[i];
			if (ascii_code == index)
			{
				ascii_len = encoding_ascii_len[i];
				huff_code_int = encoding_huff_code_int[i];
				break;
			}
		}


		for (int j = ascii_len - 1; j >= 0; j--) //������ �ڵ� ���̸�ŭ ����
		{
			if (n == 8) {
				fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb);
				
				bitstream = 0; //bitstream �ʱ�ȭ
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


	fwrite(&bitstream, sizeof(unsigned char), 1, write_hfb); //�������� ���� ��Ʈ��Ʈ�� ����

	fseek(write_hfb, 1, SEEK_SET);
	fwrite(&n, sizeof(unsigned char), 1, write_hfb);





	return true;

}
int main()
{

	string input;
	cout << "������ �ؽ�Ʈ ���ϸ��� �Է����ּ���" << '\n';
	cin >> input;
	input = input + ".txt";
	huffman_encode(input);
	

}
