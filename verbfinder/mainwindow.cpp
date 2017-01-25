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
//#include <cstdlib> //itoa()
#include <QDebug>

#include "mainwindow.h"
#include "dictentry.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
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
//            outputTextEdit->append(sentences.at(i).toUtf8().constData()+checkVerb(sentences.at(i).toUtf8()));
            outputTextEdit->append(sentences.at(i).toUtf8().constData());
        }

        readHunspell();

        outputTextEdit->append("\nResults:\n");
        QString arrowString = " -> ";
        for (int i=0; i<sentences.size(); i++) {
            outputTextEdit->append(sentences.at(i).toUtf8().constData()+arrowString+checkVerb(sentences.at(i).toUtf8()));
//            outputTextEdit->append(sentences.at(i).toUtf8().constData());
        }
    }
}

QString MainWindow::checkVerb(QString satz) {
    QString output="";
//    qDebug() << satz;
    //entferne Punkt am Ende und alles in Kleinbuchstaben
    if ((satz.at(satz.length()-1)==',') || (satz.at(satz.length()-1)=='.') || (satz.at(satz.length()-1)=='!') || (satz.at(satz.length()-1)=='?')) {
        satz.remove(satz.length()-1,1);
        satz=satz.toLower();
    }
//    qDebug() << satz;

    //gibt es ein Komma, splitte und überprüfe beide Teile separat
    if (satz.contains(',')) {
        QStringList parts=satz.split(',', QString::SkipEmptyParts);
        for (int i=0; i<parts.length(); i++) {
            output.append(checkVerb(parts.at(i)));
        }
    }
    else {
        QStringList words=satz.split(' ', QString::SkipEmptyParts);
        //durchlaufe für jedes Wort die gesamte dic und überprüfe, ob es ein Verb ist
        //BONUS TO DO: speichere beim Einlesen der dic ab welcher Position die Anfangsbuchstaben stehen
        //um nur die Worte mit dem passenden Anfangsbuchstaben durchsuchen zu müssen
        for (int i=0; i<words.length(); i++) {
            bool matchFound=false;
            int tempLetterIndex=findLetterNumber(words.at(i).at(0).toLower());
            for (int j=tempLetterIndex; j<dictEntries.length(); j++) {
                if (matchFound) break;
                DictEntry tempEntry=dictEntries.at(j);
                QVector<int> tempEntryRefs=tempEntry.getSuffixRef();
                QString tempWordPlusSuffix="";
                if (tempEntryRefs.first()==-1) {
                    if (words.at(i)==tempEntry.getWord()) {
                        output.append(" Verb: "+words.at(i));
                        matchFound=true;
                    }
                }
                else {
                    for (int k=0; k<relevantSuffixes.length(); k++) {
                        if (matchFound) break;
                        if (tempEntryRefs.contains(relevantSuffixes.at(k))) {
                            tempWordPlusSuffix=tempEntry.getWord()+suffixes.at(relevantSuffixes.at(k));
                            if (words.at(i)==tempWordPlusSuffix) {
                                if (matchFound) break;
                                output.append(" Verb: "+words.at(i));
                                matchFound=true;
                            }
                        }
                    }

//                    for (int k=0; k<tempEntryRefs.length(); k++) {
//                        if (matchFound) break;
//                        tempWordPlusSuffix=tempEntry.getWord()+suffixes.at(tempEntryRefs.at(k));
////                        outputTextEdit->append(tempWordPlusSuffix+"\n");
//                        if (words.at(i)==tempWordPlusSuffix) {
//                            if (matchFound) break;
//                            output.append("Verb: "+words.at(i));
//                            matchFound=true;
//                        }
//                    }
                }
            }
        }
    }
    return output;
}

int MainWindow::findLetterNumber(QChar letter)
{
    for (int i=0; i<letterStart.length(); i++)
        if (letterStart.at(i)==letter)
            return letterIndex.at(i);

    //der Platzhalter für i und ı liegt auf Position 9
    return 9;
}

