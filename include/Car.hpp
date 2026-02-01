#pragma once
#include "Engine.hpp"
#include "Gearbox.hpp"
#include <iostream>
#include <string>   
#include <vector>   
#include <ranges>  
#include <algorithm>

// KLASA SAMOCHÓD (Główny plyk)

class Car {
private:
    Engine engine;
    Gearbox gearbox;
    float speed = 0.0f;
    std::vector<float> speedHistory; // Logi prędkości

public:
    void startCar() { engine.start(); }
    // Resetowanie biegu przy zatrzymaniu lub awarii
    void stopCar() { 
        engine.stop(); 
        speed = 0; 
        gearbox.setNeutral(); // Ustawia bieg na luz po zgaszeniu
    }
    ///////////////////////////////////////////////////////
    // Gettery dla interfejsu graficznego (FTXUI)
    float getSpeedVal() const { return speed; }
    float getFuelVal() const { return engine.getFuel(); }
    int getRPMVal() const { return engine.getRPM(); }
    int getGearVal() const { return gearbox.getGear(); }
    bool isEngineOn() const { return engine.isRunning(); }
    ///////////////////////////////////////////////////////

    void accelerate() {
        if (!engine.isRunning()) return;
        
        // Zwiększanie obrotow silnika
        int currentRPM = engine.getRPM();
        if (currentRPM < 6000) {
            engine.setRPM(currentRPM + 250); // Przyrost obrotow 
        }

        // Obliczana prędkość na podstawie obrotów i biegu
        speed = (engine.getRPM() * gearbox.getRatio()) / 50.0f;
        
        // Silnik sie aktualizuje i zużywa paliwo
        engine.update(engine.getRPM());
        
        // Zapisanie predkosci do historii
        speedHistory.push_back(speed);
    }

    void brake() {
        if (speed > 0) {
            speed -= 5.0f;
            if (speed < 0) speed = 0;

            // Przy hamowaniu obroty spadaja, jeśli silnik jest na biegu 
            float currentRatio = gearbox.getRatio();
            if (currentRatio > 0.0f) { // Zabezpieczenie przed dzieleniem przez zero na luzie
                int targetRPM = static_cast<int>(speed * 50.0f / currentRatio);
                if (targetRPM < 800) targetRPM = 800;
                engine.setRPM(targetRPM);
            }
        }
    }

    void changeGear(bool up) {
        if (!engine.isRunning()) {
            if (up) gearbox.shiftUp();
            else gearbox.shiftDown();
            return;
        }

        // WARUNEK DŁAWIENIA SILNIKA i jego wyłaczenie w krrytycznym momencie
        if (up && gearbox.getGear() > 0 && engine.getRPM() < 1200) {
            stopCar(); // To resetuje też bieg na luz
            throw CarException("SILNIK ZGASŁ! Zbyt niskie obroty, by wrzucić wyższy bieg (dławienie).");
        }

        float oldRatio = gearbox.getRatio();
        int oldRPM = engine.getRPM(); // Stare obroty są zapisywane do logów
        
        if (up) gearbox.shiftUp();
        else gearbox.shiftDown();

        float newRatio = gearbox.getRatio();

        // Gdy zmieniamy biegi normalnie 
        if (newRatio > 0) {
            int newRPM = 0;

            // SCENARIUSZ 1: Zmiana z biegu na bieg (np. z jedynki na dwojke)
            if (oldRatio > 0) {
                newRPM = static_cast<int>(engine.getRPM() * (oldRatio / newRatio));
            } 
            // SCENARIUSZ 2: Zmiana z N na bieg (ruszanie)
            else {
                if (speed > 0) {
                    // Dopasowanie obrotow do aktualnej prędkości
                    newRPM = static_cast<int>(speed * 50.0f / newRatio);
                } else {
                    // Przywracane sa obroty jałowe
                    newRPM = (engine.getRPM() > 2000) ? 2000 : engine.getRPM(); 
                }
            }

            // WARUNEK ZEPSUCIA SILNIKA
            if (newRPM > 7500) { 
                engine.stop();
                speed = 0;
                throw CarException("SILNIK ROZLECIAŁ SIĘ! Zbyt wysokie obroty przy redukcji.");
            }

            // Diagnostyka silnika
            std::cout << " [GEARBOX] Zmiana biegu! Ratio: " << oldRatio << " -> " << newRatio 
                      << " | RPM: " << oldRPM << " -> " << newRPM << std::endl;

            engine.setRPM(newRPM);
        } 
        else {
            // Jeśli wrzucony jest luz, obroty spadają do jałowych
            engine.setRPM(800);
            std::cout << " [GEARBOX] Wrzucam Luz (N). RPM -> 800" << std::endl;
        }

        // Po zmianie biegu aktualizowana jest prędkość 
        speed = (engine.getRPM() * gearbox.getRatio()) / 50.0f;
    }

    // WYKORZYSTANIE RANGES
    void showTripSummary() const {
        if (speedHistory.empty()) return;

        auto movingOnly = speedHistory | std::ranges::views::filter([](float s) { return s > 0; });
        auto inMPH = movingOnly | std::ranges::views::transform([](float s) { return s * 0.6213f; });

        std::cout << "\n=== PODSUMOWANIE TRASY ===" << std::endl;
        
        auto result = std::ranges::minmax_element(speedHistory);
        if (result.max != speedHistory.end()) { // Zabezpieczenie przed pustym iteratorem
             std::cout << "Maksymalna prędkość: " << *result.max << " km/h" << std::endl;
        }
        
        std::cout << "Ostatnie pomiary w MPH: ";
        for (float s : inMPH | std::ranges::views::reverse | std::ranges::views::take(3)) {
            std::cout << s << " ";
        }
        std::cout << std::endl;
    }

    void displayDashboard() const {
        std::cout << "\n--- DESKA ROZDZIELCZA ---" << std::endl;
        std::cout << "Prędkość: " << speed << " km/h" << std::endl;
        std::cout << "Bieg: " << (gearbox.getGear() == 0 ? "N" : (gearbox.getGear() == -1 ? "R" : std::to_string(gearbox.getGear()))) << std::endl;
        std::cout << "Obroty: " << engine.getRPM() << " RPM" << std::endl;
        std::cout << "Paliwo: " << engine.getFuel() << " L" << std::endl;
        if (!engine.isRunning()) std::cout << "SILNIK ZGASZONY!" << std::endl;
    }
};