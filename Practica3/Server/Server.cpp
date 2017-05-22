#include <cstring>

#include "Game.h"

// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Esperar a que es conectin els dos jugadors
	play,		// Partida
	points,		// Un jugador mata a l'altre i se li suma un punt
	win			// el joc sacaba
};

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1");	// sf::IpAddress::getLocalAddress();
	sf::UdpSocket socket;										// El socket del servidor
	std::queue<InputMemoryBitStream> clientCommands;			// Misatges dels jugadors per anar executant
	std::queue<Command> com;
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ServerReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&ServerReceive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<ServerPlayer> player;							// Vector de jugadors
	std::vector<ServerPlayer> playing;							// Vector de jugadors
	ServerPlayer playertmp;

	sf::Socket::Status status = socket.bind(5000);				// Bind al port 5000
	if (status != sf::Socket::Done) {
		std::cout << "Error al intent de bind" << std::endl;
		return -1;
	}
	socket.setBlocking(false);									// Fiquem socketa non Blocking

	sender.command = &command;
	sender.socket = &socket;

	receiver.commands = &clientCommands;
	receiver.com = &com;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	receiver.players = &player;

	//-- SERVER --//

	MessageManager protocol;
	Timer timerReady;
	Timer timerPing;
	State state = connect;
	bool playersConected = false;
	const int matchScore = 10;		// La diferencia maxima per juntar 2 jugadors
	int matchNum = 0;				// Number of matches

	timerReady.Start(0);
	timerPing.Start(0);

	//-- GAME --//

	bool gameOn = true;
	thread.launch();

	std::cout << "Server";

	while (gameOn)
	{
		sf::Keyboard key;
		if (key.isKeyPressed(sf::Keyboard::BackSpace)) { // si el server vol tancar la comunicacio
			state = win;
		}
		mutex.lock();
		switch (state) {

	//-- CONECT --//
		case connect: { // que es conectin els dos jugadors
			//if (!playersConected) {
				//mutex.lock();
				if (!com.empty()) {
					switch (com.front().type) {
					case HELLO: {	// Un client es vol conectar

						for (int i = 0; i < player.size(); i++)
						{
							if (com.front().id == player[i].id) {	// el jugador que diu Hello

								OutputMemoryBitStream output;
								output.Write(HELLO, TYPE_SIZE);
								sender.SendMessages(player[i].ip, player[i].port, output.GetBufferPtr(), output.GetByteLength());
								com.pop();
							}
						}
						
					}
						break;
					case CONNECTION:
					{
						for (int i = 0; i < playing.size(); i++)
						{
							if (playing[i].id == com.front().id)
							{
								playing[i].ready = true;
								// TODO: comprobacio if de si el seu contrincant esta ready
								break;
							}
						}
						com.pop();
					}
					break;

					}
				}
				if (player.size() > TOTALPLAYERS) { // Si existeixen mes de 2 jugadors
					for (int i = 0; i < player.size(); i++) // Matchmaking
					{
						for (int j = i; j < player.size(); j++)
						{
							if (i != j)
							{
								int absoluteScore = player[i].score - player[j].score;
								if (absoluteScore < 0) absoluteScore = -absoluteScore;

								if (absoluteScore <= matchScore)
								{
									player[i].id = 0;
									player[i].x = 270;
									player[i].matchId = matchNum;
									playing.push_back(player[i]);

									player[j].id = 0;
									player[j].x = 800;
									player[j].matchId = matchNum;
									playing.push_back(player[j]);

									OutputMemoryBitStream output1;
									output1.Write(CONNECTION, TYPE_SIZE);
									output1.Write(0, ID_SIZE); // 0 per ser la propia
									output1.Write(player[i].id, ID_SIZE);
									output1.Write(player[i].x, POSITION_SIZE);
									sender.SendMessages(player[i].ip, player[i].port, output1.GetBufferPtr(), output1.GetByteLength());

									OutputMemoryBitStream output2;
									output2.Write(CONNECTION, TYPE_SIZE);
									output2.Write(0, ID_SIZE); // 0 per ser la propia
									output2.Write(player[j].id, ID_SIZE);
									output2.Write(player[j].x, POSITION_SIZE);
									sender.SendMessages(player[j].ip, player[j].port, output2.GetBufferPtr(), output2.GetByteLength());

									OutputMemoryBitStream output3;
									output3.Write(CONNECTION, TYPE_SIZE);
									output3.Write(1, ID_SIZE); // 1 per ser la propia
									output3.Write(player[i].id, ID_SIZE);
									output3.Write(player[i].x, POSITION_SIZE);
									sender.SendMessages(player[j].ip, player[j].port, output3.GetBufferPtr(), output3.GetByteLength());
									
									OutputMemoryBitStream output4;
									output4.Write(CONNECTION, TYPE_SIZE);
									output4.Write(1, ID_SIZE); // 1 per ser la propia
									output4.Write(player[j].id, ID_SIZE);
									output4.Write(player[j].x, POSITION_SIZE);
									sender.SendMessages(player[i].ip, player[i].port, output4.GetBufferPtr(), output4.GetByteLength());

									player.erase(player.begin() + j);
									player.erase(player.begin() + i);

									matchNum++;
									break;
								}
							}
						}
					}
				}
			//}
			//else
			//{
				/*if (timerReady.Check()) {
					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].x, POSITION_SIZE);
					sender.SendMessages(player[1].ip, player[1].port, output.GetBufferPtr(), output.GetByteLength());
					for (int i = 0; i < player[1].keyComs.size(); i++)
					{
						if (player[1].keyComs[i].type == CONNECTION) {
							break;
						}
						else if (i == player[1].keyComs.size() - 1) {
							Command comtmp;
							comtmp.type = CONNECTION;
							player[1].keyComs.push_back(comtmp);
						}
					}
					OutputMemoryBitStream output2;
					output2.Write(CONNECTION, TYPE_SIZE);
					output2.Write(player[1].id, ID_SIZE);
					output2.Write(player[1].x, POSITION_SIZE);
					sender.SendMessages(player[0].ip, player[0].port, output2.GetBufferPtr(), output2.GetByteLength());
					for (int i = 0; i < player[0].keyCommands.size(); i++)
					{
						if (player[0].keyComs[i].type == CONNECTION) {
							break;
						}
						else if (i == player[0].keyComs.size() - 1) {
							Command comtmp;
							comtmp.type = CONNECTION;
							player[0].keyComs.push_back(comtmp);
						}
					}
					timerReady.Start(5000);
				}*/
				//mutex.lock();
				/*if (!com.empty()) {
					switch (com.front().type) {
					case HELLO:
						playersConected = false;
						
						//clientCommands.pop();
						break;
					case CONNECTION:	// Un client es vol conectar

						player[com.front().id].ready = 1;
						for (int i = 0; i < player[com.front().id].keyComs.size(); i++) // Recorrer tots els keycommands
						{
							if (player[com.front().id].keyComs[i].type == CONNECTION) {								// si es un keycommand de ready							
								for (int j = 0; j < player[com.front().id].keyComs.size(); j++)
								{
									player[com.front().id].keyComs.erase(player[com.front().id].keyComs.begin() + j);	// borral
									//std::cout << "All messages deleted for player " << com.front().id << std::endl;
								}							
								break;
							}
						}
						com.pop();
						break;
					}
				}*/
				//mutex.unlock();
				/*if (player[0].ready == 1 && player[1].ready == 1)
				{
					state = play;
				}*/
			//}
		}
			break;

	//-- PLAY --//

		case play: {
			//mutex.lock();
			////-- PING --////

			if (timerPing.Check()) {
				// Check si un o els dos jugadors s'ha desconectat
				bool toConect = false;
				for (int i = 0; i < TOTALPLAYERS; i++)
				{
					for (int j = 0; j < player[i].keyComs.size(); j++)
					{
						if (player[i].keyComs[j].type == PING) {
							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							output.Write(i, ID_SIZE); // Misatge que s'ha desconectat el jugador i

							//std::cout << "player " << i << " seems to be disconected" << std::endl;

							for (int k = 0; k < player.size(); k++)
							{
								sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
							}

							player.erase(player.begin() + i);
							toConect = true;
						}
					}
				}
				if (toConect) { // si hi ha algun jugador desconectat
					state = connect;
				}
				else {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					bool foundMessage = false;									// Per saber si hi ha un misatge igual
					for (int k = 0; k < player.size(); k++)
					{
						sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
						for (int l = 0; l < player[k].keyComs.size(); l++)
						{
							if ( player[k].keyComs[l].type == PING) {
								foundMessage = true;
								break;
							}
						}
						if (!foundMessage)
						{
							Command com;
							com.type = PING;
							player[k].keyComs.push_back(com);
						}

						foundMessage = false;
					}
					
					timerPing.Start(3000);
				}

			}

			////-- CLIENT COMMANDS --////

			
			if (!com.empty()) {
				
				//std::cout << "And now Client Case is " << com.front().type << std::endl;

				switch (com.front().type) {

				case HELLO: {	// Un client es vol conectar
					state = connect;
					com.pop();
				}
					break;

				case CONNECTION: {
					com.pop();
				}
					break;

				case PING: {
					//std::cout << "Player " << com.front().id << " Pinged" << std::endl;
					for (int i = 0; i < player[com.front().id].keyComs.size(); i++)
					{
						if (player[com.front().id].keyComs[i].type == PING) {
							player[com.front().id].keyComs.erase(player[com.front().id].keyComs.begin());
							break;
						}
					}
					com.pop();

				}
					break;

				case DISCONNECTION: {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(com.front().id, ID_SIZE);
					//bool foundMessage = false;									// Per saber si hi ha un misatge igual
					for (int k = 0; k < player.size(); k++)
					{
						sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
					}
					player.erase(player.begin() + com.front().id);
					state = connect;
					com.pop();

				}
									break;
				case MOVEMENT: {
					Accum accumtmp = com.front().accum;

					int threshold = 6; // Maximum error able to forgive
					for (int i = -threshold; i < threshold; i++)
					{
						int distance;
						if (com.front().id == 0)
						{
							distance = accumtmp.absolute - player[1].x;
						}
						else {
							distance = accumtmp.absolute - player[0].x;
						}
						if (distance < 0) distance = -distance;
						if (accumtmp.absolute > LEFT_LIMIT && accumtmp.absolute < RIGHT_LIMIT && distance > DISTANCIA_BODY) {
							if ((player[com.front().id].x + accumtmp.delta + i) == accumtmp.absolute) { // si esta dins de la posicio que estem disposats a accpetar
								player[com.front().id].x = accumtmp.absolute;
								player[com.front().id].accum.push_back(accumtmp);

								OutputMemoryBitStream output;
								output.Write(MOVEMENT, TYPE_SIZE);
								output.Write(com.front().id, ID_SIZE);
								output.Write(player[com.front().id].accum.back().id, ACCUM_ID_SIZE);
								output.Write(player[com.front().id].accum.back().sign, ID_SIZE);
								if (player[com.front().id].accum.back().delta < 0)
									output.Write(-player[com.front().id].accum.back().delta, ACCUM_DELTA_SIZE);
								else
									output.Write(player[com.front().id].accum.back().delta, ACCUM_DELTA_SIZE);
								output.Write(player[com.front().id].accum.back().absolute, POSITION_SIZE);

								//std::cout << "Delta received: " << com.front().accum.delta << std::endl;

								for (int k = 0; k < player.size(); k++)
								{
									sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
								}
							}
						}
					}
					//accumtmp.absolute = player[com.front().id].x + accumtmp.delta;

					com.pop();
					
				}
							   break;
				case ATTACK: {

					if (player[com.front().id].attack == 0)
					{
						player[com.front().id].attack = com.front().position;

						OutputMemoryBitStream output;
						output.Write(ATTACK, TYPE_SIZE);
						output.Write(player[com.front().id].id, ID_SIZE);
						output.Write(player[com.front().id].attack, ATTACK_SIZE);

						if (com.front().id == 0)
						{
							sender.SendMessages(player[1].ip, player[1].port, output.GetBufferPtr(), output.GetByteLength());
						}
						else 
						{
							sender.SendMessages(player[0].ip, player[0].port, output.GetBufferPtr(), output.GetByteLength());
						}
							
						com.pop();
					}
					else
					{
						int distance = player[0].x - player[1].x;
						if (distance < 0) distance = -distance;
						if (distance < DISTANCE_ATTACK)
						{
							player[com.front().id].score++;

							player[0].x = player[0].originalX;
							player[1].x = player[1].originalX;							

							OutputMemoryBitStream output;
							output.Write(SCORE, TYPE_SIZE);
							output.Write(player[com.front().id].id, ID_SIZE);

							if (!com.empty())
							{
								while (!com.empty())
								{
									com.pop();
								}
							}

							for (int k = 0; k < player.size(); k++)
							{
								sender.SendMessages(player[k].ip, player[k].port, output.GetBufferPtr(), output.GetByteLength());
							}
						}
						else
						{
							com.pop();
						}
						player[com.front().id].attack = 0;
						//player[1].attack = 0;
					}
					
				}
					break;
				}
			}
		}
			break;
		}
		mutex.unlock();
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}