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

    //Creacion del socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    
    if(sd==-1)
    {
        std::cerr << "[socket]: " << strerror(errno) << std::endl;
        return -1;
    }

    //Conexion al servidor
    int c = connect(sd,res->ai_addr,res->ai_addrlen);
    
    if(c==-1)
    {
        std::cout << "[connect]: " << strerror(errno) << std::endl;
        return -1;
    }
   
    freeaddrinfo(res);

    //Bucle ppal del cliente
    while (true)
    {
        char buffer[TAMBUFFER];

        //Guardo los datos que escriba el cliente      
        std::cin.getline(buffer,TAMBUFFER);

        //El cliente se desconecta
        if(strcmp(buffer,"Q") == 0 || strcmp(buffer,"Q\n") == 0) break; 

        //Envio datos al servidor
        int s = send(sd, &buffer, strlen(buffer)+1, 0);
        if(s==-1)
        {
            std::cout << strerror(errno) << std::endl;
            return -1;
        }

        //Recibo datos del servidor
        int bytes = recv(sd, &buffer, TAMBUFFER, 0);
        buffer[bytes]= '\0';

        if(bytes==-1)
        {
            std::cout << strerror(errno) << std::endl;
            return -1;
        }
     
        std::cout << buffer << std::endl;
    
    }
 
    close(sd);

    return 0;
}