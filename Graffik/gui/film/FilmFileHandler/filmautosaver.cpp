#include "filmautosaver.h"

#include <QDebug>


FilmAutoSaver::FilmAutoSaver(OMNetwork *c_net, FilmParameters *c_parms, QObject *parent) : QObject(parent) {

    m_net      = c_net;
    m_parms    = c_parms;
    m_filmPath = QCoreApplication::applicationDirPath() + "/" + FAS_AUTO_FILE;
    m_statPath = QCoreApplication::applicationDirPath() + "/" + FAS_STATE_FILE;
    m_curState = false;

    connect(m_parms, SIGNAL(paramsReleased()), this, SLOT(filmParamsReleased()));

}

/** Film Parameters Released Slot

  This slot saves a copy of the current film parameters to a temporary file
  each time the parameters are released.  This is done to ensure that an exact
  copy of the last working film can be loaded on start.

  */

void FilmAutoSaver::filmParamsReleased() {

    qDebug() << "FAS: Got Film Params Changed";

    FilmFileHandler::writeFile(m_filmPath, m_parms, false);

}

/** Play Status Changed Slot

  Writes the current status to a temporary file when the status is changed.

  Does nothing if the state provided is equivalent to the current internal state.
  Updates internal state when called.
  */

void FilmAutoSaver::playStatusChanged(bool p_state) {
    if( p_state == m_curState )
        return;

    m_curState = p_state;

    QFile stateFile(m_statPath);

    if( stateFile.open(QIODevice::WriteOnly) ) {
        QDataStream data(&stateFile);
        data << m_curState;
        stateFile.close();
    }
    else {
        qDebug() << "FAS: Could not write to stat file" << m_statPath << stateFile.errorString();
    }
}

/** Get Saved Film Status

  Returns the saved film status from the temporary file.  Does NOT set internal state
  within this class, and will always get the state from the file.  Call playStatusChanged()
  to actually change the internal state.

  If there is no temporary state file, the state will return as false.

  @return
  Saved play state
  */

bool FilmAutoSaver::savedStatus() {

    QFile stateFile(m_statPath);
    bool state = false;

    if( stateFile.open(QIODevice::ReadOnly) ) {
        QDataStream data(&stateFile);
        data >> state;
        stateFile.close();
    }
    else {
        qDebug() << "FAS: Could not read from stat file" << m_statPath << stateFile.errorString();
    }

    return state;
}


/** Load Film from Saved File */
void FilmAutoSaver::loadSavedFilm() {

    qDebug() << "FAS: Loading saved film" << m_filmPath;

    FilmFileHandler::readFile(m_filmPath, m_parms, m_net, false);
}
