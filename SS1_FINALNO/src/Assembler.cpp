#include "Assembler.h"
#include "RegexExpr.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>


using namespace std;

Assembler::Assembler(string inputFile, string outputFile) {
	this->inputFile = inputFile;
	this->outputFile = outputFile;

	this->symbolTable = new SymbolTable();
	this->symbolTableSections = new SymbolTable();
	this->symbolTableSymbols = new SymbolTable();

	/*fstream myFile;
	cout << "izlazni fajl je : " << outputFile << endl;
	myFile.open("D:/Faks/4. godina/SS/Projekat_Moj/Projekat_SS/Projekat_SS/izlaz.txt", ios::out);
	if (myFile.is_open()) {
		cout << "OTVOREN JE FAJL " << endl;
	}
	else{
		cout << "NIJE OTVOREN !!!" << endl;
	}
	myFile << "Writing this to a file.\n";
	myFile.close();*/

	ofstream outfile;
	outfile.open(outputFile);
	//std::ofstream outfile(outputFile);	

	this->firstPass();

	
	
	//listOfRelocationTables.push_back(make_pair(string("UND"), RelocationTable(string("UND"))));
	//listOfRelocationTables[0].second.insertData(RelocationData(0, string("tip"), 0));
	//listOfSectionTables.push_back(SectionTable(string("01234567890123456789012345678901234567890123456789012345678901234567890123456789"), string("UND")));
	for (int i = 0; (unsigned)i < this->listOfSections.size();  i++) {
		listOfRelocationTables.push_back(RelocationTable(listOfSections[i].sectionName));
		listOfSectionTables.push_back(SectionTable("", listOfSections[i].sectionName));
	}

	//cout << "Velicina reloc tabele je : " << listOfRelocationTables.size() << endl;

	this->secondPass();

	outfile << setw(15) << "Ordinal" << setw(15) << "Name" << setw(15) << "Section" << setw(15) << "Value"
		<< setw(15) << "Local" << setw(15) << "Size" << endl;

	this->symbolTableSections->printSymbolTable(outfile);
	this->symbolTableSymbols->printSymbolTable(this->symbolTableSections->getTableSize(), outfile);

	for (vector<Triplet>::iterator it = listOfSections.begin(); it != listOfSections.end(); ++it) {
		for (vector<RelocationTable>::iterator itR = listOfRelocationTables.begin(); itR != listOfRelocationTables.end(); ++itR) {
			if (it->sectionName == itR->getSectionName()) {
				itR->printTable(outfile);
			}
		}
		for (vector<SectionTable>::iterator itS = listOfSectionTables.begin(); itS != listOfSectionTables.end(); ++itS) {
			if (it->sectionName == itS->getSection()) {
				itS->printTable(outfile);
			}
		}
	}
	outfile.close();
	cout << "Vidljivost je : " << this->symbolTable->getSymbolByName("term_out").getLocal() << endl;
}

