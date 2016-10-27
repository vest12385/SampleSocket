#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //for close
#include <error.h>
#include <syslog.h>
#include <regex.h>
regex_t regex;
int reti;
void spilt(char **arr, char *str, const char *key)
{
	char *s = strtok(str, key);

	while(s != NULL)
	{
		*arr++ = s;
		s = strtok(NULL, key);
	}
}
int main()
{
	int sockfd;
	struct sockaddr_in dest;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "socket error");
		perror(buff);
		exit(1);
	}

	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(3000);
	dest.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd, (struct sockaddr*)&dest, sizeof(dest)) < 0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "bind error");
		perror(buff);
		exit(1);
	}

	if(listen(sockfd, 20) < 0)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "listen error");
		perror(buff);
		exit(1);
	}

	while(1)
	{
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);

		if((clientfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "accept error");
			perror(buff);
			exit(1);
  		}
		printf("One clinet connect!!\n");
		//send(clientfd, buffer, sizeof(buffer), 0);
		char hello[10] = "Hi client!";
		char buffer1[1024];
		write(clientfd, hello, sizeof(hello));
		char buffer[50] = "Please input your equation";
//=========================== verify equation address ======================
		regcomp(&regex,"^-\\?\\([0-9]\\+\\|[0-9]\\+\\.[0-9]\\+\\) \\(+\\|-\\|\\*\\|/\\) -\\?\\([0-9]\\+\\|[0-9]\\+\\.[0-9]\\+\\)$",0);
		do
		{
			write(clientfd, buffer, sizeof(buffer));
			recv(clientfd, buffer1, sizeof(buffer1), 0);
			printf("Client send:%s\n", buffer1);
			reti = regexec(&regex, buffer1, 0, NULL, 0);
		}while(reti);
		regfree(&regex);
//==========================================================================
		char *token[3];
		spilt(token, buffer1, " ");
		double answer = 0.0;
		switch(*token[1])
		{
			case '+':
				answer = atof(token[0]) + atof(token[2]);
				break;
			case '-':
				answer = atof(token[0]) - atof(token[2]);
				break;
			case '*':
				answer = atof(token[0]) * atof(token[2]);
				break;
			case '/':
				answer = atof(token[0]) / atof(token[2]);
				break;
			default:
				answer = 0;
				break;
		}
		char answerchr[15];
		sprintf(answerchr, "%s%1.2f", "The answer is ", answer);
		write(clientfd, answerchr, sizeof(buffer1));
		close(clientfd);
	}
	close(sockfd);
	return 0;
}
