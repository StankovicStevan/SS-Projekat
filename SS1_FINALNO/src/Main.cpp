#include <iostream>
#include "Assembler.h"

using namespace std;

int main(int argc, char** argv) { //PROMENI INDEKSE ARGUMENATA
    string inputFile, outputFile;

    if (argc < 4) {
        cout << "Los format komande, komanda bi trebalo da bude u formatu asembler -o outputfile inputfile" << endl;
        return -1;
    }
    for (int i = 0; i < argc; i++) {
        cout << i << "-ti parametar je " << argv[i] << endl;
    }
    if (string(argv[1]) == "-o") {
        inputFile = string(argv[3]);
        outputFile = string(argv[2]);
        cout << "Output fajl je: " << outputFile << endl;
        try {
            Assembler* assembler = new Assembler(inputFile, outputFile);
            cout << "Okej je format" << endl;
            int c;
            cin >> c;
            return 1;
        }
        catch (string s) {
            cout << "Izuzetak : " << s << endl;
            return -2;
        }
    }
    else {
        cout << "Izabrana je losa opcija komandne linije" << endl;
        int c;
        cin >> c;
        return 0;
    }
}