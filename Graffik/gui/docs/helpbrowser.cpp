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

#include "helpbrowser.h"

HelpBrowser::HelpBrowser(QHelpEngine *c_help, QWidget *parent) :
    QTextBrowser(parent)
{
    m_help = c_help;
}

QVariant HelpBrowser::loadResource(int type, const QUrl &url) {
    if (url.scheme() == "qthelp")
        return QVariant(m_help->fileData(url));
    else
        return QTextBrowser::loadResource(type, url);
}
