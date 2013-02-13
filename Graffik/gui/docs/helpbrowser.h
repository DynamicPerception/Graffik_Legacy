/*

  Graffik Motion Control Application

  Copyright (c) 2011-2012 Dynamic Perception

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

#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QTextBrowser>
#include <QtHelp/QHelpEngine>
#include <QVariant>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    HelpBrowser(QHelpEngine* c_help, QWidget *parent = 0);
    
    QVariant loadResource(int type, const QUrl &url);

signals:
    
public slots:
    
private:
   QHelpEngine* m_help;
};

#endif // HELPBROWSER_H
