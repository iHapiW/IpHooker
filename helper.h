#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <winhttp.h>

void getIp(HINTERNET hSession, LPSTR buffer, LPDWORD bytesRead);

void sendWebhook(HINTERNET hSession, LPCWSTR webhookId, LPCWSTR webhookToken, LPCSTR buffer, DWORD size);