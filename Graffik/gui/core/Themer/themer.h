#ifndef THEMER_H
#define THEMER_H

#include <QCoreApplication>
#include <QList>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QFile>
#include <QTextStream>

/** Dynamic Themer

  Provides the ability to control dynamic themeing at run-time, including user-generated CSS and image files.

  Themes must exist in their own directories under the root theme path, include a theme.ini file, and include
  all required QSS files.  OS-specific theme files can be loaded when needed by adding a suffix to the name
  before the .qss extension, e.g.:

  main_osx.qss
  main_win.qss
  main_linux.qss
  main_elinux.qss

  The theme.ini file must contain the name of the theme, author, and source information.

  It is highly recommended to only use this library as a singleton, otherwise you will perform many more
  disk reads than required as numerous UI element spin up, and read the list of themes, look for valid
  ones.  The SingleThemer namespace has been provided to assist with this.

  This class is NOT thread safe, as it should only be used by QWidget-derived objects, which will all
  render in the main GUI thread.

  @section themechange Watching for Changes
  For any object with a lifetime wherein the theme may change during that lifetime, you must watch for the
  themeChanged() signal, and then re-collect the CSS and perform any necessary polish/unpolish functionality.

  @author
  C. A. Church
  */

class Themer : public QObject
{
    Q_OBJECT

public:
    explicit Themer(QObject* parent = 0);
    ~Themer();

    QList<QString> themes();

    QString themePath();
    void themePath(QString p_path);

    void theme(QString p_theme);
    QString theme();

    QString getThemeCSS(QString p_which);

signals:
    void themeChanged();

private:
    QString m_themePath;
    QHash<QString, QString>* m_themeList;
    QString m_curTheme;

    void _getThemeList();
    void _loadTheme(QString p_path);
    QString _osSuffix();
    QString _fileContents(QString p_path);

};

#endif // THEMER_H
