#ifndef PROTOCOLS_PROTOCOLS_H
#define PROTOCOLS_PROTOCOLS_H

#include <Windows.h>

#include "definitions.h"


#define MAX_MESSAGE_LENGTH 512
#define MAX_BUFFER_SIZE MAX_MESSAGE_LENGTH - sizeof(MESSAGE_TYPE) - sizeof(DWORD)

typedef enum _MESSAGE_TYPE
{
   LOGIN_REQUEST, LOGOUT_REQUEST, ENCRYPT_REQUEST,
   OK_RESPONSE, FAILURE_RESPONSE, ENCRYPT_RESPONSE, INITIALISE_CONNECTION_RESPONSE
}MESSAGE_TYPE;


typedef struct _MESSAGE
{
   MESSAGE_TYPE messageType;
   DWORD messageLength;
   CHAR messageBuffer[MAX_BUFFER_SIZE];

}MESSAGE, *PMESSAGE;

void DestroyMessage(PMESSAGE* message);

STATUS CreateMessage(PMESSAGE* message);

STATUS WriteMessage(HANDLE pipeHandle, PMESSAGE message);

STATUS ReadMessage(HANDLE pipeHandle, PMESSAGE* message);

STATUS AddBuffer(PMESSAGE message, LPCSTR buffer, BOOL addNullTerminator);

STATUS FullCopyBuffer(PMESSAGE message, LPCSTR buffer, DWORD bufferSize);

#endif //PROTOCOLS_PROTOCOLS_H
