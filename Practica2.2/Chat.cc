#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    memcpy(_data,&type, sizeof(uint8_t));
    _data += sizeof(uint8_t);

    memcpy(_data,&nick,nick.length()+1 * sizeof(std::string));
    _data += nick.length()+1 * sizeof(std::string);

    memcpy(_data,&message,message.length()+1 * sizeof(std::string)); 
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(&type,_data, sizeof(uint8_t));
    _data += sizeof(uint8_t);

    memcpy(&nick,_data,nick.length()+1 * sizeof(std::string));
    _data += nick.length()+1 * sizeof(std::string) * sizeof(std::string);

    memcpy(&message,_data,message.length()+1 * sizeof(std::string)); 

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
       
        ChatMessage message;
        int bytes = socket.recv(message);
        if(bytes==-1)
        {
            std::cerr<<"[recvfrom in ChatServer]: " << strerror(errno) << std::endl;
            return;
        }

        if(message.type == 0)
        {
            std::unique_ptr<Socket> client;
            clients.push_back(std::move(client));
        }
        else if(message.type==1)
        {
            for(auto& c : clients)
            {
                Socket s = *(c.get());
                if(!(socket==s))
                {
                    socket.send(message,s);
                }          
            }           
        }
        else if(message.type==2)
        {
            
        }     
        
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar

    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        //std::cin.getline(...);
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

