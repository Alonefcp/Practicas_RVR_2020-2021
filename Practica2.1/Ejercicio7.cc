#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <string.h>
#include <thread>

#define TAMBUFFER 80


class ConexionThread
{
public:
    ConexionThread(int _sd)
    {
        cliente =_sd;
    } 

    void haz_conexion()
    {

        while(true)
        {
            char buffer[TAMBUFFER];

            //Recibo datos del cliente
            int bytes = recv(cliente, (void*) buffer, TAMBUFFER-1, 0);
            buffer[bytes]= '\0';
            if(bytes <=0)
            {
                std::cout << "FIN DE LA CONEXION" << std::endl;
                break;
            }

            std::cout << "[Id: " <<  std::this_thread::get_id() << "] Data: " << buffer << std::endl;

            //Envio datos al cliente
            int s = send(cliente, buffer, bytes, 0);   
            if(s==-1)
            {
                std::cout << "[send]: " << strerror(errno) << std::endl;
                return;
            }            
        }

        close(cliente);

    }

private:
  int cliente;
};



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
        std::cout << "[socket]: " << strerror(errno) << std::endl;
        return -1;
    }

    //Bind
    bind(sd, res->ai_addr, res->ai_addrlen);


    //Espera aceptar conexiones
    listen(sd,16);

    //Bucle ppal del servidor
    while(true)
    {
        struct sockaddr cliente;
        socklen_t cliente_len = sizeof(struct sockaddr);

        //Creacion del socket del cliente
        int cliente_sd = accept(sd,&cliente,&cliente_len);

        if(cliente_sd==-1)
        {
            std::cout << "[accept]: " << strerror(errno) << std::endl;
            return -1;
        }

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        //Informacion del host y direccion del cliente
        getnameinfo(&cliente, cliente_len, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        std::cout << "Host: " << host << ", Port: " << serv << std::endl; 

        //Creo el objeto que trata las conexiones
        ConexionThread* ct = new ConexionThread(cliente_sd);

        //Creo del thread
        std::thread(&ConexionThread::haz_conexion, *ct).detach();

        delete ct;     
    }

    close(sd);
    freeaddrinfo(res);

    return 0;
}