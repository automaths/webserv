// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>

#define PORT 8080

int PerformSelect(int listeningSock, int clients[], int iNumClients)
{

    char *test_cat_start = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
    char *test_cat_end = "' />\n</head>\n</html>";

    fd_set sockSet;                  /* Set of socket descriptors for select() */
    struct timeval selTimeout;       /* Timeout for select() */
    int    i;
    int    iResult;
    int valread;

    /* Zero socket descriptor vector and set for server sockets */
    /* This must be reset every time select() is called */
    FD_ZERO(&sockSet);
    FD_SET(listeningSock, &sockSet);
    for (i=0; i < iNumClients; i++)
        FD_SET(clients[i], &sockSet);

    /* Timeout specification */
    /* This must be reset every time select() is called */
    selTimeout.tv_sec = 1;       /* timeout (secs.) */
    selTimeout.tv_usec = 0;            /* 0 microseconds */

    iResult = select(10, &sockSet, NULL, NULL, &selTimeout);

    if (iResult == -1)
    {
        printf("an error occured during select");
        /* an error occurred; process it (eg, display error message) */
    }
    else if(iResult > 0) /* ie, if a socket is ready */
    {
        // test this specific socket to see if it was one of the ones that was set
        // if (FD_ISSET(listeningSock, &sockSet))
        // {
        //     AcceptNewClient(listeningSock);
        // }
        
        /* Now test the client sockets */
        write(1, "idntifie", 7);
        for (i=0; i < iNumClients; i++)
            if (FD_ISSET(clients[i], &sockSet))
            {
                write(1, "has entered", 6);
                /* do whatever it takes to read the socket and handle the client */
                /* Please note that this will involve reassociating the socket   */
                /*     with the client record                                    */
                char buffer[30000] = {0};
                valread = read( clients[i] , buffer, 30000);
                printf("%s\n",buffer );

                int i = 0;

                write(clients[i] , test_cat_start , strlen(test_cat_start));
                while (buffer[i + 5] >= '0' && buffer[i + 5] <= '9')
                    write(clients[i], &buffer[i++ + 5], 1);
                write(clients[i] , test_cat_end , strlen(test_cat_end));

                return 1;
                // HandleClient(clients[i]);
            }
    }
    printf("the num clients is %d\n", clients[0]);
    return 0;

    /* else iResult == 0, no socket is ready to be read, */
    /*    so ignore them and move on.                    */

}







int main()
{
    int server_fd;
    int new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    char *test_html = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<body>\n<h1>Hello, World!</h1>\n</body>\n</html>";

    char *test_redir = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://google.com/' />\n</head>\n</html>";

    char *test_cat = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/101' />\n</head>\n</html>";

    char *test_cat_start = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
    char *test_cat_end = "' />\n</head>\n</html>";
   
    // char *test_cat_redir = " content='0; URL=https://http.cat/";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    int clients[1000];
    int j = 0;
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            clients[j] = new_socket;
            ++j;
        }

        while(PerformSelect(server_fd, clients, j) != 1)
            ;

        // char buffer[30000] = {0};
        // valread = read( new_socket , buffer, 30000);
        // printf("%s\n",buffer );

        // int i = 0;

        // write(new_socket , test_cat_start , strlen(test_cat_start));
        // while (buffer[i + 5] >= '0' && buffer[i + 5] <= '9')
        //     write(new_socket, &buffer[i++ + 5], 1);
        // write(new_socket , test_cat_end , strlen(test_cat_end));

        // printf("------------------Hello message sent-------------------");
        // close(new_socket);
    }
    return 0;
}