#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QDataStream>

struct Data {
    int id;
    QString str;
    QList<QPointF> points;
};

QDataStream &operator << (QDataStream &out, const Data &data) {
    out << data.id << data.str << data.points;
    return out;
}

QDataStream &operator >> (QDataStream &in, Data &data) {
    in >> data.id >> data.str >> data.points;
    return in;
}

#endif // DATA_H
