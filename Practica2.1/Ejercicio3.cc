#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <string.h>

#define TAMBUFFER 80

int main(int argc ,char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family   = AF_INET; 
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rc!=0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }    

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd==-1)
    {
        std::cerr<<"[socket] creacion de socket"<<std::endl;
        return -1;
    }

  
    int s = sendto(sd, argv[3], strlen(argv[3])+1, 0, res->ai_addr, res->ai_addrlen);
    if(s==-1)
    {
        std::cout<<"[sendto]: Error enviando mensajes\n";
        return -1;
    }
    
    
    char buffer[TAMBUFFER];
    int bytes = recvfrom(sd, (void*) buffer, TAMBUFFER, 0, res->ai_addr, &res->ai_addrlen);
    buffer[bytes]= '\0';

    if(bytes==-1)
    {
        std::cerr<<"[recvfrom]: Error recibiendo mensajes"<<std::endl;
        return -1;
    }
     
    std::cout << buffer << std::endl;

    freeaddrinfo(res);
    close(sd);

    return 0;
}