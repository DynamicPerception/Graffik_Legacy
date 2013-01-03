#ifndef SINGLETHEMER_H
#define SINGLETHEMER_H

#include <QString>
#include "core/Utilities/singleton.h"
#include "themer.h"

/** Helper Namespace for Themer Singleton

  This namespace provides two helper functions for using Themer as a singleton. This library is NOT
  thread-safe.

  The primary purpose of this namespace is to allow most read-only objects to get their stylesheets
  with a single function call.  For the case of those objects which must interact directly with the
  Themer object, to configure or read configuration, you should use the Singleton templates to access
  the object instead, e.g.:

  @code
  Themer* theme = &Singleton<Themer>::Instance();
  @endcode

  @author
  C. A. Church
  */

namespace SingleThemer {

    QString getStyleSheet(QString p_name);
}
#endif // SINGLETHEMER_H
