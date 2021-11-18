#include "RelocationData.h"

RelocationData::RelocationData(int value, string type, int ordinal) {
	this->value = value;
	this->type = type;
	this->ordinal = ordinal;
}

int RelocationData::getValue() {
	return this->value;
}

void RelocationData::setValue(int value) {
	this->value = value;
}

string RelocationData::getType() {
	return this->type;
}

void RelocationData::setType(string type) {
	this->type = type;
}

int RelocationData::getOrdinal() {
	return this->ordinal;
}

void RelocationData::setOrdinal(int ordinal) {
	this->ordinal = ordinal;
}