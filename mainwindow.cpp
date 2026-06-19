#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), dll(nullptr) // Dobra praktyka: inicjalizacja wskaźnika
{
    // Główne okno i układ
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Grupa wejścia
    QGroupBox *inputGroup = new QGroupBox("Wejście", this);
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);

    // 1. Pierwsza sekwencja + przycisk
    QHBoxLayout *seq1Layout = new QHBoxLayout();
    sequenceInput = new QLineEdit(this);
    sequenceInput->setPlaceholderText("Wprowadź sekwencję (np. ATGC...)");
    QPushButton *loadSeq1Btn = new QPushButton("📂 Wczytaj plik", this);
    seq1Layout->addWidget(sequenceInput);
    seq1Layout->addWidget(loadSeq1Btn);
    inputLayout->addLayout(seq1Layout);

    // 2. Motyw (zostaje bez zmian, bo motywy wpisuje się ręcznie)
    motifInput = new QLineEdit(this);
    motifInput->setPlaceholderText("Wprowadź motyw do wyszukania");
    inputLayout->addWidget(motifInput);

    // 3. Druga sekwencja (mutacja) + przycisk
    QHBoxLayout *seq2Layout = new QHBoxLayout();
    secondSequenceInput = new QLineEdit(this);
    secondSequenceInput->setPlaceholderText("Wprowadź drugą sekwencję do porównania mutacji");
    QPushButton *loadSeq2Btn = new QPushButton("📂 Wczytaj plik", this);
    seq2Layout->addWidget(secondSequenceInput);
    seq2Layout->addWidget(loadSeq2Btn);
    inputLayout->addLayout(seq2Layout);

    // Połączenie nowych przycisków z funkcjami
    connect(loadSeq1Btn, &QPushButton::clicked, this, &MainWindow::wczytajPlikGlowny);
    connect(loadSeq2Btn, &QPushButton::clicked, this, &MainWindow::wczytajPlikMutacji);
    // Grupa wyjścia
    QGroupBox *outputGroup = new QGroupBox("Wynik", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    output = new QTextEdit(this);
    output->setReadOnly(true);
    outputLayout->addWidget(output);

    // Grupa przycisków
    QGroupBox *buttonsGroup = new QGroupBox("Operacje", this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsGroup);

    QPushButton *analyzeButton = new QPushButton("Analizuj Sekwencję", this);
    QPushButton *transcribeButton = new QPushButton("Transkrypcja (DNA → RNA)", this);
    QPushButton *complementaryButton = new QPushButton("Nić Komplementarna", this);
    QPushButton *translateButton = new QPushButton("Translacja (RNA → Białko)", this);
    QPushButton *motifButton = new QPushButton("Wyszukaj Motyw", this);
    QPushButton *mutationButton = new QPushButton("Analizuj Mutacje", this);
    QPushButton *chartsButton = new QPushButton("Pokaż Wykresy Statystyk", this);

    buttonsLayout->addWidget(analyzeButton);
    buttonsLayout->addWidget(transcribeButton);
    buttonsLayout->addWidget(complementaryButton);
    buttonsLayout->addWidget(translateButton);
    buttonsLayout->addWidget(motifButton);
    buttonsLayout->addWidget(mutationButton);
    buttonsLayout->addWidget(chartsButton);

    // Dodaj grupy do głównego układu
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(buttonsGroup);
    mainLayout->addWidget(outputGroup);

    setCentralWidget(centralWidget);

    // Połącz przyciski ze slotami
    connect(analyzeButton, &QPushButton::clicked, this, &MainWindow::analizujSekwencje);
    connect(transcribeButton, &QPushButton::clicked, this, &MainWindow::transkrybujDNA);
    connect(complementaryButton, &QPushButton::clicked, this, &MainWindow::pobierzNicKomplementarna);
    connect(translateButton, &QPushButton::clicked, this, &MainWindow::tlumaczRNA);
    connect(motifButton, &QPushButton::clicked, this, &MainWindow::wyszukajMotyw);
    connect(mutationButton, &QPushButton::clicked, this, &MainWindow::analizujMutacje);
    connect(chartsButton, &QPushButton::clicked, this, &MainWindow::pokazWykresy);

    // Załaduj bibliotekę
    if (!zaladujDLL()) {
        QMessageBox::critical(this, "Błąd", "Nie udało się załadować biblioteki BioAnalyzer!");
    }
}