void Assembler::firstPass() {
	ifstream inFile(inputFile);
	if(inFile.is_open()){
		cout << "Otvoren je fajl" << endl;
	}
	else{
		cout << "Nije otvoren fajl " << endl;
	}
	string line;
	unsigned long lineNumber = 0;
	unsigned long locationCounter = 0;
	int currentSection = 1;
	int currentOrdinal = 0;
	int currentOrdinalSection = 0;
	int currentOrdinalSymbol = 0;
	

	int sectionNumber = 0;
	int size = 0;
	this->symbolTable->insertSymbol(Symbol(currentOrdinal++, string("UND"), 0, 0, string("local"), 0));
	this->symbolTableSections->insertSymbol(Symbol(currentOrdinalSection++, string("UND"), 0, 0, string("local"), 0));
	listOfSections.push_back(Triplet{ currentOrdinal - 1, string("UND"), 0 });
	this->symbolTable->insertSymbol(Symbol(currentOrdinal++, string("ABS"), 1, 0, string("local"), 0));
	this->symbolTableSections->insertSymbol(Symbol(currentOrdinalSection++, string("ABS"), 1, 0, string("local"), 0));
	listOfSections.push_back(Triplet{ currentOrdinal - 1, string("ABS"), 1 });

	while (getline(inFile, line)) {
		cout << line << endl;
		lineNumber++;
		vector<string> tokens = tokenize(line);

		if (regex_match(tokens[0], regex_GLOBAL)) {
			//cout << "Radi global " << endl;
			continue;
			
		}
		else if (regex_match(tokens[0], regex_SECTION)) {
			int ordinalNum;
			for (vector<Triplet>::iterator it = listOfSections.begin(); it != listOfSections.end(); ++it) {
				if (it->sectionNum == currentSection) {
					ordinalNum = it->ordinal;
				}
			}
			this->symbolTable->setSectionSize(ordinalNum, locationCounter);
			int ordinalNumSection = this->symbolTableSections->getSymbolByNum(currentSection).getOrdinal();
			
			this->symbolTableSections->setSectionSize(ordinalNumSection, locationCounter);
			if (!this->symbolTable->checkSymbolExistence(tokens[0])) {
				locationCounter = 0;
				int help = ++currentSection;
				this->symbolTable->insertSymbol(Symbol(currentOrdinal++, tokens[1], help, 0, string("local"), 0));
				this->symbolTableSections->insertSymbol(Symbol(currentOrdinalSection++, tokens[1], help, 0, string("local"), 0));
				listOfSections.push_back(Triplet{ currentOrdinal - 1, tokens[1], currentSection });
				
			}
			else {
				this->symbolTable->sectionExistence(tokens[1], sectionNumber, size);
				currentSection = sectionNumber;
				locationCounter = size;
			}
		}
		else if (regex_match(tokens[0], regex_EXTERN)) { 
			int i = 0;
			string name;
			for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
				
				if (i != 0) {
					//cout << "IME JE " << *it << endl;
					
					vector<string> tokensInsideString = tokenizeComma(*it);
					for (vector<string>::iterator it1 = tokensInsideString.begin(); it1 != tokensInsideString.end(); ++it1) {
						this->symbolTable->insertSymbol(Symbol(currentOrdinal++, *it1, 0, 0, string("global"), 0));
						this->symbolTableSymbols->insertSymbol(Symbol(currentOrdinalSymbol++, *it1, 0, 0, string("global"), 0));
						name = *it1;
						//cout << "IME JE " << *it1 << endl;
					}
				}
				i++;

			}
			//cout << "Radi extern, a redni broj je " << currentOrdinal << " i ime promenljive je " << name << endl;
			
		}
		else if (regex_match(tokens[0], regex_WORD)) {
			int i = 0;
			for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
				
				if (i++ == 0) continue;
				
				vector<string> tokensInsideString = tokenizeComma(*it);
				//cout << "Rec je : " << tokensInsideString[0] << endl;
				for (vector<string>::iterator it = tokensInsideString.begin(); it != tokensInsideString.end(); it++) {
					//cout << "Rec je : " << *it << endl;
					locationCounter += 2;
				}
			}
		}
		else if (regex_match(tokens[0], regex_SKIP)) {
			if (regex_match(tokens[1], regex_DEC_NUM)) {
				locationCounter += stoi(tokens[1]);
			}
			else if (regex_match(tokens[1], regex_HEX_NUM)) {
				locationCounter += stoi(tokens[1], 0, 16);
			}
			
		}
		else if (regex_match(tokens[0], regex_EQU)) {
			string symbolName = tokenizeComma(tokens[1])[0];
			//symbolName.erase(symbolName.size()-1,1);
			int symbolValue; //PREPRAVITI ZA DEC I HEX
			bool symbolValueFlag = false;
			//cout << "Ime simbola je : " << symbolName << " Vrednost simbola je : " << tokens[2] << endl;
			if (regex_match(tokens[2], regex_DEC_NUM)) {
				//cout << "Usao u if 1 " << endl;
				symbolValueFlag = true;
				symbolValue = stoi(tokens[2]);
			}
			else if (regex_match(tokens[2], regex_HEX_NUM)) {
				//cout << " Usao u if 2 " << endl;
				symbolValueFlag = true;
				symbolValue = stoi(tokens[2], 0, 16);
			}
			if (symbolName.empty() || !symbolValueFlag) {
				cout << "Error - EQU instruction is in wrong format, missing symbol name or symbol value ! Line: "<< line << endl;
			}
			else {
				if (!this->symbolTable->checkSymbolExistence(symbolName)) {
					this->symbolTable->insertSymbol(Symbol(currentOrdinal++, symbolName, 1, symbolValue, string("local"), 0));
					this->symbolTableSymbols->insertSymbol(Symbol(currentOrdinalSymbol++, symbolName, 1, symbolValue, string("local"), 0));
				}
			}
		}
		else if (regex_match(tokens[0], regex_END)) {
			if (this->symbolTable->getSectionlByNum(currentSection).getName() == "Error") {
				cout << "Error - No section found ! Line: " << line << endl;
			}
			else {
				int ordinalNum;
				for (vector<Triplet>::iterator it = listOfSections.begin(); it != listOfSections.end(); ++it) {
					if (it->sectionNum == currentSection) {
						ordinalNum = it->ordinal;
						break;
					}
				}
				this->symbolTable->setSectionSize(ordinalNum, locationCounter);
				int ordinalNumSection = this->symbolTableSections->getSymbolByNum(currentSection).getOrdinal();
				
				this->symbolTableSections->setSectionSize(ordinalNumSection, locationCounter);
			}
			//cout << "Vrednost Location Counter-a je : " << locationCounter << " a broj sekcije je " << currentSection << endl;
		}
		else if (regex_match(tokens[0], regex_LABEL)) {//POPRAVITI DA MOZE DA BUDE DEFINISANA LABELA I JOS NESTO U ISTOM REDU
			//cout << "Usao u if za EQU " << endl;
			if (this->symbolTable->checkSymbolExistence(tokens[0])) {
				cout << "Error - Label already exists ! Line: " << line << endl;
			}
			else {
				int pos = tokens[0].find(":");
				if (pos != 0) {
					//string sub = tokens[0].substr(pos + 1);
					string sub = tokens[0];
					sub.erase(remove(sub.begin(), sub.end(), ':'), sub.end());
					//cout << "Ime novog stringa je : " << sub << endl;
					//remove(sub.begin(), sub.end(), ' ');
					this->symbolTable->insertSymbol(Symbol(currentOrdinal++, sub, currentSection, locationCounter, string("local"), 0));
					this->symbolTableSymbols->insertSymbol(Symbol(currentOrdinalSymbol++, sub, currentSection, locationCounter, string("local"), 0));
				}
				else {
					cout << "Error - There has to be SEMI to identify label ! Line: " << line << endl;
				}

				
			}
		}
		else if (regex_match(tokens[0], regex_COMMENT)) {
			continue;
		}
		else if (regex_match(tokens[0], regex_INSTR)) {
		//cout << "Usao u if za INSTR " << endl;
			if (regex_match(tokens[0], regex_HALT)) {
				locationCounter++;
			}
			if (regex_match(tokens[0], regex_INT)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_IRET)) {
				locationCounter++;
			}
			if (regex_match(tokens[0], regex_CALL)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				cout <<"TokensHelp[0] : " << tokensHelp[0] << endl;
				tokensHelp[0].erase(std::remove(tokensHelp[0].begin(), tokensHelp[0].end(), ','), tokensHelp[0].end());
				if (regex_match(tokensHelp[0], regex_3B_jump)) {
					//cout << "Usao u 3B jump " << endl;
					locationCounter += 3;
				}
				else if(regex_match(tokensHelp[0], regex_5B_jump)){
					//cout << "Usao u 5B jump " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_RET)) {
				locationCounter++;
			}
			if (regex_match(tokens[0], regex_JMP)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				if (regex_match(tokensHelp[0], regex_3B_jump)) {
					//cout << "Usao u 3B jump " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_jump)) {
					//cout << "Usao u 5B jump " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_JEQ)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				if (regex_match(tokensHelp[0], regex_3B_jump)) {
					//cout << "Usao u 3B jump " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_jump)) {
					//cout << "Usao u 5B jump " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_JNE)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				if (regex_match(tokensHelp[0], regex_3B_jump)) {
					//cout << "Usao u 3B jump " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_jump)) {
					//cout << "Usao u 5B jump " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_JGT)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				if (regex_match(tokensHelp[0], regex_3B_jump)) {
					//cout << "Usao u 3B jump " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_jump)) {
					//cout << "Usao u 5B jump " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_PUSH)) { 
				locationCounter += 3;
			}
			if (regex_match(tokens[0], regex_POP)) {
				locationCounter += 3;
			}
			if (regex_match(tokens[0], regex_XCHG)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_ADD)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_SUB)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_MUL)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_DIV)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_CMP)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_NOT)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_AND)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_OR)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_XOR)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_TEST)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_SHL)) {
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_SHR)) { 
				locationCounter += 2;
			}
			if (regex_match(tokens[0], regex_LDR)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[2]);
				//cout << "Operand je LDR: " << tokensHelp[0] << endl;
				if (regex_match(tokensHelp[0], regex_3B_data)) {
					//cout << "Usao u 3B data " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_data)) {
					//cout << "Usao u 5B data " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
			if (regex_match(tokens[0], regex_STR)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[2]);
				//cout << "Operand je STR: " << tokensHelp[0] << endl;
				if (regex_match(tokensHelp[0], regex_3B_data)) {
					//cout << "Usao u 3B data " << endl;
					locationCounter += 3;
				}
				else if (regex_match(tokensHelp[0], regex_5B_data)) {
					//cout << "Usao u 5B data " << endl;
					locationCounter += 5;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
			}
		}
		else {
			cout << "Error - Wrong input ! Line: " << line << endl;
		}
		//cout << "Vrednost Location Counter-a je : " << locationCounter << endl;
	}
	//cout << "Vrednost Location Counter-a je : " << locationCounter << endl;
}
	

