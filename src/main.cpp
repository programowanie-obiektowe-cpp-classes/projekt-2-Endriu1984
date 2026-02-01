#include <memory>  
#include <string>  
#include "Car.hpp"
#include "Exceptions.hpp"

// Biblioteka FTXUI (Interfejs graficzny w terminalu)
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int main() {
    // Tworzenie samochodu
    Car myCar;
    
    // Wyswietlanie komunikatów o bledach lub statusie
    std::string lastMessage = "Gotowy do jazdy. Wcisnij 'S' aby odpalic.";
    bool errorState = false;

    // Wlaczenie interfejsu FTXUI
    auto screen = ScreenInteractive::Fullscreen();

    // Rendering ekranu
    auto renderer = Renderer([&] {
        
        // Pobieranie danych z klasy Car
        
        float speed = myCar.getSpeedVal();
        int rpm = myCar.getRPMVal();
        float fuel = myCar.getFuelVal();
        int gear = myCar.getGearVal();
        
        // Obliczanie procentow dla pasków
        float speed_p = speed / 200.0f; // Zakładamy max 200 km/h ale i tak samochod sie nie rozpedza do takiej wielkosci
        float rpm_p = rpm / 8000.0f;    // Max 8000 RPM
        float fuel_p = fuel / 20.0f;    // Max 20 litrów paliwa

        // Kolory pasków
        auto rpm_color = (rpm > 6500) ? Color::Red : Color::Green;
        auto fuel_color = (fuel < 5.0f) ? Color::Red : Color::Blue;

        return vbox({
            text("SYMULATOR C++ 2026") | bold | center,
            separator(),
            hbox({
                // LEWA STRONA: ZEGARY
                vbox({
                    text("PREDKOSC: " + std::to_string((int)speed) + " km/h"),
                    gauge(speed_p) | color(Color::Cyan),
                    text("OBROTY: " + std::to_string(rpm) + " RPM"),
                    gauge(rpm_p) | color(rpm_color),
                    text("PALIWO: " + std::to_string((int)fuel) + " L"),
                    gauge(fuel_p) | color(fuel_color),
                }) | border | flex,

                // PRAWA STRONA: BIEGI I STATUS
                vbox({
                    text("BIEG") | center,
                    text(std::to_string(gear)) | bold |  color(Color::Yellow) | center | size(HEIGHT, EQUAL, 3),
                    separator(),
                    paragraph(lastMessage) | color(errorState ? Color::Red : Color::White) | center,
                }) | border | flex,
            }),
            separator(),
            text("STEROWANIE: [S]tart [G]az [H]amulec [Strzalki G/D] Biegi [ESC] Wyjscie") | center
        }) | border;
    });

    // Obsługa klawiatury
    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.Exit();
            return true;
        }

        try {
            if (event == Event::Character('s') || event == Event::Character('S')) {
                if (!myCar.isEngineOn()) {
                    myCar.startCar();
                    lastMessage = "Silnik uruchomiony!";
                    errorState = false;
                } else {
                    myCar.stopCar();
                    lastMessage = "Silnik zgaszony.";
                }
            }
            if (event == Event::Character('g') || event == Event::Character('G')) {
                myCar.accelerate();
                lastMessage = "Przyspieszanie...";
            }
            if (event == Event::Character('h') || event == Event::Character('H')) {
                myCar.brake();
                lastMessage = "Hamowanie.";
            }
            if (event == Event::ArrowUp) {
                myCar.changeGear(true);
                lastMessage = "Zmiana biegu w gore.";
            }
            if (event == Event::ArrowDown) {
                myCar.changeGear(false);
                lastMessage = "Redukcja biegu.";
            }
        } catch (const CarException& e) {
            lastMessage = e.what(); // Wyświetla błąd w ramce
            errorState = true;
        }

        return false;
    });

    // Uruchomienie pętli
    screen.Loop(component);
    
    // Po wyjściu z GUI pokaż podsumowanie
    myCar.showTripSummary();
    return 0;
}