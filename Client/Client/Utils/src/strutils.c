#include <Strsafe.h>

#include "../include/strutils.h"
#include "../include/constants.h"


STATUS CreatePipeName(LPSTR* dest, LPCSTR source)
{
   STATUS status;
   LPSTR temp;
   LPCSTR copySource;
   size_t finalSize;
   size_t prefixSize;
   size_t sourceSize;
   HRESULT result;

   result = S_OK;
   prefixSize = 0;
   sourceSize = 0;
   finalSize = 0;
   temp = NULL;
   copySource = NULL;
   status = EXIT_SUCCESS_STATUS;

   if (NULL == dest)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   copySource = source;
   if (NULL == source)
   {
      copySource = PIPE_DEFAULT_NAME;
   }

   // Source size
   if (StringCchLengthA(copySource,STRSAFE_MAX_CCH, &sourceSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   // Prefix size;
   if (StringCchLengthA(PIPE_PREFIX,STRSAFE_MAX_CCH, &prefixSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   // Size of string containing full name of pipe
   finalSize = sourceSize + prefixSize + 1;
   if (finalSize > PIPE_NAME_MAX_SIZE)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   temp = (LPSTR)malloc(finalSize * sizeof(CHAR));
   if (NULL == temp)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   // Copy the pipe prefix
   result = StringCchCopyA(temp, finalSize, PIPE_PREFIX);
   if (result != S_OK)
   {
      status = result;
      goto EXIT;
   }

   // Copy the actual pipe name
   result = StringCchCatA(temp, finalSize, copySource);
   if (result != S_OK)
   {
      status = result;
      goto EXIT;
   }

   *dest = temp;

EXIT:
   if (!SUCCESS(status))
   {
      free(temp);
      temp = NULL;
   }

   return status;
}

STATUS CopyBuffer(LPSTR* dest, LPSTR source)
{
   STATUS status;
   HRESULT res;
   size_t buffSize;
   LPSTR tempBuffer;

   tempBuffer = NULL;
   buffSize = 0;
   status = EXIT_SUCCESS_STATUS;
   res = S_OK;

   if(NULL == dest)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   res = StringCchLengthA(source, STRSAFE_MAX_CCH, &buffSize);
   if (res != S_OK)
   {
      status = res;
      goto EXIT;
   }
   buffSize++;

   tempBuffer = (LPSTR)malloc(buffSize * sizeof(CHAR));
   if(NULL == tempBuffer)
   {
      status = BAD_ALLOCATION; 
      goto EXIT;
   }

   res = StringCchCopyA(tempBuffer, buffSize, source);
   if(res != S_OK)
   {
      status = res;
      goto EXIT;
   }

   *dest = tempBuffer;


EXIT:
   if(!SUCCESS(status))
   {
      free(tempBuffer);
   }
   return status;
}

DWORD GetDigit(CHAR c)
{
   if (c >= '0' && c<='9')
   {
      return c - '0';
   }
   return c - 'A' + 10;
}

DWORD GetStatus(LPSTR buffer)
{
   DWORD res, idx, size;

   res = 0;
   idx = 0;
   size = strlen(buffer);

   for(idx = 2; idx < strlen(buffer); ++idx)
   {
      res *= 16;
      res += GetDigit(buffer[idx]);
   }

   return res;
}
