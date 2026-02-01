#pragma once
#include <iostream>
#include <stdexcept>
#include <string>
#include "Exceptions.hpp"


class Engine {
private:
    bool running = false;
    float fuelLevel = 20.0f; // litry w zbiorniku na poczatku działania silnika
    float oilLevel = 1.0f;  // 0.0 do 1.0 (procent sprawności)
    int rpm = 0;

public:
    void start() {
        if (fuelLevel <= 0) throw OutOfFuelException();
        if (oilLevel <= 0.1) throw EngineSeizedException();
        running = true;
        rpm = 800; // Obroty jałowe
    }

    void stop() {
        running = false;
        rpm = 0;
    }

    void update(int load) {
        if (!running) return;

        // Zużycie zasobów w zależności od obciążenia (load)
        fuelLevel -= 0.01f * (load / 1000.0f + 1.0f);
        oilLevel -= 0.0001f; // Powolne zużycie oleju

        if (fuelLevel <= 0) {
            fuelLevel = 0;
            stop();
            throw OutOfFuelException();
        }

        if (oilLevel <= 0) {
            stop();
            throw EngineSeizedException();
        }
    }

    // Gettery potrzebne do wyświetlania parametrów
    bool isRunning() const { return running; }
    float getFuel() const { return fuelLevel; }
    int getRPM() const { return rpm; }
    void setRPM(int val) { if (running) rpm = val; }
};