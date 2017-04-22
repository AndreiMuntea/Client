#ifndef CLIENT_CLINET_H
#define CLIENT_CLINET_H

#include <Windows.h>
#include "../../Protocols/include/definitions.h"


typedef struct _CLIENT
{
   LPSTR userName;
   LPSTR password;
   LPSTR key;
   HANDLE inputFileHandle;
   HANDLE outputFileHandle;
   HANDLE pipeHandle;
}CLIENT, *PCLIENT;

STATUS CreateClient(PCLIENT* client, LPSTR userName, LPSTR password, LPSTR key, LPSTR inputFileName, LPSTR outputFileName, LPSTR pipeName);

void DestroyClient(PCLIENT* client);

STATUS BeginSession(PCLIENT client);

#endif //CLIENT_CLINET_H