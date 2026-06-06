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

/*
Powyższy komentarz odnosi się do jednego z najważniejszych (i często najbardziej mylących na początku) mechanizmów tworzenia bibliotek dynamicznych (DLL) w języku C++ w środowisku Windows.
Chodzi o rozwiązanie problemu współdzielenia kodu. Zarówno biblioteka DLL, jak i aplikacja (.exe), która z niej korzysta, używają dokładnie tego samego pliku nagłówkowego (.h). Jednak każda z tych stron patrzy na zawarte w nim funkcje inaczej:
1. Z perspektywy biblioteki (kiedy kompilujemy projekt .dll):
Kompilator musi wiedzieć, które funkcje mają być "wystawione na zewnątrz", aby inne programy mogły je wywołać. Do tego służy instrukcja __declspec(dllexport) (eksportuj do DLL).
2. Z perspektywy aplikacji testującej (kiedy kompilujesz projekt .exe):
Kompilator czyta ten sam plik nagłówkowy, ale tym razem musi otrzymać informację: "Kodu tych funkcji tutaj nie ma, będziesz musiał je 'zaciągnąć' z zewnętrznego pliku DLL podczas uruchamiania programu". Do tego służy instrukcja __declspec(dllimport) (importuj z DLL).

C++
#ifdef MATHDYNAMIC_EXPORTS
   #define MATHDYNAMIC_API __declspec(dllexport)
#else
   #define MATHDYNAMIC_API __declspec(dllimport)
#endif

Ten blok kodu to instrukcje dla preprocesora, które działają jak przełącznik:
   * Kiedy budujemy bibliotekę DLL: W pliku konfiguracyjnym projektu (MathDynamic.vcxproj) celowo zdefiniowaliśmy flagę MATHDYNAMIC_EXPORTS. Ponieważ ta flaga istnieje, preprocesor wybiera górną 
   ścieżkę. Wszędzie tam, gdzie przy funkcji napiszemy MATHDYNAMIC_API, kompilator wklei __declspec(dllexport). Biblioteka udostępnia funkcje.
   * Kiedy budujemy aplikację .exe: Aplikacja nic nie wie o fladze MATHDYNAMIC_EXPORTS (nie zdefiniowaliśmy jej w projekcie testowym). Preprocesor przeskakuje do instrukcji #else. Tym razem podmienia 
   słowo MATHDYNAMIC_API na __declspec(dllimport). Aplikacja oczekuje funkcji z zewnątrz.
*/