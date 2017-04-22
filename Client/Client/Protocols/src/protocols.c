#include <Strsafe.h>

#include "../include/protocols.h"


STATUS WriteMessage(HANDLE pipeHandle, PMESSAGE message)
{
   STATUS status;
   BOOL successOperation;
   DWORD cbWritten;
   DWORD cbToWrite;


   cbToWrite = 0;
   cbWritten = 0;
   successOperation = TRUE;
   status = EXIT_SUCCESS_STATUS;


   cbToWrite = sizeof(MESSAGE_TYPE) + sizeof(DWORD) + sizeof(message->messageBuffer);

   successOperation = WriteFile(
      pipeHandle,
      message,
      cbToWrite,
      &cbWritten,
      NULL
   );

   if (!successOperation || cbToWrite != cbWritten)
   {
      status = GetLastError();
   }

   FlushFileBuffers(pipeHandle);
   return status;
}

STATUS ReadMessage(HANDLE pipeHandle, PMESSAGE* message)
{
   STATUS status;
   BOOL successOperation;
   DWORD cbReadBytes;
   PMESSAGE tempMessage;


   cbReadBytes = 0;
   successOperation = TRUE;
   status = EXIT_SUCCESS_STATUS;
   tempMessage = NULL;

   if (NULL == message)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempMessage);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   successOperation = ReadFile(
      pipeHandle,
      tempMessage,
      MAX_MESSAGE_LENGTH,
      &cbReadBytes,
      NULL
   );

   if (GetLastError() == ERROR_MORE_DATA)
   {
      successOperation = TRUE;
   }

   if (!successOperation)
   {
      status = GetLastError();
      goto EXIT;
   }

   *message = tempMessage;

EXIT:
   if (!SUCCESS(status))
   {
      free(tempMessage);
      tempMessage = NULL;
   }
   return status;
}

void DestroyMessage(PMESSAGE* message)
{
   DWORD i;

   i = 0;

   if (NULL == message)
   {
      goto EXIT;
   }

   if (NULL == *message)
   {
      goto EXIT;
   }

   free(*message);
   *message = NULL;

EXIT:
   return;
}


STATUS CreateMessage(PMESSAGE* message)
{
   STATUS status;
   DWORD i;
   PMESSAGE tempMessage;

   i = 0;
   status = EXIT_SUCCESS_STATUS;
   tempMessage = NULL;

   tempMessage = (PMESSAGE)malloc(sizeof(MESSAGE));
   if (NULL == tempMessage)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   for (i = 0; i < sizeof(tempMessage->messageBuffer); ++i)
   {
      tempMessage->messageBuffer[i] = '\0';
   }
   tempMessage->messageType = OK_RESPONSE;
   tempMessage->messageLength = 0;
   *message = tempMessage;

EXIT:
   return status;
}


STATUS AddBuffer(PMESSAGE message, LPCSTR buffer, BOOL addNullTerminator)
{
   STATUS status;
   size_t buffSize;
   HRESULT res;
   DWORD idx;

   idx = 0;
   res = S_OK;
   buffSize = 0;
   status = EXIT_SUCCESS_STATUS;

   res = StringCchLengthA(buffer, STRSAFE_MAX_CCH, &buffSize);
   if (res != S_OK)
   {
      status = res;
      goto EXIT;
   }
   if (addNullTerminator)
   {
      buffSize++;
   }

   if (message->messageLength + buffSize > sizeof(message->messageBuffer))
   {
      status = MESSAGE_TOO_LARGE;
      goto EXIT;
   }

   for (idx = 0; idx < buffSize; ++idx)
   {
      message->messageBuffer[idx + message->messageLength] = buffer[idx];
   }
   message->messageLength += buffSize;


EXIT:
   return status;
}


STATUS FullCopyBuffer(PMESSAGE message, LPCSTR buffer, DWORD bufferSize)
{
   STATUS status;
   DWORD idx;

   idx = 0;
   status = EXIT_SUCCESS_STATUS;


   if (message->messageLength + bufferSize > sizeof(message->messageBuffer))
   {
      status = MESSAGE_TOO_LARGE;
      goto EXIT;
   }

   for (idx = 0; idx < bufferSize; ++idx)
   {
      message->messageBuffer[idx + message->messageLength] = buffer[idx];
   }
   message->messageLength += bufferSize;


EXIT:
   return status;
}
