#pragma once
#include <stdexcept>

class CarException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Rzucenie błedu braku paliwa
class OutOfFuelException : public CarException {
public:
    OutOfFuelException() : CarException("BŁĄD: Brak paliwa!") {}
};

// Rzucenie błedu zatarcia silnika
class EngineSeizedException : public CarException {
public:
    EngineSeizedException() : CarException("KATASTROFA: Silnik zatarty!") {}
};