bool MainWindow::zaladujDLL()
{
    QString appDir = QCoreApplication::applicationDirPath();

    // Qt automatycznie doda .dylib na Macu, .so na Linuxie i .dll na Windowsie
    dll = new QLibrary(appDir + "/BioAnalyzerDLL/libBioAnalyzer", this);

    qDebug() << "Próba załadowania biblioteki z:" << dll->fileName();
    if (!dll->load()) {
        qWarning() << "Nie udało się załadować biblioteki:" << dll->errorString();
        return false;
    }
    qDebug() << "Biblioteka załadowana pomyślnie!";

    // Rozwiąż funkcje (Przeniesione do wewnątrz metody!)
    rozpoznajTypFunc = (RozpoznajTypFunc) dll->resolve("rozpoznajTyp");
    analizujSekwencjeFunc = (AnalizujSekwencjeFunc) dll->resolve("analizujSekwencje");
    transkrypcjaFunc = (TranskrypcjaFunc) dll->resolve("transkrypcja");
    nicKomplementarnaFunc = (NicKomplementarnaFunc) dll->resolve("nicKomplementarna");
    translacjaFunc = (TranslacjaFunc) dll->resolve("translacja");
    wyszukajMotywFunc = (WyszukajMotywFunc) dll->resolve("wyszukajMotyw");
    analizujMutacjeFunc = (AnalizujMutacjeFunc) dll->resolve("analizujMutacje");


    if (!rozpoznajTypFunc || !analizujSekwencjeFunc || !transkrypcjaFunc || !nicKomplementarnaFunc
        || !translacjaFunc || !wyszukajMotywFunc || !analizujMutacjeFunc) {
        qWarning() << "Nie udało się rozpoznać jednej lub więcej funkcji!";
        return false;
    }

    return true;
}

void MainWindow::analizujSekwencje()
{
    QString seq = sequenceInput->text();
    if (seq.isEmpty()) {
        output->append("Wprowadź sekwencję.");
        return;
    }
    if (!analizujSekwencjeFunc) return;
    const char *result = analizujSekwencjeFunc(seq.toStdString().c_str());
    output->append(QString::fromUtf8(result));
}

void MainWindow::transkrybujDNA()
{
    QString dna = sequenceInput->text();
    if (dna.isEmpty()) {
        output->append("Wprowadź sekwencję DNA.");
        return;
    }
    if (!transkrypcjaFunc) return;
    const char *result = transkrypcjaFunc(dna.toStdString().c_str());
    output->append("Transkrypcja (DNA → RNA): " + QString::fromUtf8(result));
}

void MainWindow::pobierzNicKomplementarna()
{
    QString dna = sequenceInput->text();
    if (dna.isEmpty()) {
        output->append("Wprowadź sekwencję DNA.");
        return;
    }
    if (!nicKomplementarnaFunc) return;
    const char *result = nicKomplementarnaFunc(dna.toStdString().c_str());
    output->append("Nić komplementarna: " + QString::fromUtf8(result));
}

void MainWindow::tlumaczRNA()
{
    QString rna = sequenceInput->text().toUpper().replace("T", "U"); // Konwersja DNA na RNA w locie dla bezpieczeństwa
    if (rna.isEmpty()) {
        output->append("Wprowadź sekwencję RNA.");
        return;
    }
    if (!translacjaFunc) {
        output->append("Błąd: Funkcja translacji nie jest załadowana.");
        return;
    }
    const char *result = translacjaFunc(rna.toStdString().c_str());

    if (!result || strlen(result) == 0) {
        output->append("Translacja zwróciła pusty wynik. Upewnij się, że wprowadzasz poprawną sekwencję RNA (A, U, G, C).");
    } else {
        output->append("Translacja (RNA → Białko): " + QString::fromUtf8(result));
    }
}

void MainWindow::wyszukajMotyw()
{
    QString seq = sequenceInput->text();
    QString motif = motifInput->text();
    if (seq.isEmpty() || motif.isEmpty()) {
        output->append("Wprowadź zarówno sekwencję, jak i motyw.");
        return;
    }
    if (!wyszukajMotywFunc) return;
    const char *result = wyszukajMotywFunc(seq.toStdString().c_str(), motif.toStdString().c_str());
    output->append(QString::fromUtf8(result));
}

void MainWindow::analizujMutacje()
{
    QString seq1 = sequenceInput->text();
    QString seq2 = secondSequenceInput->text();
    if (seq1.isEmpty() || seq2.isEmpty()) {
        output->append("Wprowadź obie sekwencje.");
        return;
    }
    if (!analizujMutacjeFunc) return;
    const char *result = analizujMutacjeFunc(seq1.toStdString().c_str(), seq2.toStdString().c_str());
    output->append(QString::fromUtf8(result));
}

