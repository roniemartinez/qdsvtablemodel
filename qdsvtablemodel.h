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

#ifndef QDSVTABLEMODEL_H
#define QDSVTABLEMODEL_H

#include <QAbstractTableModel>

//a template-based matrix to store 2D data
template<class T>
class QDsvMatrix {
    QList<QList<T>> m_data;
    int m_rowCount;
    int m_columnCount;

public:
    QDsvMatrix(int row = 0, int column = 0):
        m_rowCount(row), m_columnCount(column)
    {
        if (rowCount() > 0 && columnCount() > 0) {
            for (int i = 0; i < rowCount(); ++i) {
                QList<T> rowData;
                for (int j = 0; j < columnCount(); ++j) {
                    rowData.insert(j, T());
                }
                m_data.insert(i, rowData);
            }
        }
    }

    T at(int row, int column) const {
        if (row >= rowCount() || column >= columnCount())
            return T();
        QList<T> rowData = m_data.at(row);
        return rowData.value(column);
    }

    bool setValue(int row, int column, T value) {
        if (column >= columnCount()) {
            m_columnCount = column + 1;
            for (int i = 0; i < rowCount(); ++i) {
                QList<T> rowData = m_data.at(i);
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                m_data[i] = rowData;
            }
        }
        if (row >= rowCount()) {
            m_rowCount = row + 1;
            while (m_data.size() < rowCount()) {
                QList<T> rowData;
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                m_data.append(rowData);
            }
        }

        m_data[row][column] = value;
        return true;
    }

    int rowCount() const {
        return m_rowCount;
    }

    int columnCount() const {
        return m_columnCount;
    }

    void clear() {
        m_rowCount = 0;
        m_columnCount = 0;
        m_data.clear();
    }

    void append(const QList<T> &value) {
        if (value.size() > columnCount())
            m_columnCount = value.size();
        m_data.append(value);
        ++m_rowCount;
    }
};

//QDsvTableModel class
class QDsvTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QDsvTableModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool loadFromFile(const QString &fileName, const QChar &delim = 0);
    bool loadFromData(const QByteArray &data, const QString &format);

    enum DsvWriteFlag {             //use when writing data to file
        UseDoubleQuotesIfNeeded,    //embed data on double quotes ONLY when needed
        AlwaysUseDoubleQuotes       //strictly embed data within double quotes
    };

    bool save(const QString &fileName, const QChar &delim = 0,
              DsvWriteFlag flag = UseDoubleQuotesIfNeeded);

signals:
    
public slots:

private:
    QDsvMatrix<QString> dsvMatrix;
    
};

#endif // QDSVTABLEMODEL_H
