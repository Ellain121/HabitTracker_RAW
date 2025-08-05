#pragma once

#include <QtCore/qglobal.h>

#if defined(APP_CORE_LIBRARY)
#  define APP_CORE_EXPORT Q_DECL_EXPORT
#else
#  define APP_CORE_EXPORT Q_DECL_IMPORT
#endif
