#pragma once
#include <vector>
#include <numeric>

// KLASA SKRZYNIA BIEGÓW

class Gearbox {
private:
    int currentGear = 0; // 0 - luz, -1 - wsteczny, 1-5 biegi
    const int maxGears = 5;
    // Przełożenia biegów jakies losowe
    std::vector<float> ratios = {0.0f, 0.20f, 0.45f, 0.7f, 0.95f, 1.25f}; 

public:
    void shiftUp() { if (currentGear < maxGears) currentGear++; }
    void shiftDown() { if (currentGear > -1) currentGear--; }

    // Metoda do resetowania skrzyni na luz 
    void setNeutral() { currentGear = 0; }
    
    int getGear() const { return currentGear; }
    float getRatio() const {
        if (currentGear == -1) return 0.1f; // wsteczny
        return ratios[currentGear];
    }
};

