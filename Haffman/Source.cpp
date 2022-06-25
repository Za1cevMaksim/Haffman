#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <locale.h>
#include <string.h>
#include <bitset>


using namespace std;
unsigned freq[256];

typedef struct node {
	char symb;
	bool isSymb;
	unsigned int freq;
	string code;
	int level;
	struct node* left, * right, * next;
}NODE;


struct Bits
{
	unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
};
union CBits
{
	Bits bits;
	unsigned char byte;
};



node* create(int my_freq, int i) {
	node* pnew = new NODE;
	pnew->level = 0;
	pnew->freq = my_freq;
	pnew->symb = static_cast<char>(i);
	pnew->isSymb = 1;
	pnew->left = NULL;
	pnew->right = NULL;
	pnew->next = NULL;
	return pnew;
}

void Add2List(node** pphead, node* pnew)
{
	while (*pphead)
	{
		if ((*pphead)->freq > pnew->freq)
			break;
		pphead = &((*pphead)->next);
	}
	pnew->next = *pphead;
	*pphead = pnew;
}

void PrintList(node* phead)
{
	while (phead)
	{
		cout << phead->symb << "  " << phead->code << endl;
		phead = phead->next;
	}
	cout << ("\n");
}



NODE* MakeNodeFromNode(NODE* left, NODE* right)
{
	NODE* res = new NODE;
	res->freq = left->freq + right->freq;
	res->isSymb = 0;
	res->symb = 0;
	if (left != NULL) {
		res->left = left;
	}
	else {
		res->left = NULL;
	}
	if (right != NULL) {
		res->right = right;
	}
	else {
		res->right = NULL;
	}

	res->next = 0;


	return res;
}


NODE* MakeTreeFromList(NODE* head)
{
	while (head && head->next)
	{
		NODE* left = head;
		NODE* right = head->next;
		Add2List(&(head->next->next), MakeNodeFromNode(left, right));
		head = head->next->next;
	}
	return head;
}


void MakeCodeTree(NODE* head, string s) {
	if (head->isSymb) {
		head->code = s;
	}
	if (head->left) {
		MakeCodeTree(head->left, s + "0");
	}
	if (head->right) {
		MakeCodeTree(head->right, s + "1");
	}
}

void FindCode(FILE* file, char s, node* phead) {
	while (phead->symb != static_cast<char>(s)) {
		phead = phead->next;
	}
	string code = phead->code;
	CBits byte;
	while (code.size() != 8) {
		code = "0" + code;
	}
	byte.bits.b0 = (code[0] == '1') ? 1 : 0;
	byte.bits.b1 = (code[1] == '1') ? 1 : 0;
	byte.bits.b2 = (code[2] == '1') ? 1 : 0;
	byte.bits.b3 = (code[3] == '1') ? 1 : 0;
	byte.bits.b4 = (code[4] == '1') ? 1 : 0;
	byte.bits.b5 = (code[5] == '1') ? 1 : 0;
	byte.bits.b6 = (code[6] == '1') ? 1 : 0;
	byte.bits.b7 = (code[7] == '1') ? 1 : 0;
	fwrite(&byte, 1, 1, file);
}


void PrintTree(const NODE* root, int level)
{
	if (root)
	{
		for (int i = 0; i < level; ++i)
		{
			cout << " ";
		}
		cout << root->symb << " " << root->code;
		PrintTree(root->left, level + 1);
		PrintTree(root->right, level);
	}
}

int main()
{
	setlocale(LC_ALL, "Rus");
	node* phead = NULL;
	FILE* fr = fopen("test.txt", "rb");
	if (!fr)
		return 3;
	fseek(fr, 0L, SEEK_END);
	long length = ftell(fr);
	fseek(fr, 0, SEEK_SET);
	string s;
	for (int i = 0; i < length; ++i)
	{
		unsigned char z = fgetc(fr);
		freq[z] ++;
		s += z;
	}
	fclose(fr);
	for (int i = 0;i < 256;i++) {
		if (freq[i] != 0) {
			Add2List(&phead, create(freq[i], i));
		}
	}
	node* treeHead = MakeTreeFromList(phead);
	MakeCodeTree(treeHead, "");
	PrintList(phead);
	FILE* out = fopen("test_out.txt", "wb");
	for (int i = 0;i < length;i++) {
		FindCode(out, s[i], phead);
	}
	return 1;
}