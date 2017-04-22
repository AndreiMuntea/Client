#include "../include/client.h"
#include <Strsafe.h>
#include "../../Utils/include/strutils.h"
#include "../../Protocols/include/protocols.h"


STATUS CreatePipeInstance(LPSTR pipeName, PHANDLE handle)
{
   STATUS status;
   HANDLE pipeHandle;
   DWORD dwMode;
   BOOL successOperation;

   successOperation = FALSE;
   dwMode = 0;
   pipeHandle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;

   if (handle == NULL)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   while (1)
   {
      pipeHandle = CreateFileA(
         pipeName, // pipe name 
         GENERIC_READ | GENERIC_WRITE,
         0, // no sharing 
         NULL, // default security attributes
         OPEN_EXISTING, // opens existing pipe 
         0, // default attributes 
         NULL);

      if (pipeHandle != INVALID_HANDLE_VALUE)
      {
         break;
      }

      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
      if (GetLastError() != ERROR_PIPE_BUSY)
      {
         status = PIPE_FAILURE;
         goto EXIT;
      }

      // All pipe instances are busy, so wait for 20 seconds. 

      if (!WaitNamedPipeA(pipeName, 20000))
      {
         status = PIPE_FAILURE;
         goto EXIT;
      }
   }

   dwMode = PIPE_READMODE_MESSAGE;
   successOperation = SetNamedPipeHandleState(
      pipeHandle, // pipe handle 
      &dwMode, // new pipe mode 
      NULL, // don't set maximum bytes 
      NULL); // don't set maximum time 

   if (!successOperation)
   {
      status = PIPE_FAILURE;
      goto EXIT;
   }
   *handle = pipeHandle;

EXIT:
   if (!SUCCESS(status))
   {
      if (pipeHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(pipeHandle);
      }
   }
   return status;
}

STATUS CreateClient(PCLIENT* client, LPSTR userName, LPSTR password, LPSTR key, LPSTR inputFileName, LPSTR outputFileName, LPSTR pipeName)
{
   STATUS status;
   PCLIENT tempClient;
   LPSTR tempUserName;
   LPSTR tempPassword;
   LPSTR tempKey;
   LPSTR tempPipeName;
   HANDLE inputFileHandle;
   HANDLE outputFileHandle;
   HANDLE pipeHandle;

   tempPipeName = NULL;
   pipeHandle = INVALID_HANDLE_VALUE;
   inputFileHandle = INVALID_HANDLE_VALUE;
   outputFileHandle = INVALID_HANDLE_VALUE;
   tempUserName = NULL;
   tempPassword = NULL;
   tempKey = NULL;
   tempClient = NULL;
   status = EXIT_SUCCESS_STATUS;

   if (NULL == client)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CopyBuffer(&tempUserName, userName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CopyBuffer(&tempKey, key);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CopyBuffer(&tempPassword, password);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreatePipeName(&tempPipeName, pipeName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreatePipeInstance(tempPipeName, &pipeHandle);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   inputFileHandle = CreateFileA(
      inputFileName,
      GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
   );
   if (INVALID_HANDLE_VALUE == inputFileHandle)
   {
      status = GetLastError();
      goto EXIT;
   }

   outputFileHandle = CreateFileA(
      outputFileName,
      GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
   );
   if (INVALID_HANDLE_VALUE == inputFileHandle)
   {
      status = GetLastError();
      goto EXIT;
   }


   tempClient = (PCLIENT)malloc(sizeof(CLIENT));
   if (NULL == tempClient)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempClient->pipeHandle = pipeHandle;
   tempClient->inputFileHandle = inputFileHandle;
   tempClient->outputFileHandle = outputFileHandle;
   tempClient->key = tempKey;
   tempClient->password = tempPassword;
   tempClient->userName = tempUserName;

   *client = tempClient;

EXIT:
   if (!SUCCESS(status))
   {
      free(tempPassword);
      free(tempUserName);
      free(tempKey);
      if (pipeHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(pipeHandle);
      }
      if (inputFileHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(inputFileHandle);
      }
      if (outputFileHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(outputFileHandle);
      }
      DestroyClient(&tempClient);
   }
   free(tempPipeName);
   return status;
}

void DestroyClient(PCLIENT* client)
{
   if (NULL == client)
   {
      goto EXIT;
   }

   if (NULL == *client)
   {
      goto EXIT;
   }

   free((*client)->key);
   (*client)->key = NULL;

   free((*client)->userName);
   (*client)->userName = NULL;

   free((*client)->password);
   (*client)->password = NULL;

   if ((*client)->pipeHandle != INVALID_HANDLE_VALUE)
   {
      CloseHandle((*client)->pipeHandle);
   }

   if ((*client)->inputFileHandle != INVALID_HANDLE_VALUE)
   {
      CloseHandle((*client)->inputFileHandle);
   }

   if ((*client)->outputFileHandle != INVALID_HANDLE_VALUE)
   {
      CloseHandle((*client)->outputFileHandle);
   }

   free(*client);
   *client = NULL;

EXIT:
   return;
}

STATUS CreateEncryptRequest(LPSTR buffer, DWORD bufferSize, PMESSAGE* request)
{
   STATUS status;
   PMESSAGE tempMessage;

   status = EXIT_SUCCESS_STATUS;
   tempMessage = NULL;

   if (NULL == request)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempMessage);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }
   tempMessage->messageType = ENCRYPT_REQUEST;

   status = FullCopyBuffer(tempMessage, buffer, bufferSize);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }   
   
   *request = tempMessage;

EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempMessage);
   }
   return status;
}

STATUS CreateLoginRequst(PCLIENT client, PMESSAGE* request)
{
   STATUS status;
   PMESSAGE tempMessage;

   status = EXIT_SUCCESS_STATUS;
   tempMessage = NULL;

   if (NULL == request)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempMessage);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }
   tempMessage->messageType = LOGIN_REQUEST;

   status = AddBuffer(tempMessage, client->userName, TRUE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = AddBuffer(tempMessage, " ", FALSE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = AddBuffer(tempMessage, client->password, TRUE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = AddBuffer(tempMessage, " ", FALSE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = AddBuffer(tempMessage, client->key, TRUE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   *request = tempMessage;

EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempMessage);
   }
   return status;
}

STATUS CreateLogoutRequest(PCLIENT client, PMESSAGE* request)
{
   STATUS status;
   PMESSAGE tempMessage;

   status = EXIT_SUCCESS_STATUS;
   tempMessage = NULL;

   if (NULL == request)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempMessage);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }
   tempMessage->messageType = LOGOUT_REQUEST;

   status = AddBuffer(tempMessage, client->userName, TRUE);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   *request = tempMessage;
EXIT:
   return status;
}

STATUS HandleLogoutRequest(PCLIENT client)
{
   STATUS status;
   PMESSAGE response;
   PMESSAGE request;

   status = EXIT_SUCCESS_STATUS;
   response = NULL;
   request = NULL;

   status = CreateLogoutRequest(client, &request);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = WriteMessage(client->pipeHandle, request);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = ReadMessage(client->pipeHandle, &response);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   if (response->messageType != OK_RESPONSE)
   {
      printf("LOGOUT FAILED!\n");
      status = GetStatus(response->messageBuffer);
   }

EXIT:
   DestroyMessage(&response);
   DestroyMessage(&request);
   return status;
}

STATUS HandleLoginRequest(PCLIENT client)
{
   STATUS status;
   PMESSAGE response;
   PMESSAGE request;

   status = EXIT_SUCCESS_STATUS;
   response = NULL;
   request = NULL;

   status = CreateLoginRequst(client, &request);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = WriteMessage(client->pipeHandle, request);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = ReadMessage(client->pipeHandle, &response);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   if (response->messageType != OK_RESPONSE)
   {
      printf("LOGIN FAILED!\n");
      status = GetStatus(response->messageBuffer);
   }

EXIT:
   DestroyMessage(&response);
   DestroyMessage(&request);
   return status;
}



STATUS HandleEncryption(LPVOID argument)
{
   PCLIENT client;
   STATUS status;
   PMESSAGE message;
   PMESSAGE response;
   LPSTR buffer;
   DWORD bytesRead;
   DWORD bytesWrote;
   DWORD pointerStart;
   DWORD pointerEnd;

   
   bytesRead = 0;
   bytesWrote = 0;
   buffer = NULL;
   response = NULL;
   status = EXIT_SUCCESS_STATUS;
   client = (PCLIENT)argument;
   message = NULL;
   pointerStart = FILE_BEGIN;
   pointerEnd = FILE_BEGIN;

   buffer = (LPSTR)malloc(MAX_BUFFER_SIZE * sizeof(CHAR));
   if(NULL == buffer)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   while(1)
   {
      SetFilePointer(client->inputFileHandle, pointerStart, NULL, FILE_BEGIN);

      if (!ReadFile(client->inputFileHandle, buffer, MAX_BUFFER_SIZE, &bytesRead, NULL))
      {
         status = FILE_IO_ERROR;
         goto EXIT;
      }
      pointerStart = SetFilePointer(client->inputFileHandle, 0, NULL, FILE_CURRENT);

      if (bytesRead == 0)
      {
         break;
      }

      status = CreateEncryptRequest(buffer, bytesRead, &message);
      if(!SUCCESS(status))
      {
         goto EXIT;
      }

      status = WriteMessage(client->pipeHandle, message);
      if (!SUCCESS(status))
      {
         goto EXIT;
      }
      status = ReadMessage(client->pipeHandle, &response);
      if (!SUCCESS(status))
      {
         goto EXIT;
      }

      if(response->messageType != ENCRYPT_RESPONSE)
      {
         status = GetStatus(response->messageBuffer);
         goto EXIT;
      }

      SetFilePointer(client->outputFileHandle, pointerEnd, NULL, FILE_BEGIN);
      if(!WriteFile(client->outputFileHandle, response->messageBuffer, response->messageLength, &bytesWrote,NULL))
      {
         status = FILE_IO_ERROR;
         goto EXIT;
      }

      if(bytesWrote != response->messageLength )
      {
         status = FILE_IO_ERROR;
         goto EXIT;
      }

      pointerEnd = SetFilePointer(client->inputFileHandle, 0, NULL, FILE_CURRENT);

   }


EXIT:
   free(buffer);
   DestroyMessage(&message);
   DestroyMessage(&response);
   return status;
}





STATUS BeginSession(PCLIENT client)
{
   STATUS status;

   status = EXIT_SUCCESS_STATUS;

   status = HandleLoginRequest(client);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = HandleEncryption(client);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = HandleLogoutRequest(client);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }
EXIT:
   return status;
}
