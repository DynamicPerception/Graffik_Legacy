#include "singlethemer.h"

/** Get Stylesheet for Named Item

  Returns the stylesheet for a named item, or an empty string if
  no stylesheet is found in the current theme.
  */
QString SingleThemer::getStyleSheet(QString p_name) {
       Themer* theme = &Singleton<Themer>::Instance();
       return theme->getThemeCSS(p_name);
}

