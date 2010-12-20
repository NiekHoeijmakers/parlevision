#ifndef PLVNIEK_PLUGIN_GLOBAL_H
#define PLVNIEK_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLV_NIEK_PLUGIN_LIBRARY)
#  define PLV_NIEK_EXPORT Q_DECL_EXPORT
#else
#  define PLV_NIEK_EXPORT Q_DECL_IMPORT
#endif

#endif // PLV_NIEK_PLUGIN_GLOBAL_H
