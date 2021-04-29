#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <iostream>
#include <string.h>

int main(int argc ,char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *res,*i;

    memset((void*) &hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc!=0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;

        return -1;
    }  

    for(i = res; i != NULL; i = i->ai_next)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);
                       
        std::cout << "Host: " << host << " Port: " << serv << std::endl;              
                      
    }

    freeaddrinfo(res);

    return 0;
}