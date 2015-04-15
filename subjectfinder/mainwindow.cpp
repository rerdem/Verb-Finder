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


mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent)
{
    //create gamefiles folder, if it doesn't exist
    /**
    QDir path;
    QDir().mkdir("gamefiles");
    path = QDir::currentPath() + QDir::separator() +"gamefiles";
    xmlpath=QDir::currentPath() + QDir::separator() + "gamefiles" + QDir::separator() + "adventure.xml";
    imgpath=QDir::currentPath() + QDir::separator() + "gamefiles" + QDir::separator() + + "images" + QDir::separator();
    **/

    //set QString to UTF-8 encoding for special characters
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    createInterface();
}


mainwindow::~mainwindow()
{
}


void mainwindow::createInterface()
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


void mainwindow::reset()
{
    outputTextEdit->clear();
}


void mainwindow::about()
{
    QMessageBox::information(this, "About", "A program that can find the subject in Turkish sentences.");
}


void mainwindow::analyze()
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

QString mainwindow::checkVerb(QString satz)
{
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
        if (langerSatz) ausgabe.append(" Langer Satz.");
        return ausgabe;
    }
    else {
        if (verb.contains("yorsun")) {
            QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 2. Pers. Singular";
            return ausgabe;
        }
        else {
            if (verb.contains("yoruz")) {
                QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 1. Pers. Plural";
                return ausgabe;
            }
            else {
                if (verb.contains("yorsunuz")) {
                    QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 2. Pers. Plural";
                    return ausgabe;
                }
                else {
                    if (verb.contains("yorlar")) {
                        QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 3. Pers. Plural";
                        return ausgabe;
                    }
                    else {
                        QString ausgabe=" Verb: "+verb.mid(0,verb.length()-1)+" -> Präs. 3. Pers. Singular";
                        return ausgabe;
                    }
                }
            }
        }
    }

    return ausgabe;
}
