#include "SymbolTable.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;


SymbolTable::SymbolTable() {
	
}

void SymbolTable::insertSymbol(Symbol symbol){
	symbolTable.push_back(symbol);
}

vector<Symbol> SymbolTable::getSymbolTable() {
	return symbolTable;
}

bool SymbolTable::checkSymbolExistence(string name) {
	bool flagExistence = false;
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getName() == name) {
			flagExistence = true;
			break;
		}
	}
	if (!flagExistence) {
		return false;
	}
	else {
		return true;
	}
}

Symbol SymbolTable::getSectionlByNum(int sectionNum) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getSection() == sectionNum) {
			return *it;
		}
	}
	return Symbol(0, string("Error"), 0, 0, string("local"), 0);
}

bool SymbolTable::sectionExistence(string name, int& sectionNum, int& size) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getName() == name) {
			sectionNum = it->getSection();
			size = it->getSize();
			return true;
		}
	}
	return false;
}

void SymbolTable::printSymbolTable(ofstream &outfile) {
	/*cout << "Ordinal" << setw(9) << " | " << setw(15) << "Name" << setw(15) << " | " << setw(15) << "Section" << setw(15) << " | " << setw(15) << "Value" 
	<< setw(15) << " | " << setw(15) << "Local" << setw(15) << " | " << setw(15) << "Size" << endl;*/
	stringstream ss;
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		outfile << setw(15) << it->getOrdinal() << setw(15) << it->getName() << setw(15)<< it->getSection() << setw(15)
			<< hex << it->getValue() << setw(15) << it->getLocal() << setw(15)<< it->getSize() << endl;
	}
	//cout << ss.str();
	//outfile << ss.str();
}

void SymbolTable::printSymbolTable(int offset, ofstream &outfile) {
	/*cout << "Ordinal" << setw(9) << " | " << setw(15) << "Name" << setw(15) << " | " << setw(15) << "Section" << setw(15) << " | " << setw(15) << "Value"
		<< setw(15) << " | " << setw(15) << "Local" << setw(15) << " | " << setw(15) << "Size" << endl;*/
	stringstream ss;
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		outfile << setw(15) << std::to_string(it->getOrdinal() + offset) << setw(15)  << it->getName() << setw(15) << it->getSection() << setw(15)
			<< hex << it->getValue() << setw(15) << it->getLocal() << setw(15) << setw(15) << it->getSize() << endl;
	}
	//cout << ss.str();
	//outfile << ss.str();
}

void SymbolTable::setSectionSize(int section, int size) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getOrdinal() == section) {
			//cout << "Usao u if, sekciji : " << section << " postavlja velicinu : " << size << endl;
			it->setSize(size);
		}
	}
}

void SymbolTable::setSymbolVisibility(string symbol, string visibility) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getName() == symbol) {
			//cout << "Usao u if, sekciji : " << symbol << " postavlja vidljivost : " << visibility << endl;
			it->setLocal(visibility);
		}
	}
}

bool SymbolTable::symbolExistence(string name, int& symbolNum) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		//cout << "Ime simbola je : " << name << ", a ime iteracije je : " << it->getName() << endl;
		if (it->getName() == name) {
			//cout << "Usao u if iteratora " << endl;
			symbolNum = it->getOrdinal();
			return true;
		}
	}
	return false;
}

Symbol SymbolTable::getSymbolByNum(int ordinal) {
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getOrdinal() == ordinal) {
			return *it;
		}
	}
	return Symbol(0, string("Error"), 0, 0, string("local"), 0);
}

Symbol SymbolTable::getSymbolByName(string name) {
	cout << "Duzina stringa je : " << name.size() << endl;
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		if (it->getName() == name) {
			return *it;
		}
	}
	return Symbol(0, string("Error"), 0, 0, string("local"), 0);
}

int SymbolTable::getTableSize() {
	int i = 0;
	for (vector<Symbol>::iterator it = symbolTable.begin(); it != symbolTable.end(); ++it) {
		i++;
	}
	return i;
}