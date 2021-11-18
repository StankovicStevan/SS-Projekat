#include "RelocationTable.h"

#include <iomanip>
#include <sstream>
#include <fstream>

RelocationTable::RelocationTable(string section) {
	this->section = section;
	this->ordinal = 0;
}

void RelocationTable::insertData(RelocationData data) {
	relocationTable.push_back(data);
}

void RelocationTable::printTable(ofstream& outfile) {
	stringstream ss;
	ss << "#.relocation data : " << section << endl;
	ss << setw(15) << "Offset" << setw(15) << "Type" << setw(15) << "Ordinal" << endl;
	for (vector<RelocationData>::iterator it = relocationTable.begin(); it != relocationTable.end(); ++it) {
		ss << setw(15) << std::hex<< it->getValue() << setw(15) << it->getType() << setw(15) << it->getOrdinal() << endl;
	}
	cout << ss.str();
	outfile << ss.str();
}

string RelocationTable::getSectionName() {
	return this->section;
}

RelocationTable* RelocationTable::getRelocationTable() {
	return this;
}

void RelocationTable::setRelocationData(RelocationData data) {
	this->relocationTable[this->ordinal].setOrdinal(data.getOrdinal());
	this->relocationTable[this->ordinal].setType(data.getType());
	this->relocationTable[this->ordinal].setValue(data.getValue());
	this->ordinal++;
}