#ifndef GLOBAL_H
#define GLOBAL_H

#include <QOpenGLVersionFunctionsFactory>
#include <qopenglfunctions_4_3_core.h>

#define OPENGLFUNCTIONS QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(QOpenGLContext::currentContext())


#endif // GLOBAL_H
