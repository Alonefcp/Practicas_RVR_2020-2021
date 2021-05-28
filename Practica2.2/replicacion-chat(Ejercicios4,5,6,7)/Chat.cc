#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;
    memcpy(tmp,&type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    memcpy(tmp,nick.c_str(),NICK_SIZE * sizeof(char));
    tmp += NICK_SIZE * sizeof(char);

    memcpy(tmp,message.c_str(),MSG_SIZE * sizeof(char)); 
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data;
    memcpy(&type,tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    char tmpNick[NICK_SIZE];
    memcpy(&tmpNick,tmp,NICK_SIZE * sizeof(char));
    nick = tmpNick;
    tmp += NICK_SIZE * sizeof(char);

    char tmpMessage[MSG_SIZE];
    memcpy(&tmpMessage,tmp,MSG_SIZE * sizeof(char)); 
    message=tmpMessage;

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
       

        ChatMessage* message = new ChatMessage();
        Socket* sdClient;
        int bytes = socket.recv(*message,sdClient);
        if(bytes==-1)
        {
            std::cerr<<"[recv in ChatServer]: " << strerror(errno) << std::endl;
            return;
        }

        switch (message->type)
        {
            case ChatMessage::LOGIN:{
      
                clients.push_back(std::unique_ptr<Socket>(std::move(sdClient)));
                std::cout << "LOGIN: " << message->nick << "\n";
                break;
            }
            case ChatMessage::LOGOUT:{
                
                auto it = clients.begin();
                while(it != clients.end())
                {               
                    if(*it->get() == *sdClient)
                    {
                        break;
                    }         
                    ++it; 
                }  

                if(it != clients.end()) clients.erase(it);

                std::cout << "LOGOUT: " << message->nick << "\n";

                break;
            }
            case ChatMessage::MESSAGE:{
      
                for(auto it = clients.begin(); it!=clients.end();++it)
                {
                    if(!(*it->get() == *sdClient))
                    {
                        socket.send(*message,*it->get());             
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
       
        // Al recibir como input la q el usuario sale del chat
        if(strcmp(msg.c_str(),"q") == 0)
        {
            logout();
            exit(0);
        }
        else
        {
            // Enviar al servidor usando socket
            ChatMessage em(nick,msg);
            em.type=ChatMessage::MESSAGE;
            socket.send(em,socket);
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {     
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::string msg;
        ChatMessage em(nick,msg);
        socket.recv(em);

        std::cout << em.nick << ": " << em.message << std::endl;
    }
}

