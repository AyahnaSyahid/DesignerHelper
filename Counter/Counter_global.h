#ifndef COUNTER_GLOBAL_H
#define COUNTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(COUNTER_LIBRARY)
#define COUNTER_EXPORT Q_DECL_EXPORT
#else
#define COUNTER_EXPORT Q_DECL_IMPORT
#endif

#endif  // COUNTER_GLOBAL_H
