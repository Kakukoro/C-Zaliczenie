#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLibrary>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void analizujSekwencje();
    void transkrybujDNA();
    void pobierzNicKomplementarna();
    void tlumaczRNA();
    void wyszukajMotyw();
    void analizujMutacje();
    void pokazWykresy();
    void wczytajPlikGlowny();
    void wczytajPlikMutacji();

private:
    QLineEdit *sequenceInput;
    QLineEdit *motifInput;
    QLineEdit *secondSequenceInput;
    QTextEdit *output;

    QLibrary *dll;

    // Typedefy dla funkcji z DLL (polskie nazwy!)
    typedef const char *(*RozpoznajTypFunc)(const char *);
    typedef const char *(*AnalizujSekwencjeFunc)(const char *);
    typedef const char *(*TranskrypcjaFunc)(const char *);
    typedef const char *(*NicKomplementarnaFunc)(const char *);
    typedef const char *(*TranslacjaFunc)(const char *);
    typedef const char *(*WyszukajMotywFunc)(const char *, const char *);
    typedef const char *(*AnalizujMutacjeFunc)(const char *, const char *);
    std::map<char, double> obliczSkladProcentowy(const QString &sekwencja);
    double obliczZawartoscGC(const QString &sekwencja);

    // Wskaźniki do funkcji DLL
    RozpoznajTypFunc rozpoznajTypFunc;
    AnalizujSekwencjeFunc analizujSekwencjeFunc;
    TranskrypcjaFunc transkrypcjaFunc;
    NicKomplementarnaFunc nicKomplementarnaFunc;
    TranslacjaFunc translacjaFunc;
    WyszukajMotywFunc wyszukajMotywFunc;
    AnalizujMutacjeFunc analizujMutacjeFunc;

    bool zaladujDLL();
};