void MainWindow::readHunspell()
{
    //lies türkische Affix-Datei ein
    QStringList aff;
    QFile affFile("Turkish.aff");
    if (affFile.open(QFile::ReadOnly))
    {
//        outputTextEdit->append("aff open");
        //lies txt in QStringList
        QTextStream textStream(&affFile);
        textStream.setCodec("UTF-8");
        while (true)
        {
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
//        qDebug() << aff.at(i);
        QStringList content=aff.at(i).split(' ', QString::SkipEmptyParts);
//        qDebug() << content.at(1).toInt() << content.at(3).toUtf8().constData();
        suffixes.insert(content.at(1).toInt(), content.at(3).toUtf8().constData());
    }

    //lies türkisches Dictionary ein
    QStringList dic;
    QFile dicFile("Turkish.dic");
    if (dicFile.open(QFile::ReadOnly)) {
//        outputTextEdit->append("dic open");
        //lies txt in QStringList
        QTextStream textStream(&dicFile);
        textStream.setCodec("UTF-8");
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                dic.append(line);
        }
        dicFile.close();
    }

    //init letterStart Prozess und Variablen
    QChar letterCheck='a';
    letterStart.append(letterCheck);
    letterIndex.append(0);

    //verarbeite Dictionary-Datei
    //i=1, da die erste Zeile die Anzahl der Einträge beinhaltet
    for (int i=1; i<dic.size(); i++) {
        if (dic.at(i).at(0).toLower()!=letterCheck) {
            if ((letterCheck=='h') || (letterCheck=='#')) {
                if (letterCheck=='#') {
                    if (dic.at(i).at(0).toLower()=='j') {
                        letterCheck=dic.at(i).at(0).toLower();
                        letterStart.append(letterCheck);
                        letterIndex.append(i-1);
                    }
                }
                else {
                    letterCheck='#';
                    letterStart.append(letterCheck);
                    letterIndex.append(i-1);
                }
            }

            else {
                //            qDebug() << dic.at(i).toUtf8().toLower().at(0);
                letterCheck=dic.at(i).at(0).toLower();
                letterStart.append(letterCheck);
                letterIndex.append(i-1);
            }
        }

        QStringList content=dic.at(i).split('/', QString::SkipEmptyParts);
        DictEntry tempEntry(content.at(0));
        //wenn es suffix-Referenzen gibt, füge diese an, sonst setze Wert auf -1
        if (content.size()>1) {
            QStringList references=content.at(1).split(',');
            for (int j=0; j<references.size(); j++) {
                tempEntry.addToSuffixRef(references.at(j).toInt());
            }
        }
        else
            tempEntry.addToSuffixRef(-1);
//        qDebug() << tempEntry.getWord() << tempEntry.getSuffixRef();
        dictEntries.push_back(tempEntry);
    }

    //lies zu beachtende Suffixe-Datei ein
    QStringList relSuf;
    QFile relSufFile("relevanteSuffixe.txt");
    if (relSufFile.open(QFile::ReadOnly))
    {
//        outputTextEdit->append("suf open");
        //lies txt in QStringList
        QTextStream textStream(&relSufFile);
        textStream.setCodec("UTF-8");
        while (true)
        {
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
//            qDebug() << suffixes.at(i) << relSuf.at(j);
//            qDebug() << suffixes.at(i).endsWith(relSuf.at(j));
            if (suffixes.at(i).endsWith(relSuf.at(j))) {
//            if (suffixes.at(i).contains(relSuf.at(j))) {
                relevantSuffixes.append(i);
            }
        }
    }

//    outputTextEdit->append("\nSuffixe:\n");
//    for (int i=0; i<suffixes.size(); i++) {
//        outputTextEdit->append(sentences.at(i).toUtf8().constData()+checkVerb(sentences.at(i).toUtf8()));
//        outputTextEdit->append(suffixes.at(i).toUtf8().constData());
//    }

}


