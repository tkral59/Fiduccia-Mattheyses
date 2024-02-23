#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "DataStructures.hpp"
using namespace std;

void read(string netfile, string arefile, map<string, Gate>& gates, vector<Net>& nets, int& netTot, int& nodeTot) {
	if (!nets.empty())
		nets.clear();
	if (!gates.empty())
		gates.clear();
	fstream myFile;
	myFile.open(netfile, ios::in); //out is write 
	if (myFile.is_open()) {
		string line;
		int iter = 0;
		int netcount = 0;//index of what net we're on
		Net nnet;
		Gate ngate;
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
				string add;
				vector<string> words;
				for (string::const_iterator i = line.begin(); i != line.end(); i++) {
					if (*i == ' ') { //if there is a space
						words.push_back(add);
						add.clear();
					}
					else {
						add += *i;
					}
				}
				if (!add.empty()) {
					words.push_back(add);
					add.clear();
				}
				
				//we know what net we are on based on netcount
				
				if (words.at(1) == "s") {//if we're starting a new net
					nnet = { "n" + to_string(netcount), {}, 0, 0 };//creates new empty net struct
					nets.push_back(nnet);
					netcount++;
				}
				else
					nnet = nets.at(nets.size() - 1); //else use the same net previously created

				//then add gate to gate map if needed, add net to gates net* list
				
				if (gates.count(words.at(0)) != 1) {//if gate is not in map
					ngate = Gate(words.at(0), {}, false, 0); //all gates will start with an empty net* vect, be in part 2, have gain of 0
					gates.insert(pair<string, Gate>(words.at(0), ngate)); //add new gate to map
				}
				else
					ngate = gates[words.at(0)];
				//add gate to nets gate list, and vice versa
				gates[words.at(0)].addNet(&nets.at(netcount - 1));
				nets.at(netcount - 1).gates.push_back(&gates[words.at(0)]);
			}

		}
		myFile.close();
	}
	else {
		cout << "File failed to open.\n";
	}
}

int main() {
	map<string, Gate> gm;
	vector<Net> ns = {};
	int nt = 0;
	int nodt = 0;
	read("ibm01.net", "", gm, ns, nt, nodt);
	cout << "Map of Gates:" << endl;
	for (const auto& myPair : gm) {
		cout << "gm[" << myPair.first << "]: " << endl; //<< myPair.second << endl;
	}
	for (int i = 0; i < ns.size(); i++) {
		cout << ns.at(i).name;
		for (int j = 0; j < ns.at(i).gates.size(); j++) {
			string p = ns.at(i).gates.at(j)->getName();
			cout << " " << p << " ";
		}
		cout << endl;
	}
		
}
