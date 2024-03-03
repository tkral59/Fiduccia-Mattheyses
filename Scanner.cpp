#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <random>
#include <set>
#include "DataStructures.hpp"
#include <sstream> // Add this at the top of Scanner.cpp


using namespace std;


void read(const string& netfile, const string& arefile, map<string, Gate>& gates, map<string, Net>& nets, int& netTot, int& nodeTot) {
    gates.clear(); // Clear existing gates
    nets.clear(); // Clear existing nets

    string line;
    //ifstream myFile(netfile);
    fstream myFile;
    myFile.open(netfile, ios::in);
    if (myFile.is_open()) {
        int iter = 0;
        int netcount = 0; // To keep track of nets
        string netname;
        while (getline(myFile, line)) {
            if (line.empty()) continue; // Skip empty lines
            if (iter == 2) {
                netTot = stoi(line);
                //nets.resize(netTot); // Resize nets vector according to the total number of nets
                iter++;
            }
            else if (iter == 3) {
                nodeTot = stoi(line);
                iter++;
            }
            else if (iter == 0 || iter == 1 || iter == 4) {
                iter++;
            }
            else {
                // Parsing line
                vector<string> words;
                string temp;
                stringstream ss(line);

                while (ss >> temp) {
                    words.push_back(temp);
                }

                if (words.size() < 2) continue; // Skip invalid lines

                if (words[1] == "s") {
                    // Start of a new net
                    Net newNet("n" + std::to_string(netcount));
                    //nets.at(netcount) = newNet;// Correctly create Net object
                    nets[newNet.name] = newNet;
                    netname = newNet.name;
                    netcount++;
                }

                if (gates.find(words[0]) == gates.end()) {
                    // Gate not found, create new
                    gates[words[0]] = Gate(words[0], {}, false, 0, 0, 0); // Assuming Gate constructor exists
                }
                // Assuming 'nets' is a vector of Net objects and 'gates' is a map with string keys and Gate objects
                //Net* currentNet = &nets[netcount - 1];
                Net* currentNet = &nets[netname];
                Gate* currentGate = &gates[words[0]];

                currentNet->gates.push_back(currentGate);
                currentGate->addNet(currentNet); // Assuming addNet is a public method to add a net to the gate

            }
        }
        myFile.close();
    }
    else {
        cerr << "Failed to open " << netfile << endl;
    }

    ifstream areFile(arefile);
    if (areFile.is_open()) {
        while (getline(areFile, line)) {
            stringstream ss(line);
            string gateName;
            int area;
            if (ss >> gateName >> area) {
                gates[gateName].setArea(area);
            }
        }
        areFile.close();
    }
    else {
        cerr << "Failed to open " << arefile << endl;
    }
}


void randInitParts(map<string, Gate>& gm, int gtotal, state& s, int& cutsize, map<string, Net>& netmap) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, gtotal - 1);
    set<int> chosen;
    int index;
    for (int i = 0; i < (gtotal / 2); i++) {
        do {
            index = dis(gen);
        } while (chosen.count(index) > 0);
        chosen.insert(index);
        auto it = std::next(gm.begin(), index);
        it->second.setPart(cutsize, netmap);
        s.P1.push_back((&it->second));
        s.A1 += it->second.getArea();
    }
    for (auto it = gm.begin(); it != gm.end(); it++) {
        if (chosen.count(distance(gm.begin(), it)) == 0) {
            s.P2.push_back(&it->second);
            s.A2 += it->second.getArea();
        }
    }
    s.cutsize = cutsize;
}



void initializePartitionSizes(state& s, const map<string, Gate>& gates) {
    s.A1 = s.A2 = 0;
    for (const auto& gatePair : gates) {
        const Gate& gate = gatePair.second;
        if (gate.getPart()) {
            s.A1 += gate.getArea();
        }
        else {
            s.A2 += gate.getArea();
        }
    }
    std::cout << "Initial Partition Sizes => P1: " << s.A1 << ", P2: " << s.A2 << std::endl;
}


void fillBuckets(state& s, std::map<std::string, Gate>& gates) {
    s.buckets.clear(); // Reset the buckets

    for (auto& gatePair : gates) {
        Gate& gate = gatePair.second;
        gate.calculateInitialGain(); // Make sure this updates GainTot correctly
        s.buckets[gate.getGT()].insert(&gate);
    }
}


// Assuming calculateCutSize function is declared somewhere
int calculateCutSize(const std::vector<Net>& nets) {
    int cutSize = 0;
    for (const auto& net : nets) {
        if (net.p1cnt > 0 && net.p2cnt > 0) {
            cutSize++;
        }
    }
    return cutSize;
}

bool canSwap(Gate* gate, const state& s) {
    // Simplified example: Ensure the swap doesn't result in one partition being twice as large as the other
    int gateArea = gate->getArea();
    if (gate->getPart()) { // Gate is in partition 1, moving to partition 2
        return (s.A2 + gateArea <= 2 * (s.A1 - gateArea));
    }
    else { // Gate is in partition 2, moving to partition 1
        return (s.A1 + gateArea <= 2 * (s.A2 - gateArea));
    }
}


bool canSwap(const Gate* gate, const state s, double thresh) {
    int gateArea = gate->getArea();
    double newA1 = s.A1, newA2 = s.A2;
    if (newA1 == newA2) { // have to allow some changes
        return true;
    }
    else if (gate->getPart() && newA1 < newA2) {
        return true;
    }
    else if (!(gate->getPart()) && newA1 > newA2) {
        return true;
    }
    else if (gate->getArea() == 0) {
        return true;
    }
    else {
        int flag = 0;
        int origdiff = 0;

        if (abs(newA1 - newA2) > (newA1 + newA2) * thresh) {
            flag = 1;
            origdiff = abs(newA1 - newA2);
        }

        if (gate->getPart()) { // Moving from Partition 1 to Partition 2
            newA1 -= gateArea;
            newA2 += gateArea;

        }
        else { // Moving from Partition 2 to Partition 1
            newA1 += gateArea;
            newA2 -= gateArea;
        }

        // Define a threshold for imbalance (e.g., one partition should not be more than X% larger than the other)
        int totalArea = newA1 + newA2;
        if (flag == 1) {
            if (abs(newA1 - newA2) < origdiff) {
                return true;
            }
        }
        else return std::abs(newA1 - newA2) <= totalArea * thresh;
    }

}

void updatePartitionSizes(Gate* gate, state& s) {
    // Update partition sizes based on the gate's area and current partition
    int gateArea = gate->getArea();
    if (!gate->getPart()) { // If gate is currently in partition 1, moving to partition 2
        s.A1 -= gateArea;
        s.A2 += gateArea;
        s.P1.erase(remove(s.P1.begin(), s.P1.end(), gate));
        s.P2.push_back(gate);
    }
    else { // If gate is currently in partition 2, moving to partition 1
        s.A1 += gateArea;
        s.A2 -= gateArea;
        s.P2.erase(remove(s.P2.begin(), s.P2.end(), gate));
        s.P1.push_back(gate);
    }
}


void unlockAllGates(map<string, Gate>& gates) {
    for (auto& gatePair : gates) {
        gatePair.second.unlock();
    }
}

int getCutSize(map<string, Net> netlist) {
    int cutsize = 0;
    for (auto pair : netlist) {
        if (pair.second.cut) cutsize++;
    }
    return cutsize;
}

state performFMAlgorithm(state& s, map<string, Gate>& gates, map<string, Net>& nets, double thresh) {
    state result;
    bool improvement = true;
    int mincut = s.cutsize;
    int originalCutsize = s.cutsize;
    int track = 0;
    int swapflag = 0;
    while (improvement) {
        improvement = false;
        // Correctly accessing reverse iterator for the map
        for (auto it = s.buckets.rbegin(); it != s.buckets.rend(); ++it) {
            if (it->second.empty()) {
                improvement = false;
                continue;
            }

            for (auto gatePtr : it->second) {
                // Correct pointer access for gate
                Gate* gate = gatePtr; // Directly using pointer to gate
                if (gate->getIsLocked()) {
                    continue;
                }
                // Now correctly using the public getter method
                if (canSwap(gate, s, thresh)) {
                    gate->togglePartitionAndRecalculateGain(s.cutsize, s, nets);

                    updatePartitionSizes(gate, s);
                    gate->lock(); // Assuming Lock() correctly modifies the gate's locked status

                    // It might be necessary to clear and refill buckets outside of this loop,
                    // depending on how your algorithm is supposed to work.
                    // fillBuckets(s, gates); // Consider moving this outside of the loop if necessary
                    track++;
                    cout << track << "/" << gates.size() << endl;
                    if (s.cutsize < mincut) {
                        mincut = s.cutsize;
                        result = s;
                        //result.cutsize = getCutSize(nets);

                    }
                    if (s.cutsize > originalCutsize) {
                        improvement = false;
                        break;
                    }
                    if (track == 500 || track == 1000 || track == 5000) {
                        cout << getCutSize(nets);
                    }

                    improvement = true;
                    break; // Exit the loop to start from the highest gain again
                }
                else {
                    cout << "couldnt swap" << endl;
                    swapflag = 1;
                    improvement = false;
                    continue;
                }

            }
            if (improvement) {
                //wapflag = 0;
                break;
            }
            else if (swapflag == 1) {
                continue;
            }
            else goto converged;
        }
    }
converged: unlockAllGates(gates); // Prepare for the next pass
    cout << "inner cutsize" << getCutSize(nets) << endl;
    return result;
}


void printVerificationOutput(const state& s, const map<string, Gate>& gates) {
    cout << "Partition 1 Size: " << s.A1 << endl;
    cout << "Partition 2 Size: " << s.A2 << endl;
    // Implement logic to calculate cutSize based on your project's requirements
    int cutSize = 0; // Placeholder for actual cut size calculation
    cout << "Cut Size: " << cutSize << endl;
}


void updateNets(map<string, Net>& netmap, map<string, Gate> gates) {
    for (auto pair : gates) {
        for (Net* net : pair.second.getNets()) {
            Net dummybef = netmap[net->name];
            netmap[net->name] = *net;
            Net dummy = netmap[net->name];
        }
    }
}

int main() {
    state currentState;
    int maxAreaDiff = 10000;
    std::map<string, Net> nets;
    std::string netfile = "ibm01.net";  // Adjust path as necessary
    std::string arefile = "ibm01.are";  // Adjust path as necessary
    double thresh = 0.;
    std::map<std::string, Gate> gates;
    int netTot = 0, nodeTot = 0;
    int cutsize = 0;

    // Step 1: Load the nets and gates data from files
    read(netfile, arefile, gates, nets, netTot, nodeTot);
    std::cout << "Files read successfully. Total gates: " << gates.size() << ", Total nets: " << nets.size() << std::endl;

    std::cout << "Calculating initial cut size..." << std::endl;
    //int initialCutSize = calculateCutSize(nets);

    // Add debugging output to check if the calculateCutSize function is called and the value of initialCutSize
    //std::cout << "Initial cut size calculated: " << initialCutSize << std::endl;

    // Output initial cut size
    //std::cout << "Initial Cut Size: " << initialCutSize << std::endl;


    // Step 2: Randomly initialize partitions for gates
    randInitParts(gates, gates.size(), currentState, cutsize, nets);
    int initialCutSize = currentState.cutsize;
    // Step 3: Initialize the state object for the partitioning algorithm
    //initializePartitionSizes(currentState, gates);
    std::cout << "Initial Partition Sizes => P1: " << currentState.A1 << ", P2: " << currentState.A2 << std::endl;
    fillBuckets(currentState, gates);
    // Step 4: Perform the FM partitioning algorithm
    map<string, Net> checkpoint = nets;
    state result = performFMAlgorithm(currentState, gates, nets, thresh);
    for (auto pair : nets) {
        if (checkpoint[pair.second.name].p1cnt != pair.second.p1cnt) {
            cout << "change" << endl;
            break;
        }
    }
    //updateNets(nets, gates);
    //test(nets);
    int cuts = getCutSize(nets);
    cout << "Rresults 1: " << result.cutsize << " where intital was " << initialCutSize << endl;
    cout << "New cutsize" << cuts << endl;
    vector<state> results;
    result.cutsize = getCutSize(nets);
    results.push_back(result);
    fillBuckets(currentState, gates);
    result = performFMAlgorithm(currentState, gates, nets, thresh);
    cout << "Result 2: " << result.cutsize << endl;
    result.cutsize = getCutSize(nets);
    int index = 1;
    results.push_back(result);
    while (results.at(index).cutsize < results.at(index - 1).cutsize) {
        fillBuckets(currentState, gates);
        result = performFMAlgorithm(currentState, gates, nets, thresh);
        result.cutsize = getCutSize(nets);
        results.push_back(result);
        index++;
        cout << "Result " << index + 1 << ": " << result.cutsize << endl;
    }
    // Step 5: Output some results for verification
    //printVerificationOutput(result, gates);

    // After performing the FM algorithm
    //int finalCutSize = calculateCutSize(nets);
    cout << "Initial Cut Size: " << initialCutSize << endl;
    std::cout << "Final Cut Size: " << result.cutsize << std::endl;

    // Additional Step: Debugging or detailed output (if necessary)
    // Consider iterating through gates and nets to print detailed partitioning results

    return 0;
}