void Assembler::secondPass() {
	cout << "Welcome to second pass" << endl;
	ifstream inFile(inputFile);
	string line;
	string opCode;
	vector<string> inMemory;
	int currentInHelpList = 0;

	vector<unsigned long> locationCounters;
	for (int i = 0; i < this->symbolTableSections->getTableSize(); i++) {
		locationCounters.push_back(0);
	}
	unsigned long locationCounter = 0;
	int currentSection = 0;

	while (getline(inFile, line)) {
		vector<string> tokens = tokenize(line);

		if (regex_match(tokens[0], regex_GLOBAL)) {
			int i = 0;
			int symbolOrdinal;
			for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {

				if (i != 0) {


					vector<string> tokensInsideString = tokenizeComma(*it);
					string s = *it;
					s.erase(std::remove(s.begin(), s.end(), ','), s.end());
					for (vector<string>::iterator it1 = tokensInsideString.begin(); it1 != tokensInsideString.end(); ++it1) {
						if (!this->symbolTable->symbolExistence(s, symbolOrdinal)) {
							cout << "Simbol je : " << s << endl;
							cout << "Error - Symbol that we want to export is not defined ! " << endl;
						}
						else {
							this->symbolTable->setSymbolVisibility(s, string("global"));
							this->symbolTableSymbols->setSymbolVisibility(s, string("global"));
						}

					}
				}
				i++;

			}

		}
		else if (regex_match(tokens[0], regex_SECTION)) {

			locationCounters[currentSection] = locationCounter;
			Symbol currentSectionSymbol = this->symbolTableSections->getSymbolByName(tokens[1]);
			currentSection = currentSectionSymbol.getOrdinal();
			locationCounter = locationCounters[currentSection];

		}
		else if (regex_match(tokens[0], regex_WORD)) { 
			string sectionName;
			string token;
			for (int i = 1; (unsigned)i < tokens.size(); i++) {
				token = tokenizeComma(tokens[i])[0];
				
				for (int i = 0; (unsigned)i < listOfSections.size(); i++) {
					if (listOfSections[i].ordinal == currentSection) {
						sectionName = listOfSections[i].sectionName;
						break;
					}
				}
				if (regex_match(token, regex_DEC_NUM)) {

					this->symbolTable->getSymbolByName(sectionName).setValue(stoi(token));
					this->symbolTableSections->getSymbolByName(sectionName).setValue(stoi(token));
				}
				else if (regex_match(token, regex_HEX_NUM)) {

					this->symbolTableSections->getSymbolByName(sectionName).setValue(stoi(token, 0, 16));
				}
				else {

					string global = this->symbolTable->getSymbolByName(token).getLocal() == "global" ? "global" : "local";

					int ordinal;
					if (global == "global") {
						ordinal = this->symbolTableSymbols->getSymbolByName(token).getOrdinal() + this->symbolTableSections->getTableSize();
					}
					else {

						ordinal = this->symbolTableSymbols->getSymbolByName(token).getSection();
					}

					string type = checkType(token);
					bool flagBreak = false;
					for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {

						if (token == *it && (!regex_match(tokens[1], regex(regex_pc_imm_jmp)))) {
							flagBreak = true;
							break;
						}
					}
					if ((regex_match(token, regex(regex_imm_lit_jmp))) || (regex_match(token, regex(regex_reg_dir_jmp))) || (regex_match(token, regex(regex_reg_ind_jmp))) || (regex_match(token, regex(regex_mem_lit_jmp))) || (regex_match(token, regex(regex_indirect_sum_lit_jmp)))) {
						flagBreak = true;
					}
					if (!flagBreak) {
						listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));
						helpPairsContentTable.push_back(make_pair(line, locationCounter));
					}
					
				}
				callNoOpCode(token, currentSection);
				locationCounter += 2;
			}
			


		}
		else if (regex_match(tokens[0], regex_SKIP)) {
			callSkipOper(tokens[1], currentSection);
			if (regex_match(tokens[1], regex_DEC_NUM)) {
				locationCounter += stoi(tokens[1]);
			}
			else if (regex_match(tokens[1], regex_HEX_NUM)) {
				locationCounter += stoi(tokens[1], 0, 16);
			}
			
		}
		else if (regex_match(tokens[0], regex_EQU)) {
			string symbolName = tokenizeComma(tokens[1])[0];
			cout << "Ime simbola je : " << symbolName << " , a duzina simbola je : " << symbolName.size() << endl;
			inMemory.push_back(symbolName);

		}
		else if (regex_match(tokens[0], regex_END)) {
			cout << "End of second pass ! " << endl;

			break;
		}
		else if (regex_match(tokens[0], regex_INSTR)) {

			if (regex_match(tokens[0], regex_HALT)) {
				locationCounter++;
				opCode = string("00");
				listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + opCode);
				//cout << "Trenutni izgled sadrzaja tabele sekcija: " << listOfSectionTables[currentSection].getCode() << endl;

			}
			if (regex_match(tokens[0], regex_INT)) {
				locationCounter += 2;
				//opCode = string("10DF");
				opCode = string("10");
				callOneRegFunc(opCode, tokens[1], currentSection);

			}
			if (regex_match(tokens[0], regex_IRET)) {
				locationCounter++;
				opCode = string("20");
				listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + opCode);

			}
			if (regex_match(tokens[0], regex_CALL)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				locationCounter += 3;
				
				//opCode = string("30FSUA");
				opCode = string("30");
				string operand = removeSpecialCharacterReturn(tokens[1]);
				
				string global = this->symbolTable->getSymbolByName(operand).getLocal() == "global" ? "global" : "local";
				bool flagBreak = false;
				int num = 0;
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {
					cout << "Ime inMemory simbola je : " << *it << " , a duzina je : " << it->size() << endl;
					if (operand == *it && (!regex_match(tokens[1], regex(regex_pc_imm_jmp)))) {
						flagBreak = true;
						break;
					}
				}
				if ((regex_match(tokens[1], regex(regex_imm_lit_jmp))) || (regex_match(tokens[1], regex(regex_reg_dir_jmp))) || (regex_match(tokens[1], regex(regex_reg_ind_jmp))) || (regex_match(tokens[1], regex(regex_mem_lit_jmp))) || (regex_match(tokens[1], regex(regex_indirect_sum_lit_jmp)))) {
					flagBreak = true;
				}

				int ordinal;
				if (global == "global") {
					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {

					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getSection();
				}

				string type = checkType(tokens[1]);
				cout << "Tokens[1] : " << tokens[1] << endl;
				if (regex_match(tokens[1], regex_3B_jump)) {
					num = 0;
				}
				else if (regex_match(tokens[1], regex_5B_jump)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
				if (flagBreak) {
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
					continue;
				}
				else {
					cout << "Ordinal je : " << ordinal << endl;
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
				}

				//listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
				//helpPairsContentTable.push_back(make_pair(line, locationCounter));
				//callSingleOperand(opCode, tokens[1], currentSection, line);
				/*if (regex_match(tokensHelp[0], regex_3B_jump)) {

					locationCounter += 0;
				}
				else if (regex_match(tokensHelp[0], regex_5B_jump)) {

					locationCounter += 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}*/



				
			}
			if (regex_match(tokens[0], regex_RET)) {
				locationCounter++;
				opCode = string("40");
				listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + opCode);

			}
			if (regex_match(tokens[0], regex_JMP)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				locationCounter += 3;
				//opCode = string("50FSUA");
				opCode = string("50");
				string operand = removeSpecialCharacterReturn(tokens[1]);
				
				
				string global = this->symbolTable->getSymbolByName(operand).getLocal() == "global" ? "global" : "local";

				int ordinal;
				if (global == "global") {
					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {
					//cout << "Usao u local " << endl;
					ordinal = this->symbolTable->getSymbolByName(operand).getSection();
				}

				string type = checkType(tokens[1]);
				bool flagBreak = false;
				int num = 0;
				//cout << "Ime simbola je : " <<  tokens[1] << " , a duzina je : " << tokens[1].size() << ", a operand je : " << operand << endl;
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {
					//cout << "Ime inMemory simbola je : " << *it << " , a duzina je : " << it->size() << endl;
					if (operand == *it) {
						if (regex_match(tokens[1], regex(regex_pc_imm_jmp))) {
							break;
						}
						else {
							flagBreak = true;
							break;
						}
						
					}
				}
				cout << "Tokens[1] : " << tokens[1] << endl;
				if ((regex_match(tokens[1], regex(regex_imm_lit_jmp))) || (regex_match(tokens[1], regex(regex_reg_dir_jmp))) || (regex_match(tokens[1], regex(regex_reg_ind_jmp))) || (regex_match(tokens[1], regex(regex_mem_lit_jmp))) || (regex_match(tokens[1], regex(regex_indirect_sum_lit_jmp)))) {
					flagBreak = true;
				}
				if (regex_match(tokens[1], regex_3B_jump)) {
					num = 0;
				}
				else if (regex_match(tokens[1], regex_5B_jump)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
				cout << "Flage break : " << flagBreak << endl;
				if (flagBreak) {
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
					continue;
				}
				else {
					cout << "Ordinal je : " << ordinal << endl;
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
				}
				//listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
				//helpPairsContentTable.push_back(make_pair(line, locationCounter));
				//callSingleOperand(opCode, tokens[1], currentSection, line);
				//if (regex_match(tokensHelp[0], regex_3B_jump)) {
				//	//cout << "Usao u 3B jump " << endl;
				//	locationCounter += 0;
				//}
				//else if (regex_match(tokensHelp[0], regex_5B_jump)) {
				//	//cout << "Usao u 5B jump " << endl;
				//	locationCounter += 2;
				//}
				//else {
				//	cout << "Error - Bad format of operation ! Line : " << line << endl;
				//}

			}
			if (regex_match(tokens[0], regex_JEQ)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				locationCounter += 3;

				//opCode = string("51FSUA");
				opCode = string("51");
				string operand = removeSpecialCharacterReturn(tokens[1]);
				
				string global = this->symbolTable->getSymbolByName(operand).getLocal() == "global" ? "global" : "local";

				int ordinal;
				if (global == "global") {
					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {

					ordinal = this->symbolTable->getSymbolByName(operand).getSection();
				}

				string type = checkType(tokens[1]);
				
				bool flagBreak = false;
				int num = 0;
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {

					if (operand == *it && (!regex_match(tokens[1], regex(regex_pc_imm_jmp)))) {
						flagBreak = true;
						break;
					}
				}
				if ((regex_match(tokens[1], regex(regex_imm_lit_jmp))) || (regex_match(tokens[1], regex(regex_reg_dir_jmp))) || (regex_match(tokens[1], regex(regex_reg_ind_jmp))) || (regex_match(tokens[1], regex(regex_mem_lit_jmp))) || (regex_match(tokens[1], regex(regex_indirect_sum_lit_jmp)))) {
					flagBreak = true;
				}
				if (regex_match(tokens[1], regex_3B_jump)) {
					num = 0;
				}
				else if (regex_match(tokens[1], regex_5B_jump)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}

				if (flagBreak) {
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
					continue;
				}
				else {
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
				}
				//listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
				//helpPairsContentTable.push_back(make_pair(line, locationCounter));
				//callSingleOperand(opCode, tokens[1], currentSection, line);
				//if (regex_match(tokensHelp[0], regex_3B_jump)) {

				//	locationCounter += 0;
				//}
				//else if (regex_match(tokensHelp[0], regex_5B_jump)) {

				//	locationCounter += 2;
				//}
				//else {
				//	cout << "Error - Bad format of operation ! Line : " << line << endl;
				//}

			}
			if (regex_match(tokens[0], regex_JNE)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				locationCounter += 3;
				//opCode = string("52FSUA");
				opCode = string("52");
				string operand = removeSpecialCharacterReturn(tokens[1]);
				
				string global = this->symbolTable->getSymbolByName(operand).getLocal() == "global" ? "global" : "local";

				int ordinal;
				if (global == "global") {
					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {
					ordinal = this->symbolTable->getSymbolByName(operand).getSection();
				}

				string type = checkType(tokens[1]);
				bool flagBreak = false;
				int num = 0;
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {

					if (operand == *it && (!regex_match(tokens[1], regex(regex_pc_imm_jmp)))) {
						flagBreak = true;
						break;
					}
				}
				if ((regex_match(tokens[1], regex(regex_imm_lit_jmp))) || (regex_match(tokens[1], regex(regex_reg_dir_jmp))) || (regex_match(tokens[1], regex(regex_reg_ind_jmp))) || (regex_match(tokens[1], regex(regex_mem_lit_jmp))) || (regex_match(tokens[1], regex(regex_indirect_sum_lit_jmp)))) {
					flagBreak = true;
				}
				if (regex_match(tokens[1], regex_3B_jump)) {
					num = 0;
				}
				else if (regex_match(tokens[1], regex_5B_jump)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}

				if (flagBreak) {
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
					continue;
				}
				else {
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
				}
				//listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
				//helpPairsContentTable.push_back(make_pair(line, locationCounter));
				//callSingleOperand(opCode, tokens[1], currentSection, line);
				//if (regex_match(tokensHelp[0], regex_3B_jump)) {

				//	locationCounter += 0;
				//}
				//else if (regex_match(tokensHelp[0], regex_5B_jump)) {

				//	locationCounter += 2;
				//}
				//else {
				//	cout << "Error - Bad format of operation ! Line : " << line << endl;
				//}

			}
			if (regex_match(tokens[0], regex_JGT)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				vector<string> tokensHelp = tokenizeComma(tokens[1]);
				locationCounter += 3;
				//opCode = string("53FSUA");
				opCode = string("53");

				string operand = removeSpecialCharacterReturn(tokens[1]);

				string global = this->symbolTable->getSymbolByName(operand).getLocal() == "global" ? "global" : "local";

				int ordinal;
				if (global == "global") {
					ordinal = this->symbolTableSymbols->getSymbolByName(operand).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {
					ordinal = this->symbolTable->getSymbolByName(operand).getSection();
				}

				string type = checkType(tokens[1]);
				bool flagBreak = false;
				int num = 0;
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {

					if (operand == *it && (!regex_match(tokens[1], regex(regex_pc_imm_jmp)))) {
						flagBreak = true;
						break;
					}
				}
				if ((regex_match(tokens[1], regex(regex_imm_lit_jmp))) || (regex_match(tokens[1], regex(regex_reg_dir_jmp))) || (regex_match(tokens[1], regex(regex_reg_ind_jmp))) || (regex_match(tokens[1], regex(regex_mem_lit_jmp))) || (regex_match(tokens[1], regex(regex_indirect_sum_lit_jmp)))) {
					flagBreak = true;
				}
				if (regex_match(tokens[1], regex_3B_jump)) {
					num = 0;
				}
				else if (regex_match(tokens[1], regex_5B_jump)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}

				if (flagBreak) {
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
					continue;
				}
				else {
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callSingleOperand(opCode, tokens[1], currentSection, line);
				}
				//listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
				//helpPairsContentTable.push_back(make_pair(line, locationCounter));
				//callSingleOperand(opCode, tokens[1], currentSection, line);
				//if (regex_match(tokensHelp[0], regex_3B_jump)) {

				//	locationCounter += 0;
				//}
				//else if (regex_match(tokensHelp[0], regex_5B_jump)) {

				//	locationCounter += 2;
				//}
				//else {
				//	cout << "Error - Bad format of operation ! Line : " << line << endl;
				//}

			}
			if (regex_match(tokens[0], regex_PUSH)) {
				locationCounter += 3;
				//opCode = string("B012UA");
				opCode = string("B0");
				string addrCode = string("12");
				callPushPop(opCode, addrCode, tokens[1], currentSection);

			}
			if (regex_match(tokens[0], regex_POP)) {
				locationCounter += 3;
				//opCode = string("A042UA");
				opCode = string("A0");
				string addrCode = string("42");
				callPushPop(opCode, addrCode, tokens[1], currentSection);

			}
			if (regex_match(tokens[0], regex_XCHG)) {
				locationCounter += 2;
				//opCode = string("60DS");
				opCode = string("60");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_ADD)) {
				locationCounter += 2;
				//opCode = string("70DS");
				opCode = string("70");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_SUB)) {
				locationCounter += 2;
				//opCode = string("71DS");
				opCode = string("71");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_MUL)) {
				locationCounter += 2;
				//opCode = string("72DS");
				opCode = string("72");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_DIV)) {
				locationCounter += 2;
				//opCode = string("73DS");
				opCode = string("73");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_CMP)) {
				locationCounter += 2;
				//opCode = string("74DS");
				opCode = string("74");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_NOT)) {
				locationCounter += 2;
				//opCode = string("80DS");
				opCode = string("80");
				callOneRegFunc(opCode, tokens[1], currentSection);

			}
			if (regex_match(tokens[0], regex_AND)) {
				locationCounter += 2;
				//opCode = string("81DS");
				opCode = string("81");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_OR)) {
				locationCounter += 2;
				//opCode = string("82DS");
				opCode = string("82");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_XOR)) {
				locationCounter += 2;
				//opCode = string("83DS");
				opCode = string("83");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_TEST)) {
				locationCounter += 2;
				//opCode = string("84DS");
				opCode = string("84");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_SHL)) {
				locationCounter += 2;
				//opCode = string("90DS");
				opCode = string("90");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_SHR)) {
				locationCounter += 2;
				//opCode = string("91DS");
				opCode = string("91");
				callTwoRegs(opCode, tokens[1], tokens[2], currentSection);

			}
			if (regex_match(tokens[0], regex_LDR)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				int num = 0;
				vector<string> tokensHelp = tokenizeComma(tokens[2]);
				locationCounter += 3;
				//opCode = string("A0DSUA");
				opCode = string("A0");
				string operand = tokens[2];
				string type = checkType(tokens[2]);
				string sym = removeSpecialCharacterReturn(tokens[2]);
				if (regex_match(tokens[2], regex_special_character)) {
					removeSpecialCharacter(tokens[2]);

				}
				bool flagBreak = false;
				cout << "Tokens[2] je : " << tokens[2] << endl;
				string global = this->symbolTable->getSymbolByName(sym).getLocal() == "global" ? "global" : "local";
				int ordinal;
				if (global == "global") {
					cout << "Tokens[2] je : " << tokens[2] << endl;
					ordinal = this->symbolTableSymbols->getSymbolByName(sym).getOrdinal() + this->symbolTableSections->getTableSize();
				}
				else {
					ordinal = this->symbolTable->getSymbolByName(sym).getSection();
				}

				if (regex_match(tokensHelp[0], regex_3B_data)) {

					num = 0;
				}
				else if (regex_match(tokensHelp[0], regex_5B_data)) {

					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}

				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {

					if (sym == *it ) {
						if (regex_match(operand, regex(regex_pc_rel))) {
							continue;
						}
						else {
							cout << "USAO OVDE ??? " << endl;
							flagBreak = true;
							break;
						}
					}
				}
				//cout << "FlagBreak : " << flagBreak;
				//cout << "Operand je : " << operand << " , a tokens[2] : " << tokens[2] << endl;

				if ((regex_match(operand, regex(regex_imm_lit))) || (regex_match(operand, regex(regex_reg_dir))) || (regex_match(operand, regex(regex_reg_ind))) || (regex_match(operand, regex(regex_mem_lit))) || (regex_match(operand, regex(regex_indirect_sum_lit)))) {
					flagBreak = true;
					//cout << "USAO U LOS IF ??? " << endl;
				}
				//cout << "FlagBreak1 : " << flagBreak << " ,a tokens[2]: " << tokens[2] << endl;

				if (flagBreak) {
					locationCounter += num;
					callOneRegOneOper(opCode, tokens[1], operand, currentSection, line);
					continue;
				}
				else {
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callOneRegOneOper(opCode, tokens[1], operand, currentSection, line);
				}
				
			}
			if (regex_match(tokens[0], regex_STR)) { //3 ili 5 bajtova u zavisnosti od adresiranja
				int num = 0;
				vector<string> tokensHelp = tokenizeComma(tokens[2]);
				locationCounter += 3;
				//opCode = string("B0DSUA");
				opCode = string("B0");
				string operand = tokens[2];
				string sym = removeSpecialCharacterReturn(tokens[2]);
				string type = checkType(tokens[2]);
				//if (regex_match(tokens[2], regex_special_character)) {
				//	removeSpecialCharacter(tokens[2]);

				//}
				bool flagBreak = false;
				
				string global = this->symbolTable->getSymbolByName(sym).getLocal() == "global" ? "global" : "local";
				cout << "Sym je : " << sym << " , a tokens[2] je : " << tokens[2]<< endl;
				cout << "Da li je globalan ili ne? " << this->symbolTable->getSymbolByName("myCounter").getName() << endl;
				int ordinal;
				if (global == "global") {
					cout << "Duzina stringa je : " << sym.size() << endl;
					ordinal = this->symbolTableSymbols->getSymbolByName(sym).getOrdinal() + this->symbolTableSections->getTableSize();
					cout << "Velicina section tabele : " << this->symbolTableSections->getTableSize() << endl;
					//cout << "Ordinal je : " << ordinal << " , a simbol je: "
				}
				else {
					ordinal = this->symbolTable->getSymbolByName(sym).getSection();
				}
				
				
				
				if (regex_match(tokensHelp[0], regex_3B_data)) {
					num = 0;
				}
				else if (regex_match(tokensHelp[0], regex_5B_data)) {
					num = 2;
				}
				else {
					cout << "Error - Bad format of operation ! Line : " << line << endl;
				}
				for (vector<string>::iterator it = inMemory.begin(); it != inMemory.end(); ++it) {
					if (sym == *it && (!regex_match(operand, regex(regex_pc_rel)))) {
						flagBreak = true;
						break;
					}
				}
				if ((regex_match(operand, regex(regex_imm_lit)))|| (regex_match(operand, regex(regex_reg_dir)))|| (regex_match(operand, regex(regex_reg_ind))) || (regex_match(operand, regex(regex_mem_lit)))|| (regex_match(operand, regex(regex_indirect_sum_lit)))) {
					flagBreak = true;
				}
				if (flagBreak) {
					locationCounter += num;
					callOneRegOneOper(opCode, tokens[1], operand, currentSection, line);
					continue;
				}
				else {
					listOfRelocationTables[currentSection].insertData(RelocationData(locationCounter, type, ordinal));//Dopuni za tip
					helpPairsContentTable.push_back(make_pair(line, locationCounter));
					locationCounter += num;
					callOneRegOneOper(opCode, tokens[1], operand, currentSection, line);
				}
				
			}
		}
	}
}

