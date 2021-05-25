#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
        int16_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);
    };

    virtual ~Jugador(){};

    void to_bin()
    {       
        char* tmp = _data;

        memcpy(tmp,name, MAX_NAME * sizeof(char));

        tmp += MAX_NAME * sizeof(char);

        memcpy(tmp,&x,sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(tmp,&y,sizeof(int16_t));

    }

    int from_bin(char * data)
    {
        char* tmp = data;

        memcpy(name,tmp, MAX_NAME * sizeof(char));

        tmp += MAX_NAME * sizeof(char);

        memcpy(&x,tmp,sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(&y,tmp,sizeof(int16_t));
        
        return 0;
    }


    void mostrarContenido()
    {
        std::cout<<"Nombre: " << name << " , PosX: " << x << " , PosY: " << y << std::endl;
    }


private:

    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);
    write(fd,one_w.data(),one_w.size());
    close(fd);

    // 3. Leer el fichero
    char data[one_r.size()];
    fd = open("./data_jugador", O_RDONLY);
    read(fd,data,one_r.size());
    close(fd);

    // 4. "Deserializar" en one_r
    one_r.from_bin(data);

    // 5. Mostrar el contenido de one_r
    one_r.mostrarContenido();


    return 0;
}