#include <map>

// Funkcja obliczająca skład procentowy każdego nukleotydu
std::map<char, double> MainWindow::obliczSkladProcentowy(const QString &sekwencja)
{
    std::map<char, double> sklad;
    QString seq = sekwencja.toUpper(); // Ujednolicamy wielkość liter
    int dlugosc = seq.length();

    if (dlugosc == 0) return sklad;

    // Zliczanie wystąpień
    for (int i = 0; i < dlugosc; ++i) {
        char nukleotyd = seq[i].toLatin1();
        sklad[nukleotyd]++;
    }

    // Zamiana na procenty
    for (auto &para : sklad) {
        para.second = (para.second / dlugosc) * 100.0;
    }

    return sklad;
}

// Funkcja obliczająca ogólny procent GC w sekwencji
double MainWindow::obliczZawartoscGC(const QString &sekwencja)
{
    QString seq = sekwencja.toUpper();
    int dlugosc = seq.length();
    if (dlugosc == 0) return 0.0;

    int licznikGC = 0;
    for (int i = 0; i < dlugosc; ++i) {
        char n = seq[i].toLatin1();
        if (n == 'G' || n == 'C') {
            licznikGC++;
        }
    }

    return (static_cast<double>(licznikGC) / dlugosc) * 100.0;
}

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>

void MainWindow::pokazWykresy()
{
    QString seq = sequenceInput->text().toUpper();
    if (seq.isEmpty()) {
        QMessageBox::warning(this, "Brak danych", "Wprowadź najpierw sekwencję!");
        return;
    }

    // 1. Obliczenia danych
    double gc = obliczZawartoscGC(seq);
    std::map<char, double> sklad = obliczSkladProcentowy(seq);

    // 2. Tworzenie okna dialogowego
    QDialog *chartDialog = new QDialog(this);
    chartDialog->setWindowTitle("Statystyki Bioinformatyczne");
    chartDialog->setFixedSize(550, 450); // Stały, bezpieczny rozmiar okna

    QVBoxLayout *mainLayout = new QVBoxLayout(chartDialog);

    // 3. Nagłówek okna
    QLabel *titleLabel = new QLabel("ANALIZA SKŁADU I ZAWARTOŚCI GC", chartDialog);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #ffffff; margin-bottom: 15px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 4. PRZYGOTOWANIE RYSUNKU (Klasyczny Wykres Słupkowy z osiami X i Y)
    // Tworzymy "płótno" o wymiarach 500x300 pikseli, na którym namalujemy wykres
    QPixmap pixmap(500, 300);
    pixmap.fill(Qt::white); // Białe tło wykresu

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing); // Wygładzanie krawędzi

    // Definiujemy marginesy i obszar wykresu
    int originX = 50;  // Gdzie zaczyna się oś Y
    int originY = 250; // Gdzie zaczyna się oś X (na dole)
    int graphHeight = 200;
    int graphWidth = 420;

    // Rysowanie osi układu współrzędnych (Czarny kolor, grubość 2px)
    QPen axisPen(Qt::black, 2);
    painter.setPen(axisPen);
    painter.drawLine(originX, originY, originX + graphWidth, originY); // Oś X
    painter.drawLine(originX, originY, originX, originY - graphHeight); // Oś Y

    // Rysowanie podziałek i linii pomocniczych na osi Y (0%, 50%, 100%)
    QPen gridPen(Qt::lightGray, 1, Qt::DashLine);
    painter.setFont(QFont("Arial", 9));

    for (int percent = 0; percent <= 100; percent += 50) {
        int y = originY - (percent * graphHeight / 100);
        painter.setPen(gridPen);
        if (percent > 0) {
            painter.drawLine(originX, y, originX + graphWidth, y); // Linia siatki
        }
        painter.setPen(Qt::black);
        painter.drawText(originX - 40, y + 5, QString("%1%").arg(percent));
    }

    // Dane do wykresu: Pary (Nazwa słupka, Wartość, Kolor)
    struct BarData {
        QString label;
        double value;
        QColor color;
    };

    QList<BarData> bars = {
        {"A", sklad['A'], QColor("#3498db")},  // Niebieski
        {"T", sklad['T'], QColor("#e74c3c")},  // Czerwony
        {"G", sklad['G'], QColor("#f1c40f")},  // Żółty
        {"C", sklad['C'], QColor("#9b59b6")},  // Purpurowy
        {"GC", gc, QColor("#2ecc71")}          // Zielony
    };

    // Jeśli sekwencja to RNA, zamień T na U
    if (sklad['U'] > 0) {
        bars[1] = {"U", sklad['U'], QColor("#e67e22")}; // Pomarańczowy dla U
    }

    // Rysowanie słupków pionowych
    int barWidth = 45;
    int spacing = 35; // Odstęp między słupkami

    for (int i = 0; i < bars.size(); ++i) {
        // Obliczamy pozycję X dla danego słupka
        int x = originX + spacing + i * (barWidth + spacing);

        // Skalujemy wysokość słupka (w zależności od wartości 0-100%)
        int barHeight = static_cast<int>((bars[i].value / 100.0) * graphHeight);
        int y = originY - barHeight;

        // Rysowanie słupka (pionowy prostokąt)
        painter.setPen(Qt::NoPen);
        painter.setBrush(bars[i].color);
        painter.drawRect(x, y, barWidth, barHeight);

        // Rysowanie czarnego podpisu wartości NAD słupkiem (zawsze widoczny!)
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        QString valueText = QString("%1%").arg(bars[i].value, 0, 'f', 1);
        painter.drawText(x - 2, y - 8, valueText);

        // Rysowanie podpisu pod osią X (A, T, G, C, GC)
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(x + (barWidth / 2) - 8, originY + 20, bars[i].label);
    }

    painter.end(); // Kończymy rysowanie

    // 5. Osadzenie rysunku w oknie dialogowym
    QLabel *chartLabel = new QLabel(chartDialog);
    chartLabel->setPixmap(pixmap);
    chartLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(chartLabel);

    // 6. Przycisk Zamknij na dole
    QPushButton *closeButton = new QPushButton("Zamknij", chartDialog);
    closeButton->setStyleSheet("padding: 5px 15px; font-size: 12px;");
    connect(closeButton, &QPushButton::clicked, chartDialog, &QDialog::close);
    mainLayout->addWidget(closeButton, 0, Qt::AlignRight);

    chartDialog->exec();
}