vector<string> Assembler::tokenize(string line) {
	vector <string> tokens;
	ltrim(line);

	// stringstream class check1
	stringstream check1(line);

	string intermediate;

	// Tokenizing w.r.t. space ' '
	while (getline(check1, intermediate, ' '))
	{
		tokens.push_back(intermediate);
	}

	// Printing the token vector
	/*for (int i = 0; i < tokens.size(); i++)
		cout << tokens[i] << '\n';*/

	return tokens;
}

vector<string> Assembler::tokenizeComma(string line) {
	vector <string> tokens;

	stringstream check1(line);

	//cout << "String je : " << check1.str() << endl;

	string intermediate;
	string symbol = "";

	while (getline(check1, intermediate, ','))
	{	
		//for (char c : intermediate) {
		//	string s(1, c);
		//	if (strcmp(s, string(","))) {

		//	}
		//}
		/*int p=intermediate.find(',');
		if (p != std::string::npos)
			intermediate.erase(p - 1);*/
		/*for (char c : intermediate) {
			intermediate.erase(std::remove(intermediate.begin(), intermediate.end(), c), intermediate.end());
		}*/
		intermediate.erase(std::remove(intermediate.begin(), intermediate.end(), ','), intermediate.end());
		cout <<"Simbol je : " << intermediate << endl;
		tokens.push_back(intermediate);
	}

	return tokens;
}

