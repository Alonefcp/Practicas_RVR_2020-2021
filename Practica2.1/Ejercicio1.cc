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

    int rc = getaddrinfo(argv[1], NULL, &hints, &res);

    if(rc!=0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;

        return -1;
    }  

    //Muestro, el host, la familia y el tipo de socket de las posibles conexiones
    for(i = res; i != NULL; i = i->ai_next)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
                        
                       
        std::cout << host << " " << i->ai_family << " " << i->ai_socktype << std::endl;              
                      
    }

    freeaddrinfo(res);

    return 0;
}