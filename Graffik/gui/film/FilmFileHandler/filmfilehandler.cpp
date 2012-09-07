#include "filmfilehandler.h"

FilmFileHandler::FilmFileHandler()
{
}

FilmFileHandler::~FilmFileHandler() {

}

/** Static Write File

  Writes a given set of FilmParameters to a file.  Persists a copy of OMfilmParams as returned
  by the FilmParameters object using the FilmParameters::getParamsCopy() method.

  @param p_file
  File to write to

  @param p_params
  A FilmParameters object pointer containing the parameters to persist to disk

  @param p_showErr
  Whether or not to report errors back to user (default = true)

*/
void FilmFileHandler::writeFile(QString p_file, FilmParameters* p_params, bool p_showErr) {

        // do not try to write to a file with no name
    if( p_file.isEmpty() )
        return;


    QFile filmFile(p_file);
    QString errors;
    bool errorOccur = false;


    if( filmFile.open(QIODevice::WriteOnly) ) {
        QDataStream data( &filmFile );
        data << p_params->getParamsCopy();
        filmFile.close();
    }
    else {
        errors.append("ERROR: Could not open file ");
        errors.append(p_file);
        errors.append(": ");
        errors.append( filmFile.error() );
        errorOccur = true;
    }

    if( p_showErr && errorOccur ) {
        ErrorDialog erDlg;
        erDlg.setError(errors);
        erDlg.exec();
    }

}

/** Static readFile


  Restores a persisted OMfilmParams structure from a file into a given FilmParameters object.

  @param p_file
  The path to the file to open

  @param p_params
  A pointer to a valid FilmParameters object

  @param p_showErr
  Whether or not to show loading errors via an ErrorDialog after loading the file.  Defaults to true

  */

void FilmFileHandler::readFile(QString p_file, FilmParameters* p_params, bool p_showErr) {

    if( p_file.isEmpty() )
        return;

    QFile filmFile(p_file);
    QString errors;
    bool errorOccur = false;


    if( filmFile.open(QIODevice::ReadOnly) ) {
        QDataStream data( &filmFile );
        OMfilmParams newParams;
        data >> newParams;
        filmFile.close();

        OMfilmParams* ptr = p_params->getParams();
        OMfilmParams* nparm = new OMfilmParams(newParams);
        *ptr = *nparm;
        p_params->releaseParams();

    }
    else {
        errors.append("ERROR: Could not open file ");
        errors.append(p_file);
        errors.append(": ");
        errors.append( filmFile.error() );
        errorOccur = true;
    }

    if( p_showErr && errorOccur ) {
        ErrorDialog erDlg;
        erDlg.setError(errors);
        erDlg.exec();
    }
}

