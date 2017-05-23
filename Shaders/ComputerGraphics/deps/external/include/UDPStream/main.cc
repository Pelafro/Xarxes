#include "UDPStream.hh" //sustituto directo de winsock2, NO hace falta incluir windows.h ni winsock2.h (ni sus homonimos POSIX)
#include <iostream>
#include <cstdlib>
#include <string>
#include <conio.h>
#include <map>

struct Position {
	float x;
	float y;
};

struct Client {
	Position pos;
	std::string nick;
	sockaddr player;
};

enum Command {
	LOGIN,
	MOVE
};

/*
	NOTA: IMPORTANTE
	POD: clase/struct que sólo contiene tipos básicos de c, sin punteros ni visibilidad ni métodos virtuales
	Se pueden serializar std::vector<POD>, std::string, char* y POD
	La ip/puerto no hace dns por lo que tiene que estar en formato numérico "127.0.0.1:5000", "localhost:5000" NO funcionará
*/

void server_thread(int port) {
	std::map<uint64_t, Client> listaClientes; //btree associating clients with its unique id
	UDPStream server(port); //creamos una socket udp, la bindeamos al puerto port y hacemos asíncrona
	while (true) {
		try {
			int cabecera; //creamos una variable cabecera donde guardaremos el primer int que llega en el paquete
			sockaddr sender; //creamos una variable sender donde guardaremos la socketaddress de la persona que manda el paquete
			server >> UDPStream::packet >> sender >> cabecera; //intentamos recibir un paquete que contiene una cabecera (guardamos también quien lo ha enviado)
			switch (cabecera) {
				case LOGIN:{
					std::string nick; //creamos una variable std::string donde guardaremos el siguiente campo del paquete (nick)
					server >> nick; //recibimos la variable nick del paquete
					listaClientes[sender.hash] = { { 0,0 }, nick, sender }; //añadimos un nuevo cliente a la lista, sender.hash es un id único de socket
				}break;case MOVE:{
					char pressed_key; //creamos una variable char donde guardaremos el siguiente campo del paquete (tecla que hemos pulsado)
					server >> pressed_key; //recibimos la variable pressed_key del paquete (es la segunda variable que hemos mandado)
					std::cout << listaClientes[sender.hash].nick << " is moving "<< std::endl ; //printeamos el nick de la persona que se esta moviendo
					switch (pressed_key) { //dependiendo de la tecla que apretemos, cambiamos la posición
						case 'w':
							listaClientes[sender.hash].pos.y -= 0.1f;
						break; case 's':
							listaClientes[sender.hash].pos.y += 0.1f;
						break; case 'a':
							listaClientes[sender.hash].pos.x -= 0.1f;
						break; case 'd':
							listaClientes[sender.hash].pos.x += 0.1f;
					}
				}
			}
			for (auto variable : listaClientes) { //para cada cliente
				server << UDPStream::packet << variable.second.nick << std::vector<int>({0,1,2,3}) << variable.second.pos; //creamos un paquete con el nick, un vector (std::vector) y la posición del jugador (struct -pod)
				for(auto eachPlayer : listaClientes) //con el paquete ya creado, enviamos el paquete a todos los clientes
					server << eachPlayer.second.player ;
			}
		} catch (UDPStream::wrong) { //si la cantidad de datos del paquete no se corresponde a la cantidad de datos que estamos intentando leer
			std::cout << "Client Received wrongly serialized data" << std::endl;
		} catch (UDPStream::empty) {} //si el paquete esta vacio o no hemos recibido nada
	}
}

void client_thread(const char* ipport, const char* nick) {
	UDPStream client;
	sockaddr server_address(ipport);
	client << UDPStream::packet << LOGIN << nick << server_address; //mandamos un int y un char* a la ip/puerto en cuestión
	while (true) {
		if (_kbhit())
			client << UDPStream::packet << MOVE << _getch() << server_address; //podemos enviar a una adress ya guardada
		try {
			Position server_position; //reservamos la variable en la que guardaremos la posición
			std::vector<int> test; //reservamos la variable que usaremos como vector de Plain-Old-Data
			std::string nick; //reservamos la variable en la que guardaremos la string
			client >> UDPStream::packet >> nick >> test >> server_position; //intentamos leer nick, vector y posición;
			printf("%d %d %d\n",test[0],test[1],test[2]); //printeamos vector
			std::cout << nick << "(" << server_position.x << "," << server_position.y << ")" << std::endl; //printeamos nick y posición
		} catch (UDPStream::wrong) { //si la cantidad de datos del paquete no se corresponde a la cantidad de datos que estamos intentando leer
			std::cout << "Server Received wrongly serialized data" << std::endl;
		} catch (UDPStream::empty) {} //si el paquete esta vacio o no hemos recibido nada
	}
}

#include <thread>
int main(int argc, char const *argv[]) {
	std::thread(server_thread, atoi("5000")).detach();
	std::thread(client_thread, "127.0.0.1:5000", "EntiAlum").detach();
	while (true);
}
