#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H

class HttpClient
{
public:
	void begin();

	bool performGetRequest(char* path, char* content, int maxContentSize);
	bool performPostRequest(char* path, char* content, int maxContentSize);

private:
	void sendCommonHeader(char*, char*);
	bool connect();
	void disconnect();
	void println();
	void println(char*);
	void readln(char*, int);
	bool readblock(char*, int, char);
	void skipHeaders();
};

#endif
