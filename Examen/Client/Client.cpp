#include <cstring>
#include <iostream>
#include <time.h>

#include "Game.h"

#define SpriteVelocityBot 0.05
#define SpriteVelocityTop 0.05
#define SpriteVelocityBlock 0.07

#define POSICIO_INICIAL1 270
#define POSICIO_INICIAL2 800
#define POSICIO_Y 150

// Protocol: https://docs.google.com/spreadsheets/d/152EPpd8-f7fTDkZwQlh1OCY5kjCTxg6-iZ2piXvEgeg/edit?usp=sharing

enum State {
	connect,	// Per conectarse al servidor
	send,		// enviar paraula nova y que comenci partida
	play,		// mentres els jugadors estan escribint. comproba si sacaba el temps i si algú ha encertat la partida
	points,		// Envia les puntuacions als jugadors y actualitza els seus logs
	win			// el joc sacaba
};

enum Anims {
	Idle,
	Top,
	Mid,
	Bot,
	Leg
};

int main()
{
	//-- UDP --//

	sf::IpAddress ip = sf::IpAddress::IpAddress("127.0.0.1"); //sf::IpAddress::getLocalAddress();
	unsigned short serverPort = 5000;
	sf::UdpSocket socket;
	std::queue<Command> com;
	sf::Mutex mutex;											// Per evitar varis accesos a les cues
	std::string command;										// el misatge que envia als clients
	Send sender;												// Sender per enviar misatges
	ClientReceive receiver;										// Receiver per rebre constanment misatges
	sf::Thread thread(&ClientReceive::ReceiveCommands, &receiver);	// Thread per el receiver
	std::vector<Player> player;									// Vector de jugadors

	sender.socket = &socket;

	receiver.com = &com;
	receiver.socket = &socket;
	receiver.mutex = &mutex;
	receiver.players = &player;

	std::cout << "Port: ";										// Demanem port al client
	unsigned short port;
	std::cin >> port;											// Guardem el port del client
	sf::Socket::Status status = socket.bind(port);				// Bind al port del client
	if (status != sf::Socket::Done) {							// Si falla el bind, acaba el programa
		std::cout << "Error al intent de bind" << std::endl;
		return -1;
	}

	//-- CLIENT --//

	Timer timerConnect;			// timer per intentar conectarse a servidor
	Timer timerAccum;			// timer per el acumulats de moviment
	State state = connect;		// Comencem en connect per que es conecti al server
	Player playertmp;			// Amb el tmp es guardara a ell mateix i als altres en el vector player

	playertmp.y = 150;
	player.push_back(playertmp);
	player.push_back(playertmp);
	Accum accumtmp; accumtmp.id = 0;
	player[0].accum.push_back(accumtmp);

	//-- GAME --//	

	////-- SPRITES --////

	sf::Texture background;
	if (!background.loadFromFile("../Resources/Fons.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite sprite;
	sprite.setTexture(background);

	sf::Texture particleT;
	if (!particleT.loadFromFile("../Resources/Fucsia.png")) {
		std::cout << "Can't load the image file" << std::endl;
		return -1;
	}
	sf::Sprite particle;
	particle.setTexture(particleT);
	
	//TEXT
	sf::Font font;
	if (!font.loadFromFile("../Resources/Samurai.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}
	sf::Text text1(/*std::to_string(puntsJugador1)*/"0", font, 50); //Aqui va la variable de puntuacio de cada jugador
	//text1.setColor(sf::Color::White);
	text1.setPosition(150, 750);
	sf::Text text2(/*std::to_string(puntsJugador2)*/"0", font, 50);
	//text2.setColor(sf::Color::White);
	text2.setPosition(1450, 750);
	sf::Text PointText("", font, 100);
	PointText.setPosition(300, 250);
	sf::Text Instructions("Prem 'ENTER' per iniciar la propera ronda", font, 30);
	Instructions.setPosition(300, 750);

	sf::Vector2i screenDimensions(1600, 900);											// Dimensions pantalles
	sf::RenderWindow window;															// Creem la finestra del joc
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Aoi Samurai");	// Obrim la finestra del joc
	window.setFramerateLimit(60); //FrameRate

	thread.launch();																	// Comencem thread receive

	bool attacking = false;

	int numParticles = 0;
	int velParticles = 1;
	std::vector<Particle> particles;

	while (window.isOpen())
	{
		sf::Event event; //Si no la finestra no detecta el ratolí i no es pot moure
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed )
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}				
		}

		switch (state) {
		case connect: {

			if (timerConnect.Check()) {
				if (player[0].x == 0) {
					OutputMemoryBitStream output;
					output.Write(HELLO, TYPE_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());

					//timerConnect.Stop();
				}
				timerConnect.Start(5000);
			}

			if (!com.empty()) {
				switch (com.front().type) {

				case HELLO: {

					//player[0].id = com.front().id;
					//player[0].x = com.front().position;
					//player[0].originalX = player[0].x;
					//player[0].accum.front().origin = player[0].x;

					std::cout << std::endl << "Escribe el numero de particulas que quieres (entre 1 i 15): ";
					std::cin >> numParticles;

					OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(numParticles, ACCUM_ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					/*if (player[0].id == 1)
					{}
					else
					{}*/

					com.pop();
				}
							break;

				case CONNECTION: {


					velParticles = com.front().position;
					state = play;
					/*player[1].id = com.front().id;
					player[1].x = com.front().position;
					player[1].originalX = player[1].x;*/

					com.pop();

				}
								 break;
				}
			}
			/*if (player[0].x != 0 && player[1].x != 0)
			{
				OutputMemoryBitStream output;
				output.Write(CONNECTION, TYPE_SIZE);
				output.Write(player[0].id, ID_SIZE);
				sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
				state = play;
				//std::cout << "play " << player[0].x << " " << player[1].x << std::endl;
			}*/
		}
					  break;

		case send: {}
				   break;

		case play: {

			//-- MOVEMENT --//
			sf::Keyboard key;
			if (event.type == sf::Event::KeyPressed) {

				switch (event.type)
				{
				case sf::Event::MouseButtonPressed:
				{
					switch (event.mouseButton.button) 
					{
					case sf::Mouse::Left:
					{
						int x = event.mouseButton.x;
						int y = event.mouseButton.y;

						OutputMemoryBitStream output;
						output.Write(ATTACK, TYPE_SIZE);
						output.Write(x, POSITION_SIZE);
						output.Write(y, POSITION_SIZE);



						sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					}
					break;
					}			
				}
				break;
				}

				/*if (key.isKeyPressed(sf::Keyboard::Right)) 
				//if (event.key.code == sf::Keyboard::Right)
				{
					int movement = 2;
					int distance = player[1].x - (player[0].x + movement);
					if (distance < 0) distance = -distance;
					if (distance > DISTANCIA_BODY)
					{
						if ((player[0].x + movement) < RIGHT_LIMIT)
						{
							player[0].x += movement;
							player[0].accum.back().delta += movement;
						}
					}

				}
				if (key.isKeyPressed(sf::Keyboard::Left)) 
				//else if (event.key.code == sf::Keyboard::Left)
				{
					int movement = -2;
					int distance = player[1].x - (player[0].x + movement);
					if (distance < 0) distance = -distance;
					if (distance > DISTANCIA_BODY)
					{
						if ((player[0].x + movement) > LEFT_LIMIT)
						{
							player[0].x += movement;
							player[0].accum.back().delta += movement;
						}
					}
				}
			//}
			//if (event.type == sf::Event::KeyReleased)
			//{
				if (key.isKeyPressed(sf::Keyboard::Z) && player[0].attack == 0 && !attacking)
					//if (event.key.code == sf::Keyboard::Z && !attacking && player[0].attack == 0)
					{
						attacking = true;
						std::cout << player[0].attack;
						player[0].attack = 1;


						OutputMemoryBitStream output;
						output.Write(ATTACK, TYPE_SIZE);
						output.Write(player[0].id, ID_SIZE);
						output.Write(player[0].attack, ATTACK_SIZE);

						sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());

					}
					else if (event.key.code == sf::Keyboard::Z && player[0].attack != 0)
					{
						player[0].attack = 0;
					}

				if (key.isKeyPressed(sf::Keyboard::X) && player[0].attack == 0 && !attacking)
				//else if (event.key.code == sf::Keyboard::X && !attacking && player[0].attack == 0)
				{
					attacking = true;
					player[0].attack = 2;


					OutputMemoryBitStream output;
					output.Write(ATTACK, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].attack, ATTACK_SIZE);

					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
				}
				else if (event.key.code == sf::Keyboard::X && player[0].attack != 0)
				{
					player[0].attack = 0;
				}

				if (key.isKeyPressed(sf::Keyboard::C) && player[0].attack == 0 && !attacking)
				//else if (event.key.code == sf::Keyboard::C && !attacking && player[0].attack == 0)
				{
					attacking = true;
					player[0].attack = 3;


					OutputMemoryBitStream output;
					output.Write(ATTACK, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].attack, ATTACK_SIZE);

					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
				}
				else if (event.key.code == sf::Keyboard::C && player[0].attack != 0)
				{
					player[0].attack = 0;
				}*/
			}
			/*if (player[0].attack != 0)
			{*/

				/*if (player[0].top->m_currentFrame == 12)
				{
					attacking = false;
					OutputMemoryBitStream output;
					output.Write(ATTACK, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].attack, ATTACK_SIZE);

					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					player[0].attack = 0;
				}*/

			//}

			//-- ACCUMULATED --//

			////-- PLAYER --////

			if (timerAccum.Check())
			{
				if (player[0].accum.back().delta != 0)
				{
					//int negative = 0; // 0 = positiu, 1 = negatiu
					if (player[0].accum.back().delta < 0) {
						player[0].accum.back().sign = 1;
						player[0].accum.back().delta = -player[0].accum.back().delta;
					}

					player[0].accum.back().absolute = player[0].x;			// Marco el absolut del moviment
					OutputMemoryBitStream output;
					output.Write(MOVEMENT, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					output.Write(player[0].accum.back().id, ACCUM_ID_SIZE);
					output.Write(player[0].accum.back().sign, ID_SIZE);
					output.Write(player[0].accum.back().delta, ACCUM_DELTA_SIZE);
					output.Write(player[0].accum.back().absolute, POSITION_SIZE);

					//std::cout << "Enviat " << player[0].accum.back().delta << std::endl;

					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());

					Accum accumtmp;										// Creo nou acumulat
					accumtmp.origin = player[0].x;						// Si tiene que reconcilarse a la posicion antigua
					if (player[0].accum.back().id == 15) accumtmp.id = 0;	// Si el ultim acumulat te id 15, el nou torna a 0
					else accumtmp.id = player[0].accum.back().id + 1;     // Sino, el id es un mes que l'anterior

					player[0].accum.push_back(accumtmp);
				}
				timerAccum.Start(ACCUMTIME);
			}

			////-- ENEMY --////

			if (!player[1].accum.empty())
			{
				int movement = 2;
				if (player[1].accum.front().absolute != player[1].x)
				{
					if (player[1].accum.front().delta < 0) movement = -movement;
					player[1].x += movement;
				}
				else
				{
					player[1].accum.erase(player[1].accum.begin());
				}
			}

			//-- COMMANDS --//

			for (int i = 0; i < particles.size(); i++)
			{
				if (particles[i].x != particles[i].nextX)
				{
					particles[i].x += velParticles*particles[i].right;
					if (particles[i].x + RADIUS > WIDTH)
					{
						particles[i].right = -1;
					}
					else if (particles[i].x - RADIUS < 0)
					{
						particles[i].right = 1;
					}

					particles[i].y += velParticles*particles[i].down;
					if (particles[i].x + RADIUS > HEIGHT)
					{
						particles[i].down = -1;
					}
					else if (particles[i].x - RADIUS < 0)
					{
						particles[i].down = 1;
					}
				}				
			}


			if (!com.empty()) {
				switch (com.front().type) {

				case HELLO: { 
				}
							break;

				case CONNECTION: {
					/*OutputMemoryBitStream output;
					output.Write(CONNECTION, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());*/
					com.pop();
					break;
				}
				case PING: {
					OutputMemoryBitStream output;
					output.Write(PING, TYPE_SIZE);
					output.Write(player[0].id, ID_SIZE);
					sender.SendMessages(ip, serverPort, output.GetBufferPtr(), output.GetByteLength());
					com.pop();
					break;
				}
				case MOVEMENT: {

					particles[com.front().id].nextX = com.front().x;
					particles[com.front().id].nextY = com.front().y;

					com.pop();		
				}
							   break;

				case ATTACK:
				{
					particles.erase(particles.begin() + com.front().id);

					if (particles.empty())
					{
						window.close();
					}

					com.pop();
				}
				break;

				case SCORE:
				{
					Particle particletmp;
					particletmp.id = com.front().id;
					particletmp.x = com.front().x;
					particletmp.y = com.front().y;

					particles.push_back(particletmp);

					com.pop();
				}
				break;

				default:
					break;

				}
			}
		}
				   break;
		}


		window.draw(sprite);
		if (state == play) {
			for (int i = 0; i < particles.size(); i++)
			{
				int x = particles[i].x;
				int y = particles[i].y;
				particle.setPosition(x, y);
				window.draw(particle);
			}
		}
		window.draw(text1); //Text de puntuacions
		window.draw(text2);

		window.display();		// Mostrem per la finestra
		window.clear();			// Netejem finestra
	}
	receiver.stopReceive = false;
	thread.terminate();
	return 0;
}