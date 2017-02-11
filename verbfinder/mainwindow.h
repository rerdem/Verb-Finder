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
#include <QChar>
#include <QMap>

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

    QStringList suffixes;

    QMap <QString, QString> dictionary;
    QMap <int, QString> relevantSuffixes;
    QMap <QString, QString> stemList;

    QVector<QChar> letterStart;
    QVector<int> letterIndex;

    void createInterface();
    void readHunspell();
    QString checkVerb(QString satz);

private slots:
    void about();
    void reset();
    void analyze();
};

#endif // MAINWINDOW_H
