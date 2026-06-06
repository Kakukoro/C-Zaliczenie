#include <iostream>
// Dołączamy nagłówki z naszych bibliotek (w rzeczywistości trzeba ustawić odpowiednie ścieżki include w IDE)
#include "MathStatic.h"
#include "MathDynamic.h"

int main() {
    std::cout << "--- Test Biblioteki Statycznej (.lib) ---\n";
    std::cout << "5 + 3 = " << MathStatic::Add(5, 3) << "\n";
    std::cout << "5 - 3 = " << MathStatic::Subtract(5, 3) << "\n\n";

    std::cout << "--- Test Biblioteki Dynamicznej (.dll) ---\n";
    std::cout << "5 * 3 = " << MathDynamic::Multiply(5, 3) << "\n";
    std::cout << "5.0 / 2.0 = " << MathDynamic::Divide(5.0, 2.0) << "\n";

    return 0;
}