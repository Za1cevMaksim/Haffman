#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <locale.h>
#include <string.h>
#include <Windows.h>

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

union CODE {
	unsigned char ch;
	struct {
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	} byte;
};

node* createDecList(char symb, string code) { // decompression init node
	node* pnew = new NODE;
	pnew->level = 0;
	pnew->freq = 0;
	pnew->symb = symb;
	pnew->code = code;
	pnew->isSymb = 0;
	pnew->left = NULL;
	pnew->right = NULL;
	pnew->next = NULL;
	return pnew;
}

node* create(int my_freq, int i) { // compression init node
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

void Add2List(node** pphead, node* pnew) // compression create list 
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

void AddDecList(node** pphead, node* pnew) // decompression create list 
{
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

unsigned char pack(unsigned char buf[8])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b2 = buf[1] - '0';
	code.byte.b3 = buf[2] - '0';
	code.byte.b4 = buf[3] - '0';
	code.byte.b5 = buf[4] - '0';
	code.byte.b6 = buf[5] - '0';
	code.byte.b7 = buf[6] - '0';
	code.byte.b8 = buf[7] - '0';
	//cout << code.ch;
	return code.ch;
}

void MakeCodeTree(NODE* head, string s) { // compression make tree; take 1 char return code of char in tree
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

string FindCode(char s, node* phead) { // compression 
	while (phead->symb != static_cast<char>(s)) {
		phead = phead->next;
	}
	string code = phead->code;
	return code;
}

void createHeading(node* phead, FILE* fr) // compression; create heading in file_out
{
	while (phead)
	{
		if (phead->isSymb) {
			string code = phead->code;
			const char* cstr = code.c_str();
			fwrite(&(phead->symb), sizeof(char), 1, fr);
			fwrite(&("-"), sizeof(char), 1, fr);
			fwrite(cstr, code.size(), 1, fr);
			fwrite(&("|"), sizeof(char), 1, fr);
			fwrite(&("|"), sizeof(char), 1, fr);
		}
		phead = phead->next;
	}

}

void writeFile(string symb_string, int  ost, node* phead) { // compression write in fail
	FILE* fr = fopen("test_out.txt", "wb");

	char ostatok = ost + '0';
	fwrite(&ostatok, sizeof(char), 1, fr);
	createHeading(phead, fr);
	char end = '>';
	fwrite(&end, sizeof(char), 1, fr);
	fwrite(&end, sizeof(char), 1, fr);
	const char* cstr = symb_string.c_str();
	fwrite(cstr, symb_string.size(), 1, fr);
	fclose(fr);
}

string createSymbLine(int ost, string code_string) { // compression; take all code line, divide on 8; than make new symb and write them in otput line;
	string symb_string = "";
	for (int i = 0; i < code_string.size(); i = i + 8) {
		unsigned char buf[8];
		if ((i + 8) <= code_string.size()) {
			buf[0] = code_string[i + 7];
			buf[1] = code_string[i + 6];
			buf[2] = code_string[i + 5];
			buf[3] = code_string[i + 4];
			buf[4] = code_string[i + 3];
			buf[5] = code_string[i + 2];
			buf[6] = code_string[i + 1];
			buf[7] = code_string[i];
		}
		else {
			unsigned char not_buf[8];
			for (int z = 0; z < 8- ost; z++) {
				not_buf[z] = code_string[z+i];
			}
			for (int z = 8-ost; z < 8; z++) {
				not_buf[z] = '0';
			}
			for (int i = 0;i < 8;i++) {
				buf[i] = not_buf[7 - i];
			}
		}
		char symb = pack(buf);

		symb_string = symb_string + (symb);
	}
	return symb_string;
}

string SymbToCode(int s) { // decompression take code of 1 char; return ascii code
	string code = "";
	if (s < 0) {
		s = s + 256;
	}
	while (s) {
		if (s % 2 == 0) {
			code = "0" + code;
		}
		else {
			code = "1" + code;
		}
		s = s / 2;
	}
	while (code.size() < 8) {
		code = "0" + code;
	}

	return code;
}

string FindInList(node* phead, string code) {
	while (phead) {
		if (phead->code.size() > code.size()) {
			return "-10";
		}
		if (phead->code == code) {
			char z = phead->symb;
			string s = "";
			s = s + z;
			return s;
		}
		else {
			phead = phead->next;
		}
	}
	return "-10";
}

string CodeLineToSymb(string s, node* phead, int ost) {// decompression take string of input code; return decompression code
	string code = "";
	string out = "";
	for (int i = 0; i < s.size()-ost; i++) {
		code = code + s[i];
		string z = FindInList(phead, code);
		if (z != "-10") {
			out = out + z;
			code = "";
		}
	}

	return out;
}

int compressed() {
	node* phead = NULL;
	// open file part
	FILE* fr = fopen("test1.txt", "rb");
	if (!fr)
		return 3;
	fseek(fr, 0L, SEEK_END);
	long length = ftell(fr);
	fseek(fr, 0, SEEK_SET);
	string org_line;

	for (int i = 0; i < length; ++i)
	{
		unsigned char z = fgetc(fr);
		freq[z] ++;
		org_line += z;
	}

	fclose(fr);



	// create list struct
	for (int i = 0; i < 256; i++) {
		if (freq[i] != 0) {
			Add2List(&phead, create(freq[i], i));
		}
	}

	// make code tree part
	node* treeHead = MakeTreeFromList(phead);
	MakeCodeTree(treeHead, "");


	//create string where every symbol is tree code

	string code_string = "";
	for (int i = 0; i < length; i++) {
		code_string = code_string + FindCode(org_line[i], phead);
	}
	
	// convert from code string to symb string 
	int ost = 8 - code_string.size() % 8; // ost= ostatok ot delenia
	string symb_string = createSymbLine(ost, code_string);

	// write in fail part
	writeFile(symb_string, ost, phead);
	return 1;
}

void WriteDecFile(string symb_string) {
	FILE* fr = fopen("Decompressoin.txt", "wb");
	const char* cstr = symb_string.c_str();
	fwrite(cstr, symb_string.size(), 1, fr);
}

int decompresed() {
	node* phead = NULL;
	
	// read code file
	FILE* fz = fopen("test_out.txt", "rb");
	if (!fz)
		return 3;
	fseek(fz, 0L, SEEK_END);
	long length = ftell(fz);
	fseek(fz, 0, SEEK_SET);
	string in_line;
	int ost = fgetc(fz) - 48;
	for (int i = 0; i < length; ++i)
	{
		unsigned char z = fgetc(fz);
		in_line += z;
	}
	

	//read and make list code of char
	string code = "";
	char symb = '0';
	int i = 0;
	for (i;;) {
		if (in_line[i] == '>' && in_line[i + 1] == '>') {
			break;
		}
		symb = in_line[i];
		i = i + 2;
		while (in_line[i] != '|' && in_line[i + 1] != '|') {
			code = code + in_line[i];
			i = i + 1;
		}
		code = code + in_line[i];
		i = i + 3;
		AddDecList(&phead, createDecList(symb, code));
		code = "";
		symb = '0';
	}

	
	//make code line; every symbol will be change on his ascii code;
	string code_line = "";
	i = i + 2;
	for (i; i < in_line.size()-1; i++) {
		string z = SymbToCode((int)in_line[i]);
		code_line = code_line + z;
		//cout <<i<<" "<< in_line[i] << " " << (int)in_line[i] << " " << z << endl;
	}
	
	//create out line; than write in decompression file;
	string out = "";
	out = CodeLineToSymb(code_line, phead, ost);
	

	//write in file
	WriteDecFile(out);
	return 1;
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);// to read russian file
	int status = compressed();
	if (status == 3) {
		cout << "Cant Open File" << endl;
	}
	else if (status == 1) {
		cout << "File compressed successfully" << endl;
	}
	int decompress = decompresed();
	if (decompress == 3) {
		cout << "Cant Open Decompression file" << endl;
	}
	else if (decompress == 1) {
		cout << "File decompressed successfully" << endl;
	}
}
