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
	std::vector<ServerPlayer> waiting;							// Jugadors esperan per partida
	std::vector<ServerPlayer> playing;							// Jugadors jugant
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
	Timer timerMatch;
	State state = connect;
	bool playersConected = false;
	const int matchScore = 10;		// La diferencia maxima per juntar 2 jugadors
	int matchNum = 0;				// Number of matches

	timerReady.Start(0);
	timerPing.Start(0);
	timerMatch.Start(0);
	//-- GAME --//

	bool gameOn = true;
	thread.launch();

	std::cout << "Server";

	while (gameOn)
	{
		sf::Keyboard key;
		/*if (key.isKeyPressed(sf::Keyboard::BackSpace)) { // si el server vol tancar la comunicacio
			state = win;
		}*/
		mutex.lock();
		switch (state) {

			//-- CONECT --//
		case connect: { // que es conectin els dos jugadors
			//if (!playersConected) {
				//mutex.lock();
			if (!com.empty()) {
				//std::cout << com.size();
				switch (com.front().type) {
				case HELLO: {	// Un client es vol conectar

					for (int i = 0; i < player.size(); i++)
					{
						if (com.front().accum.id == player[i].id) {	// el jugador que diu Hello
							OutputMemoryBitStream output;
							output.Write(HELLO, TYPE_SIZE);
							output.Write(player[i].id, ACCUM_DELTA_SIZE);
							sender.SendMessages(player[i].ip, player[i].port, output.GetBufferPtr(), output.GetByteLength());

							std::cout << std::endl << "New Player " << player[i].id;

							break;
						}
					}

					com.pop();
				}
							break;
				case SEARCH:
				{
					int id = com.front().id;

					for (int i = 0; i < player.size(); i++)
					{
						if (player[i].id == id)
						{
							waiting.push_back(player[i]);
							break;
						}
					}

					timerMatch.Start(5000); // reiniciem contador matchmaking

					com.pop();
				}
				break;
				case CONNECTION:
				{
					int matchid;
					for (int i = 0; i < playing.size(); i++) // Busca el matchid i desconectar jugador
					{
						if (playing[i].id == com.front().id)
						{
							matchid = playing[i].matchId;
							player[i].ready = true;
							for (int j = i; j < playing.size(); j++)
							{
								if (j != i && playing[j].matchId == matchid /*&& player[j].ready*/)
								{
									OutputMemoryBitStream output;
									output.Write(PLAY, TYPE_SIZE);
									std::cout << std::endl << "Comencen a jugar " << playing[i].id << " i " << playing[j].id;
									sender.SendMessages(playing[i].ip, playing[i].port, output.GetBufferPtr(), output.GetByteLength());
									sender.SendMessages(playing[j].ip, playing[j].port, output.GetBufferPtr(), output.GetByteLength());
									break;
								}
							}
							break;
						}
					}

					/*for (int i = 0; i < playing.size(); i++)
					{
						if (playing[i].matchId == matchid)
						{
							playing[i].ready = true;
							for (int j = 0; j < playing.size(); j++)
							{
								if (i != j && playing[i].ready == playing[j].ready)
								{
									OutputMemoryBitStream output;
									output.Write(PLAY, TYPE_SIZE);
									std::cout << std::endl << "Comencen a jugar " << playing[i].id << " i " << playing[j].id;
									sender.SendMessages(playing[i].ip, playing[i].port, output.GetBufferPtr(), output.GetByteLength());
									sender.SendMessages(playing[j].ip, playing[j].port, output.GetBufferPtr(), output.GetByteLength());
								}
							}
							break;
						}
					}*/
					com.pop();
				}
				break;
				case MOVEMENT:
				{
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

					int matchid, id1, id2;
					
					for (int i = 0; i < playing.size(); i++) // Busca el matchid i desconectar jugador
					{
						if (playing[i].id == com.front().id)
						{
							id1 = playing[i].id;
							matchid = playing[i].matchId;

							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							sender.SendMessages(playing[i].ip, playing[i].port, output.GetBufferPtr(), output.GetByteLength());
							playing.erase(playing.begin() + i);
							break;
						}
					}

					for (int i = 0; i < playing.size(); i++) // Busca el contrincant i desconectarlo
					{
						if (playing[i].id != com.front().id && playing[i].matchId == matchid)
						{
							id2 = playing[i].id;
							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							sender.SendMessages(playing[i].ip, playing[i].port, output.GetBufferPtr(), output.GetByteLength());
							playing.erase(playing.begin() + i);
						}
					}

					for (int i = 0; i < player.size(); i++)
					{
						if (player[i].id == id1 || player[i].id == id2)
						{
							player[i].ready = false;
						}
					}

					com.pop();

				}
							 break;
				case DISCONNECTION: {

					int matchid;
					int id1, id2;
					for (int i = 0; i < playing.size(); i++) // Busca el matchid
					{
						if (playing[i].id == com.front().id)
						{
							matchid = playing[i].matchId;
							playing.erase(playing.begin() + i);
							id1 = playing[i].id;
							break;
						}
					}

					for (int i = 0; i < playing.size(); i++) // Busca el contrincant
					{
						if (playing[i].id != com.front().id && playing[i].matchId == matchid)
						{
							id2 = playing[i].id;
							OutputMemoryBitStream output;
							output.Write(DISCONNECTION, TYPE_SIZE);
							sender.SendMessages(playing[i].ip, playing[i].port, output.GetBufferPtr(), output.GetByteLength());
							playing.erase(playing.begin() + i);
							break;
						}
					}

					for (int i = 0; i < player.size(); i++) // elimina el que ha marxat
					{
						if (player[i].id == id1)
						{
							player.erase(player.begin() + i);
							break;
						}
					}

					for (int i = 0; i < player.size(); i++)
					{
						if (player[i].id == id2)
						{
							player[i].ready = false;
						}
					}

					com.pop();

				}
				break;
				}
			}
			if (timerMatch.Check() && waiting.size() >= TOTALPLAYERS) { // Si existeixen mes de 2 jugadors

				int idmatch = -1;
				int bestScore = 100;
				//for (int i = 0; i < waiting.size(); i++) // Matchmaking
				//{
					for (int j = 1; j < waiting.size(); j++)
					{
						int absoluteScore = waiting[0].score - waiting[j].score;
						if (absoluteScore < 0) absoluteScore = -absoluteScore;
						if (absoluteScore < bestScore) {
							bestScore = absoluteScore;
							idmatch = j;
						}
					}
					if (idmatch != -1)
					{
						//waiting[0].id = 0;
						waiting[0].x = 270;
						waiting[0].matchId = matchNum;
						playing.push_back(waiting[0]);

						//waiting[idmatch].id = 1;
						waiting[idmatch].x = 800;
						waiting[idmatch].matchId = matchNum;
						playing.push_back(waiting[idmatch]);

						OutputMemoryBitStream output1;
						output1.Write(CONNECTION, TYPE_SIZE);
						output1.Write(0, ID_SIZE); // 0 per ser la propia
						output1.Write(waiting[0].id, ACCUM_DELTA_SIZE);
						output1.Write(waiting[0].x, POSITION_SIZE);
						sender.SendMessages(waiting[0].ip, waiting[0].port, output1.GetBufferPtr(), output1.GetByteLength());

						OutputMemoryBitStream output2;
						output2.Write(CONNECTION, TYPE_SIZE);
						output2.Write(0, ID_SIZE); // 0 per ser la propia
						output2.Write(waiting[idmatch].id, ACCUM_DELTA_SIZE);
						output2.Write(waiting[idmatch].x, POSITION_SIZE);
						sender.SendMessages(waiting[idmatch].ip, waiting[idmatch].port, output2.GetBufferPtr(), output2.GetByteLength());

						OutputMemoryBitStream output3;
						output3.Write(CONNECTION, TYPE_SIZE);
						output3.Write(1, ID_SIZE);
						output3.Write(waiting[0].id, ACCUM_DELTA_SIZE);
						output3.Write(waiting[0].x, POSITION_SIZE);
						sender.SendMessages(waiting[idmatch].ip, waiting[idmatch].port, output3.GetBufferPtr(), output3.GetByteLength());

						OutputMemoryBitStream output4;
						output4.Write(CONNECTION, TYPE_SIZE);
						output4.Write(1, ID_SIZE);
						output4.Write(waiting[idmatch].id, ACCUM_DELTA_SIZE);
						output4.Write(waiting[idmatch].x, POSITION_SIZE);
						sender.SendMessages(waiting[0].ip, waiting[0].port, output4.GetBufferPtr(), output4.GetByteLength());

						waiting.erase(waiting.begin() + idmatch);
						waiting.erase(waiting.begin());

						std::cout << "Set match " << matchNum << std::endl;

						matchNum++;


						break;
					}
				//}
				timerMatch.Start(5000);
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