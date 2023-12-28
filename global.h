#ifndef GLOBAL_H
#define GLOBAL_H

#include <QOpenGLVersionFunctionsFactory>
#include <qopenglfunctions_4_3_core.h>

#define OPENGLFUNCTIONS QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(QOpenGLContext::currentContext())

#define SERVER_PORT 3000
#define CONFIG_PORT 3001
#define BROADCAST_PORT 3002
#define IMAGE_PORT 3003

#define DATA_PORT 5000
#define COMMAND_PORT 6000

#define BUFFER_SIZE 1024

extern int clientSocket;
extern int serverSocket;
extern int configSocket;
extern int broadcastSocket;
extern int calibrationImageSocket;

#endif // GLOBAL_H
