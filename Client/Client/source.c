#include <Windows.h> 

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include <stdio.h>

#include "Client/include/client.h"

int main(int argc, char* argv[])
{

   STATUS status;
   PCLIENT client;
   LPSTR userName;
   LPSTR password;
   LPSTR inputFileName;
   LPSTR key;
   LPSTR outputFileName;
   LPSTR pipeName;

   userName = NULL;
   password = NULL;
   key = NULL;
   inputFileName = NULL;
   outputFileName = NULL;
   pipeName = NULL;
   client = NULL;
   status = EXIT_SUCCESS_STATUS;

   if(argc < 4)
   {
      printf("Invalid number of arguments!\n");
      status = -1;
      goto EXIT;
   }

   userName = argv[1];
   password = argv[2];
   inputFileName = argv[3];
   key = password;
   outputFileName = inputFileName;
   pipeName = NULL;

   if(argc >= 5)
   {
      key = argv[4];
   }

   if(argc >= 6)
   {
      outputFileName = argv[5];
   }

   if(argc >= 7)
   {
      pipeName = argv[6];
   }

   status = CreateClient(&client, userName, password, key, inputFileName, outputFileName, pipeName);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = BeginSession(client);


 EXIT:
   DestroyClient(&client);
   _CrtDumpMemoryLeaks();
   printf("EXITCODE : %d\n", status);
   return status;
}
