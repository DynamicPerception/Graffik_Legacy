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

#include "globaloptions.h"

GlobalOptions::GlobalOptions(QObject *parent) : QObject(parent) {

    m_mutex   = new QMutex;
    m_stopErr = true;
    m_display = Options::Imperial;
    m_theme   = Singleton<Themer>::Instance().theme();

}

GlobalOptions::~GlobalOptions() {
    delete m_mutex;
}

/** Set Display Type

   Set display type to one of:

   Options::Imperial
   Options::Metric
   Options::Steps

   This method is thread-safe.
   */

void GlobalOptions::display(int p_mode) {

    if( m_display == p_mode )
        return;

    m_mutex->lock();
    m_display = p_mode;
    m_mutex->unlock();

    emit optionsChanged();
}

/** Return current display type

  This method is thread-safe
  */

int GlobalOptions::display() {
    m_mutex->lock();
    int ret = m_display;
    m_mutex->unlock();
    return ret;
}

/** Set Stop on Error Parameter

  This method is thread safe
  */

void GlobalOptions::stopOnErr(bool p_mode) {
    if( m_stopErr == p_mode )
        return;

    m_mutex->lock();
    m_stopErr = p_mode;
    m_mutex->unlock();

    emit optionsChanged();
}

/** Return Stop on Error Parameter

  This method is thread-safe
  */

bool GlobalOptions::stopOnErr() {
    m_mutex->lock();
    bool ret = m_stopErr;
    m_mutex->unlock();
    return ret;
}


/** Return Current Theme Name

  This method is thread-safe, as long as no other
  object attempts to interact with the themer directly
  */
QString GlobalOptions::theme() {
    m_mutex->lock();
    QString theme = m_theme;
    m_mutex->unlock();
    return theme;
}

/** Set Current Theme Name

  This method is thread-safe, as long as no other
  object attempts to interact with the themer directly
  */
void GlobalOptions::theme(QString p_theme) {
    m_mutex->lock();
    Themer* themer = &Singleton<Themer>::Instance();
    themer->theme(p_theme);
    m_theme = p_theme;
    m_mutex->unlock();
    emit optionsChanged();
}


/** Set All Hotkeys at Once

  Sets all hotkeys at once.

  This method is thread-safe.

  param p_list
  QHash<HotKeys, QString> containing each hot key you want to enable
  and the key to activate

*/

void GlobalOptions::hotkeys(QHash<int, QString> p_list) {
    m_mutex->lock();
    m_hotKeys = p_list;
    m_mutex->unlock();
    emit optionsChanged();
}

/** Get All Hotkeys

  This method is thread-safe.

  @return
  QHash<int, QString> containing all defined hotkeys.  May be an
  empty list
  */

QHash<int, QString> GlobalOptions::hotkeys() {

    QHash<int, QString> ret;

    m_mutex->lock();
    ret = m_hotKeys;
    m_mutex->unlock();

    return ret;
}

/** Set a Specific Hotkey

  Sets the key character for a specific hotkey.

  This method is thread-safe.

  @param p_key
  A valid HotKeys member

  @param p_val
  The key value to activate the hotkey

 */

void GlobalOptions::hotkey(int p_key, QString p_val) {

    m_mutex->lock();

    m_hotKeys.insert(p_key, p_val);

    m_mutex->unlock();

    emit optionsChanged();
}

/** Get a Specific Hotkey

  Returns the activation key for the specified hotkey. If
  no key is specified, will return an empty string.

  This method is thread-safe.

  @param p_key
  A valid HotKeys member
  */

QString GlobalOptions::hotkey(int p_key) {

    QString ret("");

    m_mutex->lock();

    if( m_hotKeys.contains(p_key) ) {
        ret = m_hotKeys.value(p_key);
    }

    m_mutex->unlock();

    return ret;
}
