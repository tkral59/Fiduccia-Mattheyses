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
    int lock = 0;

public:
    Gate();
    Gate(std::string name, std::vector<Net*> nets, bool part, int gain, int area, int lock);
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
    void Lock();
    void unlock();

    // New methods for gain calculation
    void calculateInitialGain(); // Add this method prototype
    static void updateGainsAfterMove(Gate* movedGate); // Static method prototype
};

struct Net {
    std::string name;
    std::vector<Gate*> gates;
    int p1cnt;
    int p2cnt;
    bool cut;
};

struct state {
    std::vector<Gate*> P1; //partition 1 list
    std::vector<Gate*> P2;
    int cutsize;
    int A1; //area 1
    int A2; //area 2
};

double getCost(std::vector<Net> nets, std::map<std::string, Gate> gates, double& cutsize, double& AreaProduct);
#endif // DATASTRUCTURES_HPP
