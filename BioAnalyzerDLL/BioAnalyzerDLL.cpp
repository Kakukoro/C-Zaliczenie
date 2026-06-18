// BioAnalyzerDLL.cpp
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>

// Bezpieczne włączanie nagłówków Windowsa – tylko na systemie Windows
#ifdef _WIN32
#include "pch.h"
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

// --- Original logic from your script ---
std::string rozpoznajTypImpl(const std::string& seq) {
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
    return "DNA";
}

std::string analizujSekwencjeImpl(const std::string& seq) {
    std::ostringstream oss;
    oss << "\n--- PODSTAWOWA ANALIZA ---\n";
    oss << "Dlugosc sekwencji: " << seq.length() << " aminokwasow/nukleotydow.\n";

    std::map<char, int> liczniki;
    int gcCount = 0;
    int nukiCount = 0;
    for (char c : seq) {
        char uc = toupper(c);
        liczniki[uc]++;
        if (uc == 'G' || uc == 'C') gcCount++;
        if (uc == 'A' || uc == 'T' || uc == 'U' || uc == 'G' || uc == 'C') nukiCount++;
    }

    oss << "Liczba poszczegolnych elementow:\n";
    for (auto const& [znak, ilosc] : liczniki)
        oss << "  " << znak << ": " << ilosc << "\n";

    if (nukiCount > 0) {
        double gcProcent = (static_cast<double>(gcCount) / seq.length()) * 100.0;
        oss << "Zawartosc GC: " << gcProcent << "%\n";
    }
    return oss.str();
}

std::string transkrypcjaImpl(const std::string& dna) {
    std::string rna = dna;
    for (char& c : rna) {
        if (toupper(c) == 'T') c = 'U';
    }
    return rna;
}

std::string nicKomplementarnaImpl(const std::string& dna) {
    std::string komplementarna;
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

std::string translacjaImpl(const std::string& rna) {
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
        {"GAA", 'E'}, {"GGU", 'G'}, {"GGC", 'G'}, {"GGA", 'G'},
        {"GGG", 'G'}, {"UAA", '*'}, {"UAG", '*'}, {"UGA", '*'}
    };

    std::string bialko;
    for (size_t i = 0; i + 2 < rna.length(); i += 3) {
        std::string kodon = rna.substr(i, 3);
        for (char& c : kodon) c = toupper(c);
        if (kodony.count(kodon)) {
            char aminokwas = kodony[kodon];
            if (aminokwas == '*') {
                bialko += "[Kodon STOP]";
                break;
            }
            if (aminokwas == 'M' && i == 0)
                bialko += "[START]";
            bialko += aminokwas;
        }
    }
    return bialko;
}

std::string wyszukajMotywImpl(const std::string& seq, const std::string& motyw) {
    std::ostringstream oss;
    oss << "\n--- WYSZUKIWANIE MOTYWU ---\n";
    size_t pos = seq.find(motyw);
    if (pos == std::string::npos) {
        oss << "Nie znaleziono motywu: " << motyw << "\n";
        return oss.str();
    }
    while (pos != std::string::npos) {
        oss << "Znaleziono motyw '" << motyw << "' na pozycji: " << pos << "\n";
        pos = seq.find(motyw, pos + 1);
    }
    return oss.str();
}

std::string analizujMutacjeImpl(const std::string& seq1, const std::string& seq2) {
    std::ostringstream oss;
    oss << "\n--- ANALIZA MUTACJI ---\n";
    if (seq1.length() != seq2.length()) {
        oss << "Sekwencje maja rozne dlugosci.\n";
        return oss.str();
    }

    int mutacje = 0;
    for (size_t i = 0; i < seq1.length(); ++i) {
        if (toupper(seq1[i]) != toupper(seq2[i])) {
            oss << "Mutacja na pozycji " << i << ": " << seq1[i] << " -> " << seq2[i] << "\n";
            mutacje++;
        }
    }
    if (mutacje == 0) oss << "Sekwencje sa identyczne!\n";
    return oss.str();
}

// --- NOWA LOGIKA GENEROWANIA DANYCH POD WYKRESY ---
std::string pobierzSkladPercentImpl(const std::string& seq) {
    if (seq.empty()) return "0,0,0,0";
    double a = 0, t = 0, g = 0, c = 0, total = 0;
    for (char ch : seq) {
        char uc = toupper(ch);
        if (uc == 'A') { a++; total++; }
        else if (uc == 'T' || uc == 'U') { t++; total++; }
        else if (uc == 'G') { g++; total++; }
        else if (uc == 'C') { c++; total++; }
    }
    if (total == 0) return "0,0,0,0";
    std::ostringstream oss;
    oss << (a/total)*100 << "," << (t/total)*100 << "," << (g/total)*100 << "," << (c/total)*100;
    return oss.str();
}

std::string obliczGCOknoImpl(const std::string& seq, int windowSize) {
    if (seq.empty() || windowSize <= 0) return "0";
    std::ostringstream oss;
    for (size_t i = 0; i + windowSize <= seq.length(); ++i) {
        double gc = 0;
        for (int j = 0; j < windowSize; ++j) {
            char uc = toupper(seq[i + j]);
            if (uc == 'G' || uc == 'C') gc++;
        }
        oss << (gc / windowSize) * 100 << (i + windowSize < seq.length() ? "," : "");
    }
    return oss.str();
}


// --- Eksportowane funkcje C (widziane przez aplikację Qt) ---
extern "C" {
EXPORT const char* rozpoznajTyp(const char* seq) {
    static std::string result;
    result = rozpoznajTypImpl(seq);
    return result.c_str();
}

EXPORT const char* analizujSekwencje(const char* seq) {
    static std::string result;
    result = analizujSekwencjeImpl(seq);
    return result.c_str();
}

EXPORT const char* transkrypcja(const char* dna) {
    static std::string result;
    result = transkrypcjaImpl(dna);
    return result.c_str();
}

EXPORT const char* nicKomplementarna(const char* dna) {
    static std::string result;
    result = nicKomplementarnaImpl(dna);
    return result.c_str();
}

EXPORT const char* translacja(const char* rna) {
    static std::string result;
    result = translacjaImpl(rna);
    return result.c_str();
}

EXPORT const char* wyszukajMotyw(const char* seq, const char* motyw) {
    static std::string result;
    result = wyszukajMotywImpl(seq, motyw);
    return result.c_str();
}

EXPORT const char* analizujMutacje(const char* seq1, const char* seq2) {
    static std::string result;
    result = analizujMutacjeImpl(seq1, seq2);
    return result.c_str();
}

EXPORT const char* pobierzSkladPercent(const char* seq) {
    static std::string result;
    result = pobierzSkladPercentImpl(seq);
    return result.c_str();
}

EXPORT const char* obliczGCOkno(const char* seq, int windowSize) {
    static std::string result;
    result = obliczGCOknoImpl(seq, windowSize);
    return result.c_str();
}
}