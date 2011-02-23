#ifndef CALLIGRAMOBILE_EXPORT_H
#define CALLIGRAMOBILE_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

/* We use _WIN32/_WIN64 instead of Q_OS_WIN so that this header can be used from C files too */
#if defined _WIN32 || defined _WIN64
#  ifndef CALLIGRAMOBILE_EXPORT
#     if defined(MAKE_CALLIGRAMOBILE_LIB)
/* We are building this library */
#       define CALLIGRAMOBILE_EXPORT KDE_EXPORT
#     else
/* We are using this library */
#       define CALLIGRAMOBILE_EXPORT KDE_IMPORT
#     endif
#  endif
#else /* UNIX */
#   define CALLIGRAMOBILE_EXPORT KDE_EXPORT
#endif

#endif //CALLIGRAMOBILE_EXPORT_H
