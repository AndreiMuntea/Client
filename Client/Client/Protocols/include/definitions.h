#ifndef UTILS_DEFINITIONS_H
#define UTILS_DEFINITIONS_H

typedef long STATUS;

#define ERROR_BITS                  0xFFFF0000
#define WARNING_BITS                0x0000FFFF
#define INTERNAL_ERROR_BITS         0xF0000000

#define EXIT_SUCCESS_STATUS         0x00000000
#define SUCCESS(status)             (!((status) & (ERROR_BITS | WARNING_BITS)))
#define INTERNAL_ERROR(status)      ((status) & INTERNAL_ERROR_BITS)

#define BAD_ALLOCATION              0x10000000
#define NULL_POINTER                0x20000000
#define INVALID_PARAMETER           0x30000000
#define FILE_IO_ERROR               0x40000000
#define FILE_OPEN_ERROR             0x50000000
#define INSUFICIENT_BUFFER          0x60000000
#define CRITICAL_SECTION_FAILURE    0x70000000
#define PIPE_FAILURE                0x80000000
#define TIMEOUT_FAILURE             0x90000000
#define NOT_ENOUGH_MEMORY           0xA0000000
#define HANDLE_FAILURE              0xB0000000


#define IDEX_OUT_OF_BOUNDS          0x00010000
#define EMPTY_QUEUE                 0x00020000
#define SHUTDOWN_PENDING            0x00030000
#define CREATE_THREAD_FAILED        0x00040000
#define PENDING_SHUTDOWN            0x00050000
#define MESSAGE_TOO_LARGE           0x00060000


#define UNKNOWN_REQUEST             0x00000001
#define INVALID_NO_BUFFERS          0x00000002
#define USER_ALREADY_LOGGED_IN      0x00000003
#define INVALID_USER_OR_PASSWORD    0x00000004
#define SESSION_ALREADY_HAS_USER    0x00000005
#define USER_NOT_LOGGED_IN          0x00000006

#endif //UTILS_DEFINITIONS_H