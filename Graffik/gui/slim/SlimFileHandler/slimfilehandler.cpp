#include "slimfilehandler.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "core/ErrorDialog/errordialog.h"

SlimFileHandler::SlimFileHandler(SlimCommandParser *c_parse, CommandHistoryModel *c_hist)
{
}

SlimFileHandler::~SlimFileHandler() {


}

/** Static readFile

  Static method, to allow for one-line call to read a file of slim script
  and process the commands in it.  Immediately processes each command in
  file.

  @param p_file
  The path to the file to open

  @param p_parse
  A pointer to a valid SlimCommandParser object

  @param p_hist
  A pointer to a valid CommandHistoryModel object

  @param p_showErr
  Whether or not to show loading errors via an ErrorDialog after loading the file.  Defaults to true

  */

void SlimFileHandler::readFile(QString p_file, SlimCommandParser *p_parse, CommandHistoryModel *p_hist, bool p_showErr) {

        // open requested file, read each line, and send to command parser
    QFile scriptFile(p_file);
    QString line;
    QString errors;
    bool errorOccur = false;

    slimCommand lineCmd;

    if( scriptFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        QTextStream text( &scriptFile );

        while(! text.atEnd() ) {
            line = text.readLine();

            if( line.isEmpty() )
                continue;

            SlimFileHandler::_trimLeadingSpaces(line);

                // ignore comments

            if( line[0] == '#' )
                continue;

                // parse the line and execute the command

            try {
                lineCmd = p_parse->parse(line);
            }
            catch (int e) {
                qDebug() << "SFH: readFile: Caught Exception" << e;
                QString erMsg;

                if( e == SLIM_ERR_ARG )
                    erMsg = "An invalid argument was provided for the command";
                else if( e == SLIM_ERR_ARGS )
                    erMsg = "Invalid number of arguments provided to command";
                else if( e == SLIM_ERR_NOCMD )
                    erMsg = "Unknown command";
                else if( e == SLIM_ERR_NODEV )
                    erMsg = "The specified device was not found";
                else if( e == SLIM_ERR_EMPTY )
                    erMsg = "No command was input";

                errors.append("ERROR: \n\t" + line + "\n\t" + erMsg + "\n");
                errorOccur = true;
                continue;
            }

            p_hist->insertRow(lineCmd);
        }

        scriptFile.close();
    }
    else {
        errors.append("ERROR: Could not open file ");
        errors.append(p_file);
        errors.append(": ");
        errors.append( scriptFile.error() );
        errorOccur = true;
    }

    if( p_showErr && errorOccur ) {
        ErrorDialog erDlg;
        erDlg.setError(errors);
        erDlg.exec();
    }
}


void SlimFileHandler::_trimLeadingSpaces(QString &line) {
    int i = 0;
    while( i < line.length() && ( line[i] == ' ' || line[i] == '\t' ) )
        i++;
    line.remove(0,i);
}
