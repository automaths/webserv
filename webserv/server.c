#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>

#define PORT 8080

int main()
{

    // INITING SERVER

    int server_fd;
    struct sockaddr_in address;


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        return (perror("In socket"), 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
        return (perror("In bind"), 0);
    if (listen(server_fd, 10) < 0)
        return (perror("In listen"), 0);

    // SERVER RUNNING

    char *test_cat_start = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
    char *test_cat_end = "' />\n</head>\n</html>";
    int new_socket;
    int addrlen = sizeof(address);
    fd_set sockSet;                  /* Set of socket descriptors for select() */
    struct timeval timeout;       /* Timeout for select() */
    int    i;
    int    result;
    int size_read;

    int iNumClients = 0;
    int clients[1000];

    while (1)
    {
        /* Zero socket descriptor vector and set for server sockets */
        /* This must be reset every time select() is called */
        FD_ZERO(&sockSet);
        FD_SET(server_fd, &sockSet);
        for (i=0; i < iNumClients; i++)
            FD_SET(clients[i], &sockSet);

        /* Timeout specification */
        /* This must be reset every time select() is called */
        timeout.tv_sec = 1;       /* timeout (secs.) */
        timeout.tv_usec = 0;            /* 0 microseconds */

        result = select(10, &sockSet, NULL, NULL, &timeout);

        if (result == -1)
            printf("an error occured during select");
        else if(result > 0) /* ie, if a socket is ready */
        {
            printf("Found at least one ready socket\n");
            // test this specific socket to see if it was one of the ones that was set
            if (FD_ISSET(server_fd, &sockSet))
            {
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
                    return (perror("In accept"), 0);
                else
                {
                    clients[iNumClients] = new_socket;
                    ++iNumClients;
                }
            }
            
            /* Now test the client sockets */
            for (i=0; i < iNumClients; i++)
                if (FD_ISSET(clients[i], &sockSet) && clients[i] != -1)
                {
                    /* do whatever it takes to read the socket and handle the client */
                    /* Please note that this will involve reassociating the socket   */
                    /*     with the client record                                    */
                    char buffer[30000] = {0};
                    size_read = read( clients[i] , buffer, 30000);
                    printf("%s\n",buffer );

                    int k = 0;

                    write(clients[i] , test_cat_start , strlen(test_cat_start));
                    while (buffer[k + 5] >= '0' && buffer[k + 5] <= '9')
                        write(clients[i], &buffer[k++ + 5], 1);
                    write(clients[i] , test_cat_end , strlen(test_cat_end));

                    close (clients[i]);
                    clients[i] = -1;
                }

        }

        /* else result == 0, no socket is ready to be read, */
        /*    so ignore them and move on.                    */

        printf("The number of clients is %d\n", iNumClients);
    }
    return 0;

}