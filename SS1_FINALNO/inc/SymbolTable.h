#pragma once
#include <iostream>
#include <vector>

#include "Symbol.h"

using namespace std;

class SymbolTable {
private:
	vector<Symbol> symbolTable;
public:
	SymbolTable();
	void insertSymbol(Symbol symbol);
	bool checkSymbolExistence(string name);
	Symbol getSectionlByNum(int sectionNum);
	bool sectionExistence(string name, int& sectionNum, int& size);
	void printSymbolTable(ofstream& outfile);
	void printSymbolTable(int offset, ofstream& outfile);
	void setSectionSize(int section, int size);
	void setSymbolVisibility(string symbol, string visibility);
	bool symbolExistence(string name, int& symbolNum);
	Symbol getSymbolByNum(int ordinal);
	Symbol getSymbolByName(string name);
	vector<Symbol> getSymbolTable();
	int getTableSize();
	~SymbolTable();
};