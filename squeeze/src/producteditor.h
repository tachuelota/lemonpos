/***************************************************************************
 *   Copyright (C) 2007-2009 by Miguel Chavez Gamboa                       *
 *   miguel@lemonpos.org                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *

 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#ifndef PRODUCTEDITOR_H
#define PRODUCTEDITOR_H

#include <KDialog>
#include <QDate>
#include <QtGui>
#include <QPixmap>
#include <QtSql>
#include "ui_producteditor.h"

enum returnType {statusNormal=998, statusMod=999};

class ProductEditorUI : public QFrame, public Ui::productEditor
{
  Q_OBJECT
  public:
    ProductEditorUI( QWidget *parent=0 );
};

class ProductEditor : public KDialog
{
  Q_OBJECT
  public:
    ProductEditor( QWidget *parent=0, bool newProduct = false  );
    ~ProductEditor();

    qulonglong getCode()     { return ui->editCode->text().toULongLong(); };
    QString getDescription() { return ui->editDesc->text(); };
    double  getStockQty()    { return ui->editStockQty->text().toDouble(); };
    int     getCategoryId();
    int     getMeasureId();
    QString getCategoryStr(int c);
    QString getMeasureStr(int c);
    double  getCost()        { return ui->editCost->text().toDouble(); };
    double  getTax1()        { return ui->editTax->text().toDouble(); };
    double  getTax2()        { return ui->editExtraTaxes->text().toDouble(); };
    double  getPrice()       { return ui->editFinalPrice->text().toDouble(); };
    qulonglong getPoints()   { return ui->editPoints->text().toULongLong(); };
    QPixmap getPhoto()       { qDebug()<<"pixmap Size:"<<pix.size(); return pix; };
    

    void    populateCategoriesCombo();
    void    populateMeasuresCombo();

    void    setDb(QSqlDatabase database);
    void    setCode(qulonglong c)      {ui->editCode->setText(QString::number(c)); };
    void    setDescription(QString d)  {ui->editDesc->setText(d); };
    void    setStockQty(double q)      {ui->editStockQty->setText(QString::number(q)); };
    void    setCategory(QString str);
    void    setCategory(int i);
    void    setMeasure(QString str);
    void    setMeasure(int i);
    void    setCost(double c)          {ui->editCost->setText(QString::number(c)); };
    void    setTax1(double t)          {ui->editTax->setText(QString::number(t)); };
    void    setTax2(double t)          {ui->editExtraTaxes->setText(QString::number(t)); };
    void    setPrice(double p)         {ui->editFinalPrice->setText(QString::number(p)); };
    void    setPoints(qulonglong p)    {ui->editPoints->setText(QString::number(p)); };
    void    setPhoto(QPixmap p);
    
    void    disableCode()              { ui->editCode->setReadOnly(true); };
    void    enableCode()               { ui->editCode->setReadOnly(false); modifyCode=true;};
    
private slots:
    void    changePhoto();
    void    changeCode();
    void    calculatePrice();
    void    checkIfCodeExists();
    void    checkFieldsState();
protected slots:
    virtual void slotButtonClicked(int button);
  private:
    ProductEditorUI *ui;
    QSqlDatabase db;
    QPixmap pix;
    returnType status;
    bool modifyCode;
};

#endif
