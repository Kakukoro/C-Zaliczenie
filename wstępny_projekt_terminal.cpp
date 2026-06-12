#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// 1. Automatyczne rozpoznawanie typu sekwencji (DNA/RNA/Białko)
std::string rozpoznajTyp(const std::string& seq) {
    bool maT = false;
    bool maU = false;
    bool maInneNukleotydy = false;

    for (char c : seq) {
        char uc = toupper(c);
        if (uc == 'T') maT = true;
        else if (uc == 'U') maU = true;
        else if (uc != 'A' && uc != 'C' && uc != 'G') {
            maInneNukleotydy = true;
        }
    }

    if (maInneNukleotydy) return "Bialko";
    if (maU && !maT) return "RNA";
    return "DNA"; // Domyślnie lub gdy ma T
}

// 2. Podstawowa analiza i statystyki
void analizujSekwencje(const std::string& seq) {
    std::cout << "\n--- PODSTAWOWA ANALIZA ---" << std::endl;
    std::cout << "Dlugosc sekwencji: " << seq.length() << " aminokwasow/nukleotydow." << std::endl;

    std::map<char, int> liczniki;
    int gcCount = 0;
    int nukiCount = 0;

    for (char c : seq) {
        char uc = toupper(c);
        liczniki[uc]++;
        if (uc == 'G' || uc == 'C') gcCount++;
        if (uc == 'A' || uc == 'T' || uc == 'U' || uc == 'G' || uc == 'C') nukiCount++;
    }

    std::cout << "Liczba poszczegolnych elementow:" << std::endl;
    for (auto const& [znak, ilosc] : liczniki) {
        std::cout << "  " << znak << ": " << ilosc << std::endl;
    }

    if (nukiCount > 0) {
        double gcProcent = (static_cast<double>(gcCount) / seq.length()) * 100.0;
        std::cout << "Zawartosc GC: " << gcProcent << "%" << std::endl;
    }
}

// 3. Operacje na DNA/RNA
std::string transkrypcja(std::string dna) {
    for (char &c : dna) {
        if (toupper(c) == 'T') c = 'U';
        else if (toupper(c) == 't') c = 'u';
    }
    return dna;
}

std::string nicKomplementarna(std::string dna) {
    std::string komplementarna = "";
    for (char c : dna) {
        switch (toupper(c)) {
            case 'A': komplementarna += 'T'; break;
            case 'T': komplementarna += 'A'; break;
            case 'C': komplementarna += 'G'; break;
            case 'G': komplementarna += 'C'; break;
            default:  komplementarna += c;
        }
    }
    return komplementarna;
}

// 4. Translacja RNA -> Białko (Uproszczona tabela kodonów)
std::string translacja(const std::string& rna) {
    // Prosty słownik kodonów
    std::map<std::string, char> kodony = {
        {"AUG", 'M'}, {"UUU", 'F'}, {"UUC", 'F'}, {"UUA", 'L'}, {"UUG", 'L'},
        {"UCU", 'S'}, {"UCC", 'S'}, {"UCA", 'S'}, {"UCG", 'S'}, {"UAU", 'Y'},
        {"UAC", 'Y'}, {"UGU", 'C'}, {"UGC", 'C'}, {"UGG", 'W'}, {"CUU", 'L'},
        {"CUC", 'L'}, {"CUA", 'L'}, {"CUG", 'L'}, {"CCU", 'P'}, {"CCC", 'P'},
        {"CCA", 'P'}, {"CCG", 'P'}, {"CAU", 'H'}, {"CAC", 'H'}, {"CAA", 'Q'},
        {"CAG", 'Q'}, {"CGU", 'R'}, {"CGC", 'R'}, {"CGA", 'R'}, {"CGG", 'R'},
        {"AUU", 'I'}, {"AUC", 'I'}, {"AUA", 'I'}, {"ACU", 'T'}, {"ACC", 'T'},
        {"ACA", 'T'}, {"ACG", 'T'}, {"AAU", 'N'}, {"AAC", 'N'}, {"AAA", 'K'},
        {"AAG", 'K'}, {"AGU", 'S'}, {"AGC", 'S'}, {"AGA", 'R'}, {"AGG", 'R'},
        {"GUU", 'V'}, {"GUC", 'V'}, {"GUA", 'V'}, {"GUG", 'V'}, {"GCU", 'A'},
        {"GCC", 'A'}, {"GCA", 'A'}, {"GCG", 'A'}, {"GAU", 'D'}, {"GAC", 'D'},
        {"GAA", 'E'}, {"GAG", 'E'}, {"GGU", 'G'}, {"GGC", 'G'}, {"GGA", 'G'},
        {"GGG", 'G'}, {"UAA", '*'}, {"UAG", '*'}, {"UGA", '*'} // * oznacza STOP
    };

    std::string bialko = "";
    // Przetwarzamy co 3 litery (ramka odczytu 0)
    for (size_t i = 0; i + 2 < rna.length(); i += 3) {
        std::string kodon = rna.substr(i, 3);
        // Zamiana na wielkie litery dla pewności
        for (char &c : kodon) c = toupper(c);
        
        if (kodony.count(kodon)) {
            char aminokwas = kodony[kodon];
            if (aminokwas == '*') {
                std::cout << "[Kodon STOP na pozycji " << i << "]" << std::endl;
                break; // Przerywamy przy kodonie STOP
            }
            if (aminokwas == 'M' && i == 0) {
                std::cout << "[Kodon START na pozycji " << i << "]" << std::endl;
            }
            bialko += aminokwas;
        }
    }
    return bialko;
}

