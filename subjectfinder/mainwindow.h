#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <iostream>
using namespace std;
#include <fstream>
#include <QtGui>
#include <QPushButton>


class mainwindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit mainwindow(QWidget *parent = 0);
    ~mainwindow();
    
private:
    QWidget *centralWidget;
    QGridLayout *mainBox;
    QTextEdit *outputTextEdit;
    QPushButton *analyzeButton;
    QPushButton *resetButton;

    void createInterface();


private slots:
    void about();
    void reset();
    void analyze();
};

#endif // MAINWINDOW_H
