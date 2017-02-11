#include <QString>
#include <QDir>
#include <QtCore>
#include <QtGui>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>
#include <QChar>
#include <QMap>
#include <QDebug>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    readHunspell();
    createInterface();
}


MainWindow::~MainWindow()
{
}


void MainWindow::createInterface()
{
    centralWidget = new QWidget(this);
    this->setCentralWidget( centralWidget );

    //this->setWindowState(Qt::WindowMaximized);

    //create file menu
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("&About"), this, SLOT(about()));
    fileMenu->addSeparator();
    //fileMenu->addAction(tr("&Open and Analyze"), this, SLOT(reset()));
    //fileMenu->addAction(tr("&Save"), this, SLOT(save()));
    //fileMenu->addAction(tr("&Load"), this, SLOT(load()));
    //fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), this, SLOT(close()));


    //create image label
    //slideImageLabel = new QLabel(this);


    //create inventory button
    analyzeButton=new QPushButton("Analyze");
    connect(analyzeButton, SIGNAL(clicked()), this, SLOT(analyze()));
    resetButton=new QPushButton("Clear");
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));


    //create field for slide text
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setReadOnly(true);


    //create and fill Layout
    mainBox = new QGridLayout(centralWidget);
    mainBox->addWidget(outputTextEdit, 0, 0, 5, 2);
    mainBox->addWidget(analyzeButton, 6, 0);
    mainBox->addWidget(resetButton, 6, 1);
}


void MainWindow::reset()
{
    outputTextEdit->clear();
}


void MainWindow::about()
{
    QMessageBox::information(this, "About", "A program that can find the verbs in Turkish sentences.");
}


void MainWindow::analyze()
{
    QString datapath = QFileDialog::getOpenFileName(this, tr("Öffne Text..."), QDir::currentPath(), tr("Textdateien (*.txt)"));
    if (datapath.isNull()==false)
    {
        //lies txt in QStringList
        QStringList stringList;
        QFile textFile(datapath);
        textFile.open(QFile::ReadOnly);
        QTextStream textStream(&textFile);
        textStream.setCodec("UTF-8");
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else {
                stringList.append(line);
            }
        }
        textFile.close();

        //Teile StringList in Sätze auf
        QString buffer="";
        QStringList sentences;
        for (int i=0; i<stringList.size(); i++) {
            for (int j=0; j<stringList.at(i).length(); j++) {
                if ((stringList.at(i).at(j)=='.') || (stringList.at(i).at(j)=='!') || (stringList.at(i).at(j)=='?')) {
                    buffer.append(stringList.at(i).at(j));
                    sentences.append(buffer);
                    buffer="";
                }
                else {
                    buffer.append(stringList.at(i).at(j));
                }
            }
        }

        //entferne leading and trailing spaces
        for (int i=0; i<sentences.size(); i++) {
            sentences.replace(i, sentences.at(i).trimmed());
        }


        //output
        outputTextEdit->append("Raw:\n");
        for (int i=0; i<stringList.size(); i++) {
            outputTextEdit->append(stringList.at(i).toUtf8().constData());
        }
        outputTextEdit->append("\nProcessed:\n");
        for (int i=0; i<sentences.size(); i++) {
            outputTextEdit->append(sentences.at(i).toUtf8().constData());
        }

        outputTextEdit->append("\nResults:\n");
        QString arrowString = " -> ";
        for (int i=0; i<sentences.size(); i++) {
            outputTextEdit->append(sentences.at(i).toUtf8().constData()+arrowString+checkVerb(sentences.at(i).toUtf8()));
        }
    }
}

