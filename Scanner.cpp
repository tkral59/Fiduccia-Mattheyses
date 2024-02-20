#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "DataStructures.hpp"
using namespace std;

void read(string netfile, string arefile, vector<Gate>& gates, vector<Net>& nets, int& netTot, int& nodeTot) {
	fstream myFile;
	myFile.open(netfile, ios::in); //out is write 
	if (myFile.is_open()) {
		string line;
		int iter = 0;
		while (getline(myFile, line)) {
			//cout << line << endl; //testing
			if (iter == 2) {
				netTot = stoi(line, 0, 10);
				iter++;
			}
			else if (iter == 3) {
				nodeTot = stoi(line, 0, 10);
				iter++;
			}
			else if ((iter == 0) || (iter == 1) || (iter == 4))
				iter++;
			else {

			}

		}
		myFile.close();
	}
	else {
		cout << "File failed to open.\n";
	}
}

int main() {
	vector<Gate> gs = {};
	vector<Net> ns = {};
	int nt = 0;
	int nodt = 0;
	read("ibm01.net", "", gs, ns, nt, nodt);
}