void Assembler::ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

void Assembler::callOneRegOneOper(string opCode, string regD, string operand, int currentSection, string line) {//ODRADI
	string finalString = "";
	vector<string> registerD = tokenizeComma(regD);
	string addrCode = "";
	bool flag_indirect_sum_lit = false;
	string payload = "";
	finalString.append(opCode);
	string finalRegD = "";
	if (registerD[0] == "psw") {
		finalRegD = "8";
	}
	else if (registerD[0] == "sp") {
		finalRegD = "6";
	}
	else if (registerD[0] == "pc") {
		finalRegD = "7";
	}
	else if (regex_match(registerD[0], regex_register)) {
		finalRegD = registerD[0][1];
	}
	if ((regex_match(operand, regex(regex_indirect_sum_lit)))) {
		cout << "USAO" << endl;
		flag_indirect_sum_lit = true;
		smatch reg;
		regex_search(operand, reg, regex(regex_indirect_sum_lit));
		for (auto m : reg)
			std::cout << "  submatch " << m << '\n';
		cout << "Reg je : " << reg[1] << " , a reg 3 je : " << reg[3] << endl;
		cout << "Reg str : " << reg[1] << endl;
		if (reg[1].str() == "psw") {
			finalRegD.append("8");
		}
		else if (reg[1].str() == "sp") {
			finalRegD.append("6");
		}
		else if (reg[1].str() == "pc") {
			finalRegD.append("7");
		}
		else if (regex_match(reg[3].str(), regex_register)) {
			for (char m : reg[3].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				finalRegD.append(s);
			}
			//finalRegD.append(reg[1].str());
		}
		cout << "Reg[3] : " << reg[3].str() << endl;
		string reg3 = reg[3].str();
		if (regex_match(reg[3].str(), regex_DEC_NUM)) {
			cout << "USAO U DEC" << endl;
			payload = reg[3].str();
		}
		else if (regex_match(reg[3].str(), regex_HEX_NUM)) {
			cout << "USAO U HEX " << reg3 << endl;
			smatch s1;
			regex_search(reg3, s1, regex_HEX_NUM);
			cout << "s1[1] : " << s1[2].str() << endl;
			payload = s1[2].str();

			/*for (char m : reg[3].str()) {
				if (m < 0 || m > 9) {
					continue;
				}
				string s(1, m);
				cout << "S je : " << s << endl;
				payload.append(s);
			}*/
		}
		cout << "Payload je : " << payload << endl;
	}
	else if ((regex_match(operand, regex(regex_indirect_sum_sym)))) {
		cout << "USAO" << endl;
		smatch reg;
		regex_search(operand, reg, regex(regex_indirect_sum_sym));
		for (auto m : reg)
			std::cout << "  submatch " << m << '\n';
		cout << "Reg je : " << reg[1] << " , a reg 3 je : " << reg[3] << endl;
		cout << "Reg str : " << reg[1] << endl;
		if (reg[1].str() == "psw") {
			finalRegD.append("8");
		}
		else if (reg[1].str() == "sp") {
			finalRegD.append("6");
		}
		else if (reg[1].str() == "pc") {
			finalRegD.append("7");
		}
		else if (regex_match(reg[3].str(), regex_register)) {
			for (char m : reg[3].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				finalRegD.append(s);
			}
			//finalRegD.append(reg[1].str());
		}
	}
	else if (regex_match(operand, regex(regex_reg_dir))) {
		smatch reg;
		regex_search(operand, reg, regex(regex_reg_dir));
		if (reg[1].str() == "psw") {
			finalRegD.append("8");
		}
		else if (reg[1].str() == "sp") {
			finalRegD.append("6");
		}
		else if (reg[1].str() == "pc") {
			finalRegD.append("7");
		}
		else if (regex_match(reg[1].str(), regex_register)) {
			for (char m : reg[1].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				finalRegD.append(s);
			}
		}
	}
	else if (regex_match(operand, regex(regex_reg_ind))) {
		smatch reg;
		regex_search(operand, reg, regex(regex_reg_ind));
		if (reg[1].str() == "psw") {
			finalRegD.append("8");
		}
		else if (reg[1].str() == "sp") {
			finalRegD.append("6");
		}
		else if (reg[1].str() == "pc") {
			finalRegD.append("7");
		}
		else if (regex_match(reg[1].str(), regex_register)) {
			for (char m : reg[1].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				finalRegD.append(s);
			}
		}
	}
	else if (regex_match(operand, regex(regex_pc_rel))) {
		finalRegD.append("7");
	}
	else {
		finalRegD.append("F");
	}
	cout << "FinalRegD je : " << finalRegD << endl;
	finalString.append(finalRegD);
	if (!flag_indirect_sum_lit){
		addrCode = checkAddrModeDATA(operand, line, currentSection);
	}
	else {
		addrCode.append("03");
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << payload;
		addrCode.append(dataStream.str());
	}
	finalString.append(addrCode);
	cout << "CallOneRegOneOper: " << finalString << "Linija: " << line<< endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callTwoRegs(string opCode, string regD, string regS, int currentSection){//OKEJ
	string finalString = "";
	string finalRegD = "";
	string finalRegS = "";
	vector<string> registerD = tokenizeComma(regD);
	finalString.append(opCode);
	if (registerD[0] == "psw") {
		finalRegD = "8";
	}
	else if (registerD[0] == "sp") {
		finalRegD = "6";
	}
	else if (registerD[0] == "pc") {
		finalRegD = "7";
	}
	else if (regex_match(registerD[0], regex_register)) {
		finalRegD = registerD[0][1];
	}
	if (regS == "psw") {
		finalRegS = "8";
	}
	else if (regS == "sp") {
		finalRegS = "6";
	}
	else if (regS == "pc") {
		finalRegS = "7";
	}
	else if (regex_match(regS, regex_register)) {
		finalRegS = regS[1];
	}

	finalString.append(finalRegD);
	finalString.append(finalRegS);
	cout << "CallTwoRegs : " << finalString << "Linija: " << endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callOneRegFunc(string opCode, string regD, int currentSection){//OKEJ
	string finalString = "";
	string finalRegD = "";
	finalString.append(opCode);
	if (regD == "psw") {
		finalRegD = "8";
	}
	else if (regD == "sp") {
		finalRegD = "6";
	}
	else if (regD == "pc") {
		finalRegD = "7";
	}
	else if (regex_match(regD, regex_register)) {
		finalRegD = regD[1];
	}
	finalRegD.append("F");
	finalString.append(finalRegD);
	cout <<"CallOneRegFunc: " << finalString << endl;
	
	//cout << "Finalni sadrzaj : " << finalString << endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callPushPop(string opCode, string addrCode, string regD, int currentSection){//OKEJ
	string finalString = "";
	string finalRegD = "";
	finalString.append(opCode);
	if (regD == "psw") {
		finalRegD = "8";
	}
	else if (regD == "sp") {
		finalRegD = "6";
	}
	else if (regD == "pc") {
		finalRegD = "7";
	}
	else if (regex_match(regD, regex_register)) {
		finalRegD = regD[1];
	}
	finalRegD.append("6");
	finalString.append(finalRegD);
	finalString.append(addrCode);
	cout << "CallPushPop: " << finalString << endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callSingleOperand(string opCode, string operand, int currentSection, string line){//ODRADI
	string finalString = "";
	finalString.append(opCode);
	bool flag_indirect_sum_lit = false;
	string addrCode = "";
	string payload = "";
	string regCode = "";
	if ((regex_match(operand, regex(regex_indirect_sum_lit_jmp)))) {
		flag_indirect_sum_lit = true;
		smatch reg;
		regex_match(operand, reg, regex(regex_indirect_sum_lit_jmp));
		if (reg[1].str() == "psw") {
			regCode.append("8");
		}
		else if (reg[1].str() == "sp") {
			regCode.append("6");
		}
		else if (reg[1].str() == "pc") {
			regCode.append("7");
		}
		else if (regex_match(reg[3].str(), regex_register)) {
			for (char m : reg[3].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				regCode.append(s);
			}
		}
		if (regex_match(reg[3].str(), regex_DEC_NUM)) {
			cout << "USAO U DEC" << endl;
			payload = reg[3].str();
		}
		else if (regex_match(reg[3].str(), regex_HEX_NUM)) {
			cout << "USAO U HEX" << endl;
			string reg3 = reg[3].str();
			smatch s1;
			regex_search(reg3, s1, regex_HEX_NUM);
			cout << "s1[1] : " << s1[2].str() << endl;
			payload = s1[2].str();
		}
	}
	if ((regex_match(operand, regex(regex_indirect_sum_sym_jmp)))) {
		cout << "USAO" << endl;
		smatch reg;
		regex_search(operand, reg, regex(regex_indirect_sum_sym_jmp));
		for (auto m : reg)
			std::cout << "  submatch " << m << '\n';
		cout << "Reg je : " << reg[1] << " , a reg 3 je : " << reg[3] << endl;
		cout << "Reg str : " << reg[1] << endl;
		if (reg[1].str() == "psw") {
			regCode.append("8");
		}
		else if (reg[1].str() == "sp") {
			regCode.append("6");
		}
		else if (reg[1].str() == "pc") {
			regCode.append("7");
		}
		else if (regex_match(reg[3].str(), regex_register)) {
			for (char m : reg[3].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				regCode.append(s);
			}
			//finalRegD.append(reg[1].str());
		}
	}
	else if (regex_match(operand, regex(regex_reg_dir_jmp))) {
		smatch reg;
		regex_search(operand, reg, regex(regex_reg_dir_jmp));
		if (reg[1].str() == "psw") {
			regCode.append("8");
		}
		else if (reg[1].str() == "sp") {
			regCode.append("6");
		}
		else if (reg[1].str() == "pc") {
			regCode.append("7");
		}
		else if (regex_match(reg[1].str(), regex_register)) {
			for (char m : reg[1].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				regCode.append(s);
			}
		}
	}
	else if (regex_match(operand, regex(regex_reg_ind_jmp))) {
		smatch reg;
		regex_search(operand, reg, regex(regex_reg_ind_jmp));
		if (reg[1].str() == "psw") {
			regCode.append("8");
		}
		else if (reg[1].str() == "sp") {
			regCode.append("6");
		}
		else if (reg[1].str() == "pc") {
			regCode.append("7");
		}
		else if (regex_match(reg[1].str(), regex_register)) {
			for (char m : reg[1].str()) {
				if (m == 'r') {
					continue;
				}
				string s(1, m);
				regCode.append(s);
			}
		}
	}
	else if (regex_match(operand, regex(regex_pc_imm_jmp))) {
		regCode.append("7");
	}
	else {
		regCode.append("F");
	}
	regCode.append("F");
	finalString.append(regCode);
	if (!flag_indirect_sum_lit) {
		cout << "Operand pre addr-a je : " << operand << endl;
		addrCode = checkAddrModeJMP(operand, line, currentSection);
		cout << "AddrCode je : " << addrCode << endl;
	}
	else {
		addrCode.append("03");
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << payload;
		addrCode.append(dataStream.str());
	}
	
	finalString.append(addrCode);
	cout << "CallSingleOperand: " << finalString << "Linija: " << line<< endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callNoOpCode(string data, int currentSection){//OKEJ
	string finalString = "";
	short num = 0;
	if (regex_match(data, regex_DEC_NUM)) {
		num = stoi(data, 0);
	}
	else if (regex_match(data, regex_HEX_NUM)) {
		num = stoi(data, 0, 16);
	}
	else {
		if (this->symbolTable->getSymbolByName(data).getLocal() == "global") {
			num = 0;
		}
		else {
			num = this->symbolTable->getSymbolByName(data).getValue();
		}
		
	}
	std::stringstream ss;
	ss << std::hex << num;

	if (ss.str().length() % 2 != 0) {
		finalString.append("0");
	}
	finalString.append(ss.str());
	if (finalString.length() % 4 != 0) {
		finalString.append("00");
	}
	cout << "CallNoOpCode: " <<finalString << endl;


	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

void Assembler::callSkipOper(string data, int currentSection){//OKEJ
	string finalString = "";
	int num = 0;
	if (regex_match(data, regex_DEC_NUM)) {
		num = stoi(data);
	}
	else if (regex_match(data, regex_HEX_NUM)) {
	num = stoi(data, 0, 16);
	}
	for (int i = 0; i < num; i++) {
		finalString.append("00");
	}
	//cout << "Final string: " << finalString << endl;
	listOfSectionTables[currentSection].setCode(listOfSectionTables[currentSection].getCode() + finalString);
}

string Assembler::checkType(string data) {
	string relocType;
	if ((regex_match(data, regex(regex_imm_lit))) || (regex_match(data, regex(regex_imm_sym))) || (regex_match(data, regex(regex_imm_lit_jmp))) || (regex_match(data, regex(regex_imm_sym_jmp)))) {
		relocType = "R_386_16";
	}
	else if ((regex_match(data, regex(regex_reg_dir))) || (regex_match(data, regex(regex_reg_dir_jmp)))) {
		relocType = "direktno";
	}
	else if ((regex_match(data, regex(regex_reg_ind))) || (regex_match(data, regex(regex_reg_ind_jmp)))) {
		relocType = "indirektno";
	}
	else if ((regex_match(data, regex(regex_pc_rel))) || (regex_match(data, regex(regex_pc_imm_jmp)))) {
		relocType = "R_386_PC16";
	}
	else if ((regex_match(data, regex(regex_indirect_sum_lit))) || (regex_match(data, regex(regex_indirect_sum_sym))) || (regex_match(data, regex(regex_indirect_sum_lit_jmp))) || (regex_match(data, regex(regex_indirect_sum_sym_jmp)))) {
		relocType = "indirektno sa pomerajem";
	}
	else if ((regex_match(data, regex(regex_mem_lit))) || (regex_match(data, regex(regex_mem_sym))) || (regex_match(data, regex(regex_mem_lit_jmp))) || (regex_match(data, regex(regex_mem_sym_jmp)))) {
		relocType = "memorijsko";
	}
	return relocType;
}

void Assembler::removeSpecialCharacter(string& s) {
	
	//for (int i = 0; i < s.size(); i++) {

		// Finding the character whose
		// ASCII value fall under this
		// range
	if (s[0] < 'A' || s[0] > 'Z' &&
		s[0] < 'a' || s[0] > 'z')
	{
		// erase function to erase
		// the character
		s.erase(0, 1);
		//i--;
	}
	s.erase(s.size()-1,1);
	//}
	//cout << "Izmenjeno ime je: " << s;
}

string Assembler::checkAddrModeJMP(string data, string line, int currentSection) {//POPRAVI
	cout << "Operand je : " << data << endl;
	string addrCode;
	if (regex_match(data, regex(regex_mem_lit_jmp))) {
		cout << "Usao u MEM " << line << endl;
		addrCode = "04";
		string dataHelp = removeSpecialCharacterReturn(data);
		//dataHelp.erase(dataHelp.size()-1,1);
		int num = 0;
		if (regex_match(dataHelp, regex_DEC_NUM)) {
			num = stoi(dataHelp, 0);
		}
		else if (regex_match(dataHelp, regex_HEX_NUM)) {
			num = stoi(dataHelp, 0, 16);
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_mem_sym_jmp)))) {
		cout << "Usao u MEM JMP ADDR" << line << endl;
		addrCode = "04";
		addrCode.append(appendTo5B(data, line, currentSection));
	}
	else if	((regex_match(data, regex(regex_imm_lit_jmp)))) {
		cout << "Usao u IMM " << line << endl;
		addrCode = "00";
		string dataHelp = removeSpecialCharacterReturn(data);
		dataHelp.erase(dataHelp.size()-1,1);
		int num = 0;
		if (regex_match(dataHelp, regex_DEC_NUM)) {
			num = stoi(dataHelp, 0);
		}
		else if (regex_match(dataHelp, regex_HEX_NUM)) {
			smatch s;
			regex_search(data, s, regex_HEX_NUM);
			string dataHelp = s[2].str();
			num = stoi(dataHelp, 0, 16);
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_imm_sym_jmp)))) {
		cout << "Usao u IMM JUMP" << line << endl;
		addrCode = "00";
		string dataHelp = removeSpecialCharacterReturn(data);
		//dataHelp.erase(dataHelp.size()-1,1);
		cout << "DataHelp je : " << dataHelp << " , a duzina je : " << dataHelp.size() << endl;
		int value = this->symbolTable->getSymbolByName(dataHelp).getValue();
		cout << "Vrednost simbola je : " << this->symbolTable->getSymbolByName("term_out").getValue() << endl;
		cout << "Value je : " << value << endl;
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << value;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_reg_dir_jmp)))) {
		addrCode = "01";
	}
	else if ((regex_match(data, regex(regex_reg_ind_jmp)))) {
		addrCode = "02";
	}
	else if ((regex_match(data, regex(regex_pc_imm_jmp)))) {
		addrCode = "03";
		string dataHelp = removeSpecialCharacterReturn(data);
		short value;
		if (this->symbolTable->getSymbolByName(dataHelp).getLocal() == "global") {
			value = 0 - 2;
		}
		else {
			value = this->symbolTable->getSymbolByName(dataHelp).getValue() - 2;
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << value;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_indirect_sum_sym_jmp)))) {
		addrCode = "03";
		addrCode.append(appendTo5B(data, line, currentSection));
	}
	else if(regex_match(data, regex(regex_indirect_sum_lit_jmp))){
		cout << "Usao u IND_SUM " << line << endl;
		addrCode = "03";
		string dataHelp = removeSpecialCharacterReturn(data);
		int num = 0;
		smatch offset;
		regex_search(data, offset, regex_indirect_sum_offset);
		if (regex_match(offset.str(), regex_DEC_NUM)) {
			num = stoi(offset.str(), 0);
		}
		else if (regex_match(offset.str(), regex_HEX_NUM)) {
			num = stoi(offset.str(), 0, 16);
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}
	

	return addrCode;
}

