#include "SectionTable.h"

#include <iomanip>
#include <sstream>
#include <fstream>

SectionTable::SectionTable(string code, string section) {
	this->code = code;
	this->section = section;
}

string SectionTable::getCode() {
	return this->code;
}

void SectionTable::setCode(string code) {
	this->code = code;
}

string SectionTable::getSection() {
	return this->section;
}

void SectionTable::setSection(string section) {
	this->section = section;
}

void SectionTable::addCode(string code) {
	this->code.append(code);
}

void SectionTable::printTable(ofstream& outfile) {
	stringstream ss;
	ss << "#." << section << endl;
	string output;
	for (int i = 0; i < (int) this -> code.length(); i++) {
		if (i == 0) {
			ss << this->code[i];
			continue;
		}
		if (i % 16 == 0) {
			ss << endl;
		}
		else if (i % 2 == 0 && i % 16 != 0) {
			ss << " ";
		}
		ss << this -> code[i];
		
	}
	//for (int i = 0; i < (int)this->code.length();) { //Proveri da li valja
	//	output += this->code.substr(i, 1) + " ";
	//	i += 2;
	//}
	//for (int i = 0; i < (int)output.length(); i++) {
	//	if ((i % 29) == 0) {
	//		cout << i << endl;
	//	}
	//	else {
	//		cout << i;
	//	}
	//}
	ss << endl;
	cout << ss.str();
	outfile << ss.str();
}