// 5. Wyszukiwanie motywów
void wyszukajMotyw(const std::string& seq, const std::string& motyw) {
    std::cout << "\n--- WYSZUKIWANIE MOTYWU ---" << std::endl;
    size_t pos = seq.find(motyw);
    if (pos == std::string::npos) {
        std::cout << "Nie znaleziono motywu: " << motyw << std::endl;
        return;
    }
    while (pos != std::string::npos) {
        std::cout << "Znaleziono motyw '" << motyw << "' na pozycji: " << pos << std::endl;
        pos = seq.find(motyw, pos + 1);
    }
}

// 6. Analiza mutacji między dwoma sekwencjami o tej samej długości
void analizujMutacje(const std::string& seq1, const std::string& seq2) {
    std::cout << "\n--- ANALIZA MUTACJI ---" << std::endl;
    if (seq1.length() != seq2.length()) {
        std::cout << "Sekwencje maja rozne dlugosci. Porownanie uproszczone (pozycja po pozycji) niemozliwe." << std::endl;
        return;
    }

    int mutacje = 0;
    for (size_t i = 0; i < seq1.length(); ++i) {
        if (toupper(seq1[i]) != toupper(seq2[i])) {
            std::cout << "Mutacja (Substytucja) na pozycji " << i 
                      << ": " << seq1[i] << " -> " << seq2[i] << std::endl;
            mutacje++;
        }
    }
    if (mutacje == 0) std::cout << "Sekwencje sa identyczne!" << std::endl;
}

int main() {
    std::string sekwencja;
    
    // Symulacja prostego menu/interfejsu w konsoli
    std::cout << "=== BIO-ANALIZATOR C++ ===" << std::endl;
    std::cout << "Wprowadz swoja sekwencje (np. ATGCEF...): ";
    std::cin >> sekwencja;

    // Automatyczne rozpoznawanie
    std::string typ = rozpoznajTyp(sekwencja);
    std::cout << "Rozpoznany typ sekwencji: " << typ << std::endl;

    // Podstawowe statystyki
    analizujSekwencje(sekwencja);

    // Operacje warunkowe zależne od typu sekwencji
    if (typ == "DNA") {
        std::cout << "\n--- OPERACJE DLA DNA ---" << std::endl;
        std::string rna = transkrypcja(sekwencja);
        std::cout << "Transkrypcja (DNA -> RNA): " << rna << std::endl;
        std::cout << "Nic komplementarna:        " << nicKomplementarna(sekwencja) << std::endl;
        
        std::cout << "\nPrzejdzmy do translacji tego RNA na bialko..." << std::endl;
        std::string bialko = translacja(rna);
        std::cout << "Wynik translacji (Bialko): " << bialko << std::endl;
    } else if (typ == "RNA") {
        std::cout << "\n--- OPERACJE DLA RNA ---" << std::endl;
        std::string bialko = translacja(sekwencja);
        std::cout << "Wynik translacji (Bialko): " << bialko << std::endl;
    }

    // Wyszukiwanie motywu
    std::string motyw;
    std::cout << "\nWpisz motyw do wyszukania w sekwencji: ";
    std::cin >> motyw;
    wyszukajMotyw(sekwencja, motyw);

    // Porównanie mutacji
    std::string drugaSekwencja;
    std::cout << "\nWpisz druga sekwencje do porownania mutacji (tej samej dlugosci): ";
    std::cin >> drugaSekwencja;
    analizujMutacje(sekwencja, drugaSekwencja);

    std::cout << "\nKoniec programu. Nacisnij Enter, aby zamknac...";
    std::cin.get(); // Czyszczenie bufora
    std::cin.get(); // Oczekiwanie na enter
    return 0;
}