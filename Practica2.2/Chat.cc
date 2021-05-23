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

    memcpy(_data,&nick,8 * sizeof(char));
    _data += 8 * sizeof(char);

    memcpy(_data,&message,80 * sizeof(char)); 
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    memcpy(&type,_data, sizeof(uint8_t));
    _data += sizeof(uint8_t);

    memcpy(&nick,_data,8 * sizeof(char));
    _data += 8 * sizeof(char);

    memcpy(&message,_data,80 * sizeof(char)); 

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
        Socket* sdClient = nullptr;
        int bytes = socket.recv(message,sdClient);
        if(bytes==-1)
        {
            std::cerr<<"[recv in ChatServer]: " << strerror(errno) << std::endl;
            return;
        }

        switch (message.type)
        {
            case ChatMessage::LOGIN:{
      
                clients.push_back(std::unique_ptr<Socket>(std::move(sdClient)));
                break;
            }
            case ChatMessage::LOGOUT:{

                auto it = clients.begin();
                while(it != clients.end())
                {               
                    if(*it->get() == *sdClient)
                    {
                        clients.erase(it);
                        break;
                    }         
                    ++it; 
                }     
                break;
            }
            case ChatMessage::MESSAGE:{
      
                for(auto it = clients.begin(); it!=clients.end();++it)
                {
                    if(!(*it->get() == *sdClient))
                    {
                        socket.send(message,*(it->get()));                  
                    }   
                }
                break;
            }
            default:
                break;
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
        std::string msg;
        std::getline(std::cin,msg);

        // Enviar al servidor usando socket
        ChatMessage em(nick,msg);
        em.type=ChatMessage::MESSAGE;
        em.nick=nick;
        socket.send(em,socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage em;
        socket.recv(em);

        std::cout << em.nick << ": " << em.message << std::endl;
    }
}

