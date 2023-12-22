#ifndef GLOBAL_H
#define GLOBAL_H

#include <QOpenGLVersionFunctionsFactory>
#include <qopenglfunctions_4_3_core.h>

#define OPENGLFUNCTIONS QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(QOpenGLContext::currentContext())

#define DATA_PORT 5000
#define CONFIG_PORT 4000

#define BUFFER_SIZE 1024

extern int clientSocket;

#endif // GLOBAL_H
