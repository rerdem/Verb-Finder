#ifndef DICTENTRY_H
#define DICTENTRY_H

#include <QList>
#include <QString>
#include <QVector>

class DictEntry
{
public:
    DictEntry();
    DictEntry(QString word);
    ~DictEntry();

    QString getWord() const;
    void setWord(const QString &value);

    QVector<int> getSuffixRef() const;
//    void setSuffixRef(const QVector<int> &value);
    void addToSuffixRef(int toAdd);

private:
    QString word;
    QVector<int> suffixRef;
};

#endif // DICTENTRY_H
