#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")


using namespace std;

int main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't initialize winsock!" << endl;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create socket!" << endl;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // bind to any address

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN); // SOMAXCONN - number of max connections

	sockaddr_in client;
	int clientSize = sizeof(client);

	int sent;
	char buf[4096];
	
	// Plan to use core html files instead of vanilla strings, but the purpose is important
	string linkOneHtml = "<html>\n"
		"<body>\n"
		"<h1>Hello from Link One</h1>\n"
		"<a href='/link2'>Link 2</a> | <a href='/link3'>Link 3</a>\n"
		"</body>\n"
		"</html>\n\n";

	string linkTwoHtml = "<html>\n"
		"<body>\n"
		"<h1>Hello from Link Two</h1>\n"
		"<a href='/link1'>Link 1</a> | <a href='/link3'>Link 3</a>\n"
		"</body>\n"
		"</html>\n\n";

	string linkThreeHtml = "<html>\n"
		"<body>\n"
		"<h1>Hello from Link Three</h1>\n"
		"<a href='/link1'>Link 1</a> | <a href='/link2'>Link 2</a>\n"
		"</body>\n"
		"</html>\n\n";

	string linkHomeHtml = "<html>\n"
		"<body>\n"
		"<h1>Hello from home page</h1>\n"
		"<a href='/link1'>Link 1</a> | <a href='/link2'>Link 2</a> | <a href='/link3'>Link 3</a>\n"
		"</body>\n"
		"</html>\n\n";

	string headers;
	int contentLength;

	// Wait for a connection
	while (1)
	{
		SOCKET clientSocket;
		if ((clientSocket = accept(listening, (sockaddr*)&client, &clientSize)) == -1)
		{
			cerr << "Can't create client socket!" << endl;
		}

		// Check what we received

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		string tempBuf = buf;
		std::istringstream f(buf);
		std::string line;
		std::getline(f, line);

		if (line.find("GET") != string::npos) {
			if (line.find("link1") != string::npos)
			{
				headers = "HTTP/1.1 200 OK\n"
					"Content-length: " + std::to_string(linkOneHtml.length()) + "\n"
					"Content-type: text/html\n"
					"\n";
				string linkOneContents = headers + linkOneHtml;
				strcpy_s(buf, linkOneContents.c_str());
			}
			else if (line.find("link2") != string::npos)
			{
				headers = "HTTP/1.1 200 OK\n"
					"Content-length: " + std::to_string(linkTwoHtml.length()) + "\n"
					"Content-type: text/html\n"
					"\n";
				string linkTwoContents = headers + linkTwoHtml;

				strcpy_s(buf, linkTwoContents.c_str());
			}
			else if (line.find("link3") != string::npos)
			{
				headers = "HTTP/1.1 200 OK\n"
					"Content-length: " + std::to_string(linkThreeHtml.length()) + "\n"
					"Content-type: text/html\n"
					"\n";
				string linkThreeContents = headers + linkThreeHtml;
				strcpy_s(buf, linkThreeContents.c_str());
			}
			else {
				headers = "HTTP/1.1 200 OK\n"
					"Content-length: " + std::to_string(linkHomeHtml.length()) + "\n"
					"Content-type: text/html\n"
					"\n";
				string homePageContent = headers + linkHomeHtml;
				strcpy_s(buf, homePageContent.c_str());
			}

			sent = send(clientSocket, buf, strlen(buf), 0);

			cout << string(buf, 0, strlen(buf)) << endl;
		}
		else {
			cout << "Only GET requests are allowed for now!" << endl;
		}
		// Close the socket
		closesocket(clientSocket);

	}

	// Cleanup winsock
	WSACleanup();
}