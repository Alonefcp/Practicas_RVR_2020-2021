#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <unistd.h>



#include <iostream>

#include <string.h>



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

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while(true)

    {
        char buffer[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t cliente_len = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*) buffer, 79, 0, &cliente, &cliente_len);
        buffer[bytes]= '\0';

        if(bytes==-1)
        {
            std::cerr<<"Error recieving messages"<<std::endl;

            return -1;
        }

        getnameinfo(&cliente, cliente_len, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        std::cout << "Host: " << host << ", Port: " << serv << std::endl; 

        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);

        if(std::tolower(buffer[0])=='t')

        {

            int tam = strftime(buffer,79,"%T",timeinfo);

            sendto(sd, buffer, tam, 0, &cliente, cliente_len);

        } 
        else if(std::tolower(buffer[0])=='d')
        {

            int tam = strftime(buffer,79,"%D",timeinfo);

            sendto(sd, buffer, tam, 0, &cliente, cliente_len);
        }
        else if(buffer[0]=='q')
        {

            std::cout<< "Saliendo... " << std::endl;

            break;
        } 
        else
        {

            std::cout<< "Comando no soportado: " << buffer[0] << std::endl;
            std::cout<< "Comandos soportados: t, d, q" << std::endl;

        }          

    }

    close(sd);
    return 0;

}