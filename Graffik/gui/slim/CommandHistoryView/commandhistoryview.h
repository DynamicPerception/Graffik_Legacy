/*

  Graffik Motion Control Application

  Copyright (c) 2011-2013 Dynamic Perception

 This file is part of Graffik.

    Graffik is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Graffik is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Graffik.  If not, see <http://www.gnu.org/licenses/>.

    */

#ifndef COMMANDHISTORYVIEW_H
#define COMMANDHISTORYVIEW_H

#include <QTableView>
#include <QList>
#include <QPixmap>
#include <QAbstractItemModel>

#include "Slim/CommandHistoryModel/commandhistorymodel.h"

class CommandHistoryView : public QTableView
{
    Q_OBJECT
    Q_PROPERTY(int colWidth0 READ colWidth0 WRITE setColWidth0 DESIGNABLE true)
    Q_PROPERTY(int colWidth1 READ colWidth1 WRITE setColWidth1 DESIGNABLE true)
    Q_PROPERTY(int colWidth2 READ colWidth2 WRITE setColWidth2 DESIGNABLE true)
    Q_PROPERTY(int colWidth3 READ colWidth3 WRITE setColWidth3 DESIGNABLE true)
    Q_PROPERTY(int colWidth4 READ colWidth4 WRITE setColWidth4 DESIGNABLE true)
    Q_PROPERTY(QPixmap errLED READ errLED WRITE setErrLED DESIGNABLE true)
    Q_PROPERTY(QPixmap okLED READ okLED WRITE setOkLED DESIGNABLE true)
    Q_PROPERTY(QPixmap queueLED READ queueLED WRITE setQueueLED DESIGNABLE true)
    Q_PROPERTY(QPixmap bcastLED READ bcastLED WRITE setBcastLED DESIGNABLE true)

public:
    explicit CommandHistoryView(QWidget *parent = 0);
    
    int sizeHintForColumn(int p_col);

    int colWidth0();
    int colWidth1();
    int colWidth2();
    int colWidth3();
    int colWidth4();

    void setColWidth0(int p_wid);
    void setColWidth1(int p_wid);
    void setColWidth2(int p_wid);
    void setColWidth3(int p_wid);
    void setColWidth4(int p_wid);

    QPixmap errLED();
    QPixmap okLED();
    QPixmap queueLED();
    QPixmap bcastLED();

    void setErrLED(QPixmap p_pix);
    void setOkLED(QPixmap p_pix);
    void setQueueLED(QPixmap p_pix);
    void setBcastLED(QPixmap p_pix);

    void setModel(QAbstractItemModel *model);

signals:
    
public slots:
    
private:
    QList<int> m_colWidths;
    bool m_modelSet;

    void _setColWidth(int p_col, int p_wid);

};

#endif // COMMANDHISTORYVIEW_H
