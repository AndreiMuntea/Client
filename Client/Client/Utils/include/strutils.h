#ifndef UTILS_STRUTILS_H
#define UTILS_STRUTILS_H

#include <Windows.h>

#include "../../Protocols/include/definitions.h"

/*
 * Creates the pipe name appending the specified prefix;
 * @params: dest   - a LPSR*, the destination string
 *          source - a LPCSTR, the source string, if NULL, the default value will be used
 * @returns: STATUS - EXIT_SUCCESS_STATUS if successful
 *                  - error code otherwise 
 */
STATUS CreatePipeName(LPSTR* dest, LPCSTR source);

STATUS CopyBuffer(LPSTR* dest, LPSTR source);

DWORD GetStatus(LPSTR buffer);
#endif //UTILS_STRUTILS_H