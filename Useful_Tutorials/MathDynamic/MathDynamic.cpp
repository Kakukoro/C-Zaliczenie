#include "MathDynamic.h"

namespace MathDynamic {
    int Multiply(int a, int b) {
        return a * b;
    }

    double Divide(double a, double b) {
        if (b == 0.0) return 0.0; // Uproszczona obsługa błędu
        return a / b;
    }
}