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

  Supports OS-specific themes
  */

class Themer
{
public:
    Themer();
    ~Themer();

    QList<QString> themes();

    QString themePath();
    void themePath(QString p_path);

    void theme(QString p_theme);
    QString theme();

    QString getThemeCSS(QString p_which);

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
