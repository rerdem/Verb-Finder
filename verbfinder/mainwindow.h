#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <iostream>
using namespace std;
#include <fstream>
#include <QtGui>
#include <QPushButton>
#include <QtWidgets>
#include <QVector>
#include "dictentry.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QWidget *centralWidget;
    QGridLayout *mainBox;
    QTextEdit *outputTextEdit;
    QPushButton *analyzeButton;
    QPushButton *resetButton;

    QStringList suffixe;
    QVector<DictEntry> dictEntries;

    void createInterface();
    void readHunspell();
    QString checkVerb(QString satz);
    QString checkVerb2(QString satz);

private slots:
    void about();
    void reset();
    void analyze();
};

#endif // MAINWINDOW_H
