#include "dictentry.h"
#include <QList>
#include <QString>
#include <QVector>

DictEntry::DictEntry()
{
}

DictEntry::DictEntry(QString word)
{
    this->word=word;
    suffixRef={};
}

DictEntry::~DictEntry()
{

}
QString DictEntry::getWord() const
{
    return word;
}

void DictEntry::setWord(const QString &value)
{
    word = value;
}
QVector<int> DictEntry::getSuffixRef() const
{
    return suffixRef;
}

void DictEntry::addToSuffixRef(int toAdd)
{
    suffixRef.push_back(toAdd);
}

//void DictEntry::setSuffixRef(const QVector<int> &value)
//{
//    suffixRef = value;
//}

