#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>

#include <iostream>
#include <string.h>

#define TAMBUFFER 80
#define MAX_THREAD 5


class MesnsajeThread
{
public:
    MesnsajeThread(int _sd)
    {
        sd =_sd;
    } 

    void haz_mensaje()
    {      
        char buffer[TAMBUFFER];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t cliente_len = sizeof(struct sockaddr);
        
        int bytes = recvfrom(sd, (void*) buffer, TAMBUFFER-1, 0, &cliente, &cliente_len);
        buffer[bytes]= '\0';

        if(bytes==-1)
        {
            std::cerr<<"[recvfrom]:Error recieving messages"<<std::endl;
            close(sd);
            return;
        }


        for(int i =0;i<5;i++)
        {
            std::cout<<i<<std::endl;
            sleep(1);
        }

        getnameinfo(&cliente, cliente_len, host, NI_MAXHOST,serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        std::cout << "[Id: " <<  std::this_thread::get_id() << "] Host: " << host << ", Port: " << serv << std::endl; 

        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);

        if(strcmp(buffer,"t")==0 || strcmp(buffer,"t\n")==0)
        {
            int tam = strftime(buffer,TAMBUFFER-1,"%r",timeinfo) + 1;
            sendto(sd, buffer, tam, 0, &cliente, cliente_len);
        } 
        else if(strcmp(buffer,"d")==0 || strcmp(buffer,"d\n")==0)
        {
            int tam = strftime(buffer,TAMBUFFER-1,"%D",timeinfo) + 1;
            sendto(sd, buffer, tam, 0, &cliente, cliente_len);
        }
        else if(strcmp(buffer,"q")==0 || strcmp(buffer,"q\n")==0)
        {
            std::cout<< "Saliendo... " << std::endl;
            return;
        } 
        else
        {
            std::cout<< "Comando no soportado: " << buffer << std::endl;
            std::cout<< "Comandos soportados: t, d, q" << std::endl;           
        }
    }

private:
  int sd;
};


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
        close(sd);
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    std::thread pool[MAX_THREAD];
    
    for(int i=0;i<MAX_THREAD;i++)
    {      
        
        MesnsajeThread* mt = new MesnsajeThread(sd);
    
        pool[i] = std::thread([&mt](){

            while (true)
            {
                mt->haz_mensaje();
            }
            
            delete mt;

        });   
        
    }

    for(int i=0;i<MAX_THREAD;i++)
    {
        pool[i].join();
    }

    close(sd);

    return 0;
}