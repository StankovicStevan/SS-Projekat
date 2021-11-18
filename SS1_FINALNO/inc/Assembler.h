#pragma once
#include <iostream>
#include <vector>

#include "SymbolTable.h"
#include "RelocationTable.h"
#include "SectionTable.h"

using namespace std;

class Assembler {
private:
	void firstPass();
	void secondPass();
	vector<string> tokenize(string line);
	vector<string> tokenizeComma(string line);
	void ltrim(std::string& s);

	struct Triplet {
		int ordinal;
		string sectionName;
		int sectionNum;
	};
	vector <Triplet> listOfSections;
	vector <RelocationTable> listOfRelocationTables;
	vector <SectionTable> listOfSectionTables;
	vector<pair<string, int>> helpPairsContentTable; //Prvi je linija, drugi je offset
	void callOneRegOneOper(string opCode, string regD, string operand, int currentSection, string line);
	void callTwoRegs(string opCode, string regD, string regS, int currentSection);
	void callOneRegFunc(string opCode, string regD, int currentSection);
	void callPushPop(string opCode, string addrCode, string regD, int currentSection);
	void callSingleOperand(string opCode, string operand, int currentSection, string line);
	void callNoOpCode(string data, int currentSection);
	void callSkipOper(string data, int currentSection);

	string inputFile;
	string outputFile;
	SymbolTable* symbolTable;
	SymbolTable* symbolTableSections;
	SymbolTable* symbolTableSymbols;
	RelocationTable* relocationTable;
	string checkType(string data);
	void removeSpecialCharacter(string &s);

	string checkAddrModeJMP(string data, string line, int currentSection);

	string checkAddrModeDATA(string data, string line, int currentSection);

	string appendTo5B(string operand, string line, int currentSection);

	string removeSpecialCharacterReturn(string s);

	string removeSpecialCharacterReturnSTR_LDR(string s);

	
public:
	Assembler(string inputFile, string outputFile);
	//~Assembler();
};