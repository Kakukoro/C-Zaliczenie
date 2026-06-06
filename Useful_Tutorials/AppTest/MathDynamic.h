#pragma once

// Mechanizm eksportu/importu w zależności od tego, kto kompiluje kod
#ifdef MATHDYNAMIC_EXPORTS
#define MATHDYNAMIC_API __declspec(dllexport)
#else
#define MATHDYNAMIC_API __declspec(dllimport)
#endif

namespace MathDynamic {
    MATHDYNAMIC_API int Multiply(int a, int b);
    MATHDYNAMIC_API double Divide(double a, double b);
}