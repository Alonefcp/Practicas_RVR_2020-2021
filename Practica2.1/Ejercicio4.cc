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
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc!=0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }  

    
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd==-1)
    {
        std::cout << "[socket]: " << strerror(errno) << std::endl;
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    listen(sd,16);

    struct sockaddr cliente;
    socklen_t cliente_len = sizeof(struct sockaddr);

    int cliente_sd = accept(sd,&cliente,&cliente_len);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&cliente, cliente_len, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
    std::cout << "Host: " << host << ", Port: " << serv << std::endl; 

    while(true)
    {
        char buffer[TAMBUFFER];

        int bytes = recv(cliente_sd, (void*) buffer, TAMBUFFER-1, 0);
        buffer[bytes]= '\0';
        if(bytes <=0)
        {
            std::cout << "FIN DE LA CONEXION" << std::endl;
            break;
        }

        //std::cout << "Data: " << buffer<<std::endl;

        int s = send(cliente_sd, buffer, bytes, 0);   
        if(s==-1)
        {
            std::cout << "[send]: " << strerror(errno) << std::endl;
            close(sd);
            close(cliente_sd);
            return -1;
        }            
    }

    close(sd);
    close(cliente_sd);

    return 0;
}