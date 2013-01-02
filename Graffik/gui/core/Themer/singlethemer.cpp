#include "singlethemer.h"

QString SingleThemer::getStyleSheet(QString p_name) {
       Themer* theme = &Singleton<Themer>::Instance();
       return theme->getThemeCSS(p_name);
}