string Assembler::checkAddrModeDATA(string data, string line, int currentSection) { //POPRAVI
	cout << "Operand je : " << data << endl;
	string addrCode;
	if (regex_match(data, regex(regex_mem_lit))) {
		cout << "Usao u MEM LIT" << line << endl;
		addrCode = "04";
		//string dataHelp = removeSpecialCharacterReturn(data);
		
		//cout << "DataHelp je : " << dataHelp << endl;
		int num = 0;
		if (regex_match(data, regex_DEC_NUM)) {
			num = stoi(data, 0);
			//num = data;
		}
		else if (regex_match(data, regex_HEX_NUM)) {
			cout << "Data je : " << data << endl;
			smatch s;
			regex_search(data, s, regex_HEX_NUM);
			string dataHelp = s[2].str();
			num = stoi(dataHelp, 0, 16);
			//num = dataHelp;
			cout << "DataHelp je : " << num << endl;
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}
	else if (regex_match(data, regex(regex_mem_sym))) {
		cout << "Usao u MEM SYM" << line << endl;
		addrCode = "04";
		addrCode.append(appendTo5B(data, line, currentSection));
	}
	else if ((regex_match(data, regex(regex_imm_lit)))) {
		cout << "Usao u IMM " << line << endl;
		addrCode = "00";
		string dataHelp = removeSpecialCharacterReturn(data);
		int num = 0;
		if (regex_match(dataHelp, regex_DEC_NUM)) {
			num = stoi(dataHelp, 0);
		}
		else if (regex_match(dataHelp, regex_HEX_NUM)) {
			num = stoi(dataHelp, 0, 16);
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_imm_sym)))) {
		cout << "Usao u IMM " << line << endl;
		addrCode = "00";
		string dataHelp = removeSpecialCharacterReturn(data);
		int value = this->symbolTable->getSymbolByName(dataHelp).getValue();
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << value;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_reg_dir)))) {
		addrCode = "01";
	}
	else if ((regex_match(data, regex(regex_reg_ind)))) {
		addrCode = "02";
	}
	else if ((regex_match(data, regex(regex_pc_rel)))) {
		cout << "USAO U PC REL" << endl;
		addrCode = "03";
		string dataHelp = removeSpecialCharacterReturn(data);
		short value;
		if (this->symbolTable->getSymbolByName(dataHelp).getLocal() == "global") {
			value = 0 - 2;
		}
		else {
			value = this->symbolTable->getSymbolByName(dataHelp).getValue() - 2;
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << value;
		addrCode.append(dataStream.str());
	}
	else if ((regex_match(data, regex(regex_indirect_sum_sym)))) {
		addrCode = "03";
		addrCode.append(appendTo5B(data, line, currentSection));
	}
	else if (regex_match(data, regex(regex_indirect_sum_lit))) {
		cout << "Usao u IND_SUM " << line << endl;
		addrCode = "03";
		string dataHelp = removeSpecialCharacterReturn(data);
		int num = 0;
		smatch offset;
		regex_search(data, offset, regex_indirect_sum_offset);
		if (regex_match(offset.str(), regex_DEC_NUM)) {
			num = stoi(offset.str(), 0);
		}
		else if (regex_match(offset.str(), regex_HEX_NUM)) {
			num = stoi(offset.str(), 0, 16);
		}
		stringstream dataStream;
		dataStream << std::setfill('0') << std::setw(4) << std::hex << num;
		addrCode.append(dataStream.str());
	}


	return addrCode;
}

