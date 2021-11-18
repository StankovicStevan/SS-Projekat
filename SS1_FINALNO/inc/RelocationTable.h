#pragma once
#include <iostream>
#include <vector>

#include "RelocationData.h"

using namespace std;

class RelocationTable {
private:
	vector<RelocationData> relocationTable;
	string section;
	int ordinal;
public:
	RelocationTable(string section);
	void insertData(RelocationData data);
	void printTable(ofstream& outfile);
	string getSectionName();
	RelocationTable* getRelocationTable();
	void setRelocationData(RelocationData data);
};