QString MainWindow::checkVerb(QString satz) {
    QString output="";

    //entferne Punkt am Ende und alles in Kleinbuchstaben
    if ((satz.at(satz.length()-1)==',') || (satz.at(satz.length()-1)=='.') || (satz.at(satz.length()-1)=='!') || (satz.at(satz.length()-1)=='?')) {
        satz.remove(satz.length()-1,1);
        satz=satz.toLower();
    }

    //gibt es ein Komma, splitte und überprüfe beide Teile separat
    if (satz.contains(',')) {
        QStringList parts=satz.split(',', QString::SkipEmptyParts);
        for (int i=0; i<parts.length(); i++) {
            output.append(checkVerb(parts.at(i)));
        }
    }
    else {
        QStringList words=satz.split(' ', QString::SkipEmptyParts);
        //für jedes Wort:
        //stemme, dann finde Stamm in dic
        //wenn Stamm=Wort ist und keine Suffixe hat, Verb gefunden
        //sonst überprüfe, ob Stamm relevante Suffixe zulässt und finde das richtige
        for (int i=0; i<words.length(); i++) {
            QString currentStem="";
            QMap<QString, QString>::iterator iterStem = stemList.find(words.at(i));
            if (iterStem!=stemList.end()) {
                currentStem=iterStem.value();

                QVector<int> currentSuffixList;
                QString unprocessedSuffixes="";
                QMap<QString, QString>::iterator iterDict = dictionary.find(currentStem);
                if (iterDict!=dictionary.end())
                    unprocessedSuffixes=iterDict.value();
                QStringList processedSuffixes=unprocessedSuffixes.split(',', QString::SkipEmptyParts);
                if (processedSuffixes.length()>0) {
                    for (int j=0; j<processedSuffixes.length(); j++) {
                        currentSuffixList.append(processedSuffixes.at(j).toInt());
                    }
                }

                if (currentSuffixList.first()==-1) {
                    if (words.at(i)==currentStem) {
                        output.append(" Verb: "+words.at(i));
                    }
                }
                else {
                    for (int j=0; j<currentSuffixList.length(); j++) {
                        if (relevantSuffixes.contains(currentSuffixList.at(j))) {

                            QString compareString="";
                            compareString.append(currentStem);

                            QMap<int, QString>::iterator iterSuff = relevantSuffixes.find(currentSuffixList.at(j));
                            if (iterSuff!=relevantSuffixes.end())
                                compareString.append(iterSuff.value());

                            if (words.at(i)==compareString) {
                                output.append(" Verb: "+words.at(i));
                                break;
                            }
                        }
                    }
                }
                //FALLBACK WEGEN GIDIYORUZ
                //wenn bisher kein Verb gefunden wurde, überprüfe, ob das Wort mit einem
                //gesuchten Suffix endet, überprüfe dann im dictionary, ob das Wort als
                //eigener Eintrag ohne Suffix existiert
                if (output=="") {
                    QMap<int, QString>::const_iterator iterRel = relevantSuffixes.constBegin();
                    while (iterRel != relevantSuffixes.constEnd()) {
                        if (words.at(i).endsWith(iterRel.value())) {
                            iterDict = dictionary.find(words.at(i));
                            if (iterDict!=dictionary.end())
                                output.append(" Verb: "+words.at(i));
                            break;
                        }
                        ++iterRel;
                    }
                }
                //ENDE FALLBACK WEGEN GIDIYORUZ

            }
        }
    }
    return output;
}


void MainWindow::readHunspell()
{
    //lies türkische Affix-Datei ein
    QStringList aff;
    QFile affFile("Turkish.aff");
    if (affFile.open(QFile::ReadOnly)) {
        //lies txt in QStringList
        QTextStream textStream(&affFile);
        textStream.setCodec("UTF-8");
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                aff.append(line);
        }
        affFile.close();
    }

    //verarbeite Affix-Datei
    //i=6, um die LANG, SET, TRY und FLAG Anweisungen zu übergehen
    //Da in der türkischen Hunspell Datei nur Suffixe mit einer Möglichkeit
    //vorhanden sind, können die dazwischen liegenden Zeilen übersprungen werden
    for (int i=6; i<aff.size(); i+=3) {
        QStringList content=aff.at(i).split(' ', QString::SkipEmptyParts);
        suffixes.insert(content.at(1).toInt(), content.at(3).toUtf8().constData());
    }

    //lies türkisches Dictionary ein
    QStringList dic;
    QFile dicFile("Turkish.dic");
    if (dicFile.open(QFile::ReadOnly)) {
        //lies txt in QStringList
        QTextStream textStream(&dicFile);
        textStream.setCodec("UTF-8");
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                dic.append(line);
        }
        dicFile.close();
    }

    //verarbeite Dictionary-Datei
    //i=1, da die erste Zeile die Anzahl der Einträge beinhaltet
    for (int i=1; i<dic.size(); i++) {
        QStringList content=dic.at(i).split('/', QString::SkipEmptyParts);
        //wenn es suffix-Referenzen gibt, füge diese an, sonst setze Wert auf -1
        if (content.size()>1)
            dictionary.insert(content.at(0), content.at(1));
        else {
            int intToString=-1;
            dictionary.insert(content.at(0), QString::number(intToString));
        }
    }

    //lies zu beachtende Suffixe-Datei ein
    QStringList relSuf;
    QFile relSufFile("relevanteSuffixe.txt");
    if (relSufFile.open(QFile::ReadOnly)) {
        //lies txt in QStringList
        QTextStream textStream(&relSufFile);
        textStream.setCodec("UTF-8");
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else {
                if ((line != "") && (line.at(0)!= '#'))
                    relSuf.append(line);
            }
        }
        relSufFile.close();
    }

    //finde IDs aus aff-Datei (suffixe QStringList)
    for (int i=0; i<suffixes.length(); i++) {
        for (int j=0; j<relSuf.length(); j++) {
            if (suffixes.at(i).endsWith(relSuf.at(j))) {
                relevantSuffixes.insert(i, suffixes.at(i));
            }
        }
    }

    //lies türkischen Stemmer ein
    QStringList stems;
    QFile stemFile("generated.dict");
    if (stemFile.open(QFile::ReadOnly)) {
        //lies txt in QStringList
        QTextStream textStream(&stemFile);
        textStream.setCodec("UTF-8");
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                stems.append(line);
        }
        stemFile.close();
    }

    //verarbeite Stem-Datei
    //i=4, da die ersten 4 Zeilen Notizen beinhalten
    for (int i=4; i<stems.size(); i++) {
        QStringList content=stems.at(i).split('\t', QString::SkipEmptyParts);
        stemList.insert(content.at(0), content.at(1));
    }
}