string Assembler::appendTo5B(string operand, string line, int currentSection) {
	string dataCode = "";
	string dataHelp = "";
	int value = 0;
	if (regex_match(operand, regex(regex_indirect_sum_sym))) {
		smatch s;
		regex_search(operand, s,regex(regex_indirect_sum_sym));
		dataHelp = s[3].str();
	}
	else if (regex_match(operand, regex(regex_indirect_sum_sym_jmp))) {
		smatch s;
		regex_search(operand, s, regex(regex_indirect_sum_sym_jmp));
		dataHelp = s[3].str();
	}
	else {
		dataHelp = removeSpecialCharacterReturn(operand);
	}
	/*Symbol* symbol = this->symbolTable->getSymbolByName(data).getSymbol();
	if (symbol->getSection() == 1) {
		if ()
	}*/
	if(this->symbolTable->getSymbolByName(dataHelp).getLocal() == "global"){
		value = 0;
	}
	else {
		value = this->symbolTable->getSymbolByName(dataHelp).getValue();
	}
	stringstream dataStream;
	dataStream << std::setfill('0') << std::setw(4) << std::hex << value;
	dataCode.append(dataStream.str());
	return dataCode;
}

string Assembler::removeSpecialCharacterReturn(string s) { //POPRAVI OVO
	//cout << "String je : " << s << endl;
	if (regex_match(s, regex(regex_indirect_sum_sym)) || (regex_match(s, regex(regex_indirect_sum_sym_jmp)))) {
		string ret = "";
		bool plusF = false;
		bool endF = false;
		for (int i = 0; i < s.size(); i++) {
			if (s[i] == '+') {
				plusF = true;
			}
			if (s[i] == ']') {
				endF = true;
			}
			if (plusF && !endF) {
				string m(1, s[i]);
				ret.append(m);
			}
		}
		//return ret;
		ret.erase(0, 1);
		cout << "Return Value je : " << ret << endl;
		return ret;
	}
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ') {
			s.erase(0, 1);
		}
	}
	if (s[0] < 'A' || s[0] > 'Z' &&
		s[0] < 'a' || s[0] > 'z')
	{

		s.erase(0, 1);
	}
	//cout << "String novi je : " << s << endl;
	//if(s[s.size()-1] == ' '){
		s.erase(s.size()-1,1);
	//}
	
	return s;
}

string Assembler::removeSpecialCharacterReturnSTR_LDR(string s) { //POPRAVI OVO
	//cout << "String je : " << s << endl;

	if (regex_match(s, regex(regex_indirect_sum_sym)) || regex_match(s, regex(regex_indirect_sum_sym_jmp))) {
		smatch sm;
		regex_search(s, sm, regex(regex_indirect_sum_sym));
		cout << "Sm.str() : " << sm[1].str() << endl;
	}
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ') {
			s.erase(0, 1);
		}
	}
	if (s[0] < 'A' || s[0] > 'Z' &&
		s[0] < 'a' || s[0] > 'z')
	{

		s.erase(0, 1);
	}
	//cout << "String novi je : " << s << endl;
	//if(s[s.size()-1] == ' '){
		//s.erase(s.size()-1,1);
	//}
	
	return s;
}