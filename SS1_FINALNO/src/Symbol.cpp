#include "Symbol.h"

Symbol::Symbol(int ordinal, string name, int section, int value, string local, int size/*, string accesss*/) {
	this->ordinal = ordinal;
	this->name = name;
	this->section = section;
	this->value = value;
	this->local = local;
	this->size = size;
	//this->access = access;
}

int Symbol::getOrdinal() {
	return this->ordinal;
}

void Symbol::setOrdinal(int ordinal) {
	this->ordinal = ordinal;
}

string Symbol::getName() {
	return this->name;
}

void Symbol::setName(string name) {
	this->name = name;
}

int Symbol::getSection() {
	return this->section;
}

void Symbol::setSection(int section) {
	this->section = section;
}

int Symbol::getValue() {
	return this->value;
}

void Symbol::setValue(int value) {
	this->value = value;
}

string Symbol::getLocal() {
	return this->local;
}

void Symbol::setLocal(string local) {
	this->local = local;
}

int Symbol::getSize() {
	return this->size;
}

void Symbol::setSize(int ssize) {
	this->size = ssize;
	cout << "Vrednost velicine je : " << this->size << endl;
}

//string Symbol::getAccess() {
//	return this->access;
//}
//
//void Symbol::setAccess(string access) {
//	this->access = access;
//}

Symbol* Symbol::getSymbol() {
	return new Symbol(this->ordinal, this->name, this->section, this->value, this->local, this->size);
}
