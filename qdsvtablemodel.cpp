/*************************************************************************
** Copyright (c) 2013 Ronie Martinez <ronmarti18@gmail.com>             **
** All rights reserved.                                                 **
**                                                                      **
** This library is free software; you can redistribute it and/or modify **
** it under the terms of the GNU Lesser General Public License as       **
** published by the Free Software Foundation; either version 2.1 of the **
** License, or (at your option) any later version.                      **
**                                                                      **
** This library is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    **
** Lesser General Public License for more details.                      **
**                                                                      **
** You should have received a copy of the GNU Lesser General Public     **
** License along with this library; if not, write to the Free Software  **
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA        **
** 02110-1301  USA                                                      **
*************************************************************************/

#include <QTextStream>
#include <QRegExp>
#include <QFileInfo>

#include "qdsvtablemodel.h"

QDsvTableModel::QDsvTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int QDsvTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dsvMatrix.rowCount();
}

int QDsvTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dsvMatrix.columnCount();
}

QVariant QDsvTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return dsvMatrix.at(index.row(), index.column());
    return QVariant();
}

bool QDsvTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
        return dsvMatrix.setValue(index.row(), index.column(), value.toString());
    return false;
}

Qt::ItemFlags QDsvTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

void checkString(QString &temp, QList<QString> &list, QDsvMatrix<QString> &data,
                 const QChar &delimiter, const QChar &character = 0)
{
    if(temp.count("\"")%2 == 0) {
        if (temp.startsWith(QChar('\"')) && temp.endsWith( QChar('\"') ) ) {
             temp.remove(QRegExp("^\"") );
             temp.remove(QRegExp("\"$") );
        }
        temp.replace("\"\"", "\"");
        list.append(temp);
        if (character != delimiter) {
            data.append(list);
            list.clear();
        }
        temp.clear();
    } else {
        temp.append(character);
    }
}


bool QDsvTableModel::loadFromFile(const QString &fileName, const QChar &delim)
{
    dsvMatrix.clear();
    QChar delimiter;
    QFile file(fileName);
    if (delim == 0) {
        QString extension = QFileInfo(file).completeSuffix();
        if (extension.toLower() == "csv")
            delimiter = QChar(',');
        else if (extension.toLower() == "tsv")
            delimiter = QChar('\t');
        else
            return false; //unknown file extension = unknown delimiter
    } else  if (delim == QChar('"'))
            return false; //the ONLY invalid delimiter is double quote (")
    else
        delimiter = delim;
    if (!file.isOpen())
        if (!file.open(QFile::ReadOnly|QFile::Text))
            return false;
    QString temp;
    QChar lastCharacter;
    QTextStream in(&file);
    QList<QString> row;
    while (true) {
        QChar character;
        in >> character;
        if (in.atEnd()) {
            if (lastCharacter == delimiter) //cases where last character is equal to the delimiter
                temp = "";
            checkString(temp, row, dsvMatrix, delimiter, QChar('\n'));
            break;
        } else if (character == delimiter || character == QChar('\n'))
            checkString(temp, row, dsvMatrix, delimiter, character);
        else {
            temp.append(character);
            lastCharacter = character;
        }
    }

    file.close();
    reset();
    return true;
}

bool QDsvTableModel::loadFromData(const QByteArray &data, const QString &format)
{
    //not yet implemented ...
    Q_UNUSED(data);
    Q_UNUSED(format);
    return false;
}

bool QDsvTableModel::save(const QString &fileName, const QChar &delim,
                          QDsvTableModel::DsvWriteFlag flag)
{
    QChar delimiter;
    QFile file(fileName);
    if (delim == 0) {
        QString extension = QFileInfo(file).completeSuffix();
        if (extension.toLower() == "csv")
            delimiter = QChar(',');
        else if (extension.toLower() == "tsv")
            delimiter = QChar('\t');
        else
            return false; //unknown file extension = unknown delimiter
    } else  if (delim == QChar('"'))
            return false; //the ONLY invalid delimiter is double quote (")
    else
        delimiter = delim;
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setGenerateByteOrderMark(true);
    for (int i = 0; i < dsvMatrix.rowCount(); ++i) {
        for (int j = 0; j < dsvMatrix.columnCount(); ++j) {
            QString s = dsvMatrix.at(i, j);
            if (s.contains("\"")) {
                s.replace("\"", "\"\"");
            }

            if (flag == UseDoubleQuotesIfNeeded) {
                if (s.contains("\"") || s.contains(" ") || s.contains(delim))
                    s = "\"" + s + "\"";
            } else {
                s = "\"" + s + "\"";
            }

            out << s;

            if (j == dsvMatrix.columnCount() - 1)
                out << endl;
            else
                out << delimiter;
        }
    }
    file.close();
    return true;
}





