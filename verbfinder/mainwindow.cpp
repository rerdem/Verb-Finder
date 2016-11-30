#include <QString>
#include <QDir>
#include <QtCore>
#include <QtGui>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <vector>
#include <cstdlib> //itoa()
#include "mainwindow.h"


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
    QMessageBox::information(this, "About", "A program that can find the subject in Turkish sentences.");
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
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                stringList.append(line);
        }
        textFile.close();

        for (int i=0; i<stringList.size(); i++)
        {
            outputTextEdit->append(stringList.at(i).toUtf8().constData()+checkVerb(stringList.at(i).toUtf8()));
        }
    }
}

QString MainWindow::checkVerb(QString satz)
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