void MainWindow::wczytajPlikGlowny()
{
    // Okno wyboru pliku z filtrem na TXT i FASTA
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Wybierz plik z sekwencją (Dozwolone: .fasta, .fa, .txt)",
                                                    "",
                                                    "Pliki sekwencji (*.fasta *.fa *.txt);;Wszystkie pliki (*)");

    if (fileName.isEmpty()) return; // Użytkownik anulował wybór

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Błąd", "Nie można otworzyć pliku!");
        return;
    }

    QTextStream in(&file);
    QString czystaSekwencja = "";
    bool czyFasta = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // Jeśli to format FASTA, ignorujemy linię nagłówka rozpoczynającą się od '>'
        if (line.startsWith('>')) {
            czyFasta = true;
            continue;
        }

        // Doklejamy linię do całości (FASTA często łamie sekwencję na wiele linii)
        czystaSekwencja += line;
    }
    file.close();

    // Wrzucamy oczyszczony tekst do pierwszego okienka
    sequenceInput->setText(czystaSekwencja.toUpper());

    // Małe powiadomienie dla użytkownika
    if (czyFasta) {
        output->append("Pomyślnie wczytano i przefiltrowano plik formatu FASTA.");
    } else {
        output->append("Pomyślnie wczytano plik tekstowy TXT.");
    }
}

void MainWindow::wczytajPlikMutacji()
{
    // Dokładnie to samo dla drugiego pola (sekwencji z mutacją)
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Wybierz plik z sekwencją mutacji (Dozwolone: .fasta, .fa, .txt)",
                                                    "",
                                                    "Pliki sekwencji (*.fasta *.fa *.txt);;Wszystkie pliki (*)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Błąd", "Nie można otworzyć pliku!");
        return;
    }

    QTextStream in(&file);
    QString czystaSekwencja = "";
    bool czyFasta = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        if (line.startsWith('>')) {
            czyFasta = true;
            continue;
        }
        czystaSekwencja += line;
    }
    file.close();

    secondSequenceInput->setText(czystaSekwencja.toUpper());

    if (czyFasta) {
        output->append("Pomyślnie wczytano i przefiltrowano plik mutacji formatu FASTA.");
    } else {
        output->append("Pomyślnie wczytano plik tekstowy mutacji TXT.");
    }
}