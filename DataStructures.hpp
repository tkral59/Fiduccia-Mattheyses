#pragma once
#include <iostream>
#include <vector>
#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

struct Net; //forward declaration

class Gate {
private:
    std::string name;
    std::vector<Net*> nets;
    bool Part; // For now, will treat partition as a boolean
    int GainTot;
    int area;

public:
    Gate();
    Gate(std::string name, std::vector<Net*> nets, bool part, int gain, int area);
    ~Gate();
    std::vector<Net*> getNets();
    void addNet(Net* net);
    void removeNet(Net* net);
    void setPart();
    void unsetPart();
    bool getPart();
    int getGT();
    void incGT();
    void decGT();
    friend std::ostream& operator<<(std::ostream& os, Gate const& g);
    std::string getName();
    int getArea();
    void setArea(int a);

    // New methods for gain calculation
    void calculateInitialGain(); // Add this method prototype
    static void updateGainsAfterMove(Gate* movedGate); // Static method prototype
};

struct Net {
    std::string name;
    std::vector<Gate*> gates;
    int p1cnt;
    int p2cnt;
};

#endif // DATASTRUCTURES_HPP
