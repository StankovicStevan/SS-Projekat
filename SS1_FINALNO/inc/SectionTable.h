#pragma once
#include <iostream>

using namespace std;

class SectionTable {
private:
	string code;
	string section;
public:
	SectionTable(string code, string section);
	string getCode();
	void setCode(string code);
	string getSection();
	void setSection(string section);
	void addCode(string code);
	void printTable(ofstream& outfile);
};