QString MainWindow::checkVerb2(QString satz)
{
    //lies türkische Nomen ein
    QStringList nomenList;
    QFile textFile("TR_nomen_dict.txt");
    if (textFile.open(QFile::ReadOnly))
    {
        //lies txt in QStringList
        QTextStream textStream(&textFile);
        textStream.setCodec("UTF-8");
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                nomenList.append(line);
        }
        textFile.close();

        //füge Personalpronomen hinzu
        nomenList.append("ben");
        nomenList.append("sen");
        nomenList.append("o");
        nomenList.append("biz");
        nomenList.append("siz");
        nomenList.append("onlar");

    }

    //finde Verb
    QStringList worte = satz.split(" ");
    bool langerSatz=false;
    QString ausgabe="Keine Ausgabe";
    if ((worte.length()-1)!= 0) {
        QString erstes = worte.value( 0 );
        langerSatz=true;
    }
    QString verb = worte.value( worte.length() - 1 );

    //Präsens
    if (verb.contains("yorum")) {
        QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 1. Pers. Singular";
        if (langerSatz) {
            bool subject=false;
            for (int i=0; i<nomenList.length(); i++) {
                if (worte.value(0).toLower()==nomenList.value(i).toLower())
                {
                    ausgabe.append(" Subjekt: ");
                    ausgabe.append(worte.value(0));
                    subject=true;
                    break;
                }
            }
            if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
        }
        else ausgabe.append(" Subjekt im Verb enthalten.");
        return ausgabe;
    }
    else {
        if (verb.contains("yorsun")) {
            QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 2. Pers. Singular";
            if (langerSatz) {
                bool subject=false;
                for (int i=0; i<nomenList.length(); i++) {
                    if (worte.value(0).toLower()==nomenList.value(i).toLower())
                    {
                        ausgabe.append(" Subjekt: ");
                        ausgabe.append(worte.value(0));
                        subject=true;
                        break;
                    }
                }
                if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
            }
            else ausgabe.append(" Subjekt im Verb enthalten.");
            return ausgabe;
        }
        else {
            if (verb.contains("yoruz")) {
                QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 1. Pers. Plural";
                if (langerSatz) {
                    bool subject=false;
                    for (int i=0; i<nomenList.length(); i++) {
                        if (worte.value(0).toLower()==nomenList.value(i).toLower())
                        {
                            ausgabe.append(" Subjekt: ");
                            ausgabe.append(worte.value(0));
                            subject=true;
                            break;
                        }
                    }
                    if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
                }
                else ausgabe.append(" Subjekt im Verb enthalten.");
                return ausgabe;
            }
            else {
                if (verb.contains("yorsunuz")) {
                    QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 2. Pers. Plural";
                    if (langerSatz) {
                        bool subject=false;
                        for (int i=0; i<nomenList.length(); i++) {
                            if (worte.value(0).toLower()==nomenList.value(i).toLower())
                            {
                                ausgabe.append(" Subjekt: ");
                                ausgabe.append(worte.value(0));
                                subject=true;
                                break;
                            }
                        }
                        if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
                    }
                    else ausgabe.append(" Subjekt im Verb enthalten.");
                    return ausgabe;
                }
                else {
                    if (verb.contains("yorlar")) {
                        QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 3. Pers. Plural";
                        if (langerSatz) {
                            bool subject=false;
                            for (int i=0; i<nomenList.length(); i++) {
                                if (worte.value(0).toLower()==nomenList.value(i).toLower())
                                {
                                    ausgabe.append(" Subjekt: ");
                                    ausgabe.append(worte.value(0));
                                    subject=true;
                                    break;
                                }
                            }
                            if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
                        }
                        else ausgabe.append(" Subjekt im Verb enthalten.");
                        return ausgabe;
                    }
                    else {
                        if (verb.contains("yor")) {
                            QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 3. Pers. Singular";
                            if (langerSatz) {
                                bool subject=false;
                                for (int i=0; i<nomenList.length(); i++) {
                                    if (worte.value(0).toLower()==nomenList.value(i).toLower())
                                    {
                                        ausgabe.append(" Subjekt: ");
                                        ausgabe.append(worte.value(0));
                                        subject=true;
                                        break;
                                    }
                                }
                                if (!subject) ausgabe.append(" Subjekt im Verb enthalten oder nicht gefunden.");
                            }
                            else ausgabe.append(" Subjekt im Verb enthalten.");
                            return ausgabe;
                        }
                        else {
                            QString ausgabe=" Kein grammatikalisch korrekter Satz des türkischen Präsens.";
                            return ausgabe;
                        }
                    }
                }
            }
        }
    }

    return ausgabe;
}
