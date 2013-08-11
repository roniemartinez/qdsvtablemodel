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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdsvtablemodel.h"
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->action_Open->trigger();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Save Delimiter-Separated Value"),
                                                    QDir::currentPath(), "All files (*.*);;"
                                                    "CSV (*.csv);; TSV (*.tsv)");
    if (!fileName.isEmpty()) {
        QDsvTableModel *model = new QDsvTableModel(this);
        QString extension = QFileInfo(QFile(fileName)).completeSuffix();
        if (extension.toLower() == "csv" || extension.toLower() == "tsv") //known file extensions
            model->loadFromFile(fileName);
        else {
            while (true) {
                bool ok;
                QString s = QInputDialog::getText(this, tr("Unknown File Format"),
                                                  tr("Enter Delimiter:"), QLineEdit::Normal,
                                                  "", &ok);
                if (ok && s.size() == 1) {
                    QChar delim = s.constData()[0];
                    model->loadFromFile(fileName, delim);
                    break;
                }
            }
        }
        ui->tableView->setModel(model);
    }
}

void MainWindow::on_action_Save_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Delimiter-Separated Value"),
                                                    QDir::currentPath(), "All files (*.*);;"
                                                    "CSV (*.csv);; TSV (*.tsv)");
    if (!fileName.isEmpty()) {
        QDsvTableModel *model = qobject_cast<QDsvTableModel*>(ui->tableView->model());
        if (!model)
            return;
        QString extension = QFileInfo(QFile(fileName)).completeSuffix();
        if (extension.toLower() == "csv" || extension.toLower() == "tsv") //known file extensions
            model->save(fileName, 0);
        else {
            while (true) {
                bool ok;
                QString s = QInputDialog::getText(this, tr("Unknown File Format"),
                                                  tr("Enter Delimiter:"), QLineEdit::Normal,
                                                  "", &ok);
                if (ok && s.size() == 1) {
                    QChar delim = s.constData()[0];
                    model->save(fileName, delim);
                    break;
                }
            }
        }
    }
}
