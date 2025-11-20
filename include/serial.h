#ifndef SERIAL_H
#define SERIAL_H
/** 
 * @file    serial.h
 * @brief   Simule le comportement d'une communication série via socket
 */
// ==================== INCLUDES ====================

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include <fcntl.h>

// ==================== COM management ====================
int serial_init();
int serial_accept();
void serial_close();
bool SetSocketBlockingEnabled(int fd, bool blocking);

// ==================== Message Management ====================
void send_response(const char *msg);
void trim_newline(char *s);
void receive_request(char buf[256], int len);

#endif // SERIAL_H