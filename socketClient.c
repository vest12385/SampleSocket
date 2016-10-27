#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <syslog.h>
#include <regex.h>
regex_t regex;
int reti;
void removeNewline(char * word)
{
	/*char *pos;
	if((pos=strchr(word, '\n')) != NULL)
		*pos = '\0';*/
	if(word[strlen(word) - 1] == '\n')
		word[strlen(word) - 1] = '\0';
}
int main()
{
//=========================== verify ip address ============================
	char *ip = (char*)malloc(sizeof(char)*17);
	regcomp(&regex, "^[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}$", 0);
	do
	{
		printf("Please input correct IP addresss:");
		fgets(ip, sizeof(char)*17, stdin );
		removeNewline(ip);
		reti = regexec(&regex, ip, 0, NULL, 0);
	}while(reti);
	regfree(&regex);
//==========================================================================

	int sockfd;
	struct sockaddr_in dest;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "socket error");
		perror(buff);
		exit(1);
	}

	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(3000);
	//dest.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(inet_pton(AF_INET, ip, &dest.sin_addr)<=0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "inet_pton error occured");
		perror(buff);
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) < 0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "connect error");
		perror(buff);
		exit(1);
	}

	char buffer[128];
	bzero(buffer, 128);
	char *resp = (char *)malloc(sizeof(char)*128);
	//recv(sockfd, buffer, sizeof(buffer), 0);
	read(sockfd, buffer, sizeof(buffer) - 1 );
	do
	{
		read(sockfd, buffer, sizeof(buffer) - 1 );
		printf("%s\n", buffer);
		fgets(resp, 128, stdin);
		removeNewline(resp);
		write(sockfd, resp, sizeof(char)*128);
	}while(!strstr(buffer, "answer"));
	close(sockfd);
	return 0;
}
