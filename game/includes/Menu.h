#include <netinet/in.h> // socket struct
class Game {
private:
    // Configuración general
    int    FPS = 50;
    int gameMode = 1; // 1: easy(200). 2: Medium (50) , 3: Hard (20), 4: Progressive

    // info del servidor
    char  serverAddress[32] =  {'l','o','c','a','l','h','o','s','t','\0'};
    int serverPort = 7777;
    bool serverStatus = false;
    unsigned int players_count = 0;
    int sockfd,
        p1Sock,
        p2Sock;
    struct sockaddr_in servaddr,
                        cliaddr,
                        p1Client,
                        p2Client;
    // info del tablero
    const int width = 84;
    const int height = 20;
    const int scoreBoard = 4;
//  Configuración del Player 1
    char player1Name = ' ';
    int playerOneScore = 0;
    int playerOnePos = (height + scoreBoard)/2 - 2;
    char p1UpKey = 'w';
    char p1DownKey = 's';
//    Configuraciones del jugador dos
    char player2Name = ' ';
    int playerTwoScore = 0;
    int playerTwoPos = (height + scoreBoard)/2 - 2;
    char p2UpKey = 'i';
    char p2DownKey = 'k';

    // datos de la bola
    int ballXPosition = width/2;
    int ballYPosition = (height + scoreBoard)/2;
    int ballXDirection = 1; // 1: left && -1: right
    int ballYDirection = 1; // 1: down && -1: up

    char key[31] = "@PoNg_CdD_iN_Cpp_HaVe_FuN!@";

public:
    Game();
    ~Game();

     int getFPS();
     void setFPS(int n_fps);


    // PLAYERS
    int getPlayers();
    void setPlayers();

    int getPlayerOneScore();
    void setPlayerOneScore(int);

    int getPlayerTwoScore();
    void setPlayerTwoScore(int);

    int getPlayerOneYPos();
    void setPlayerOnePos(int);

    int getPlayerTwoYPos();
    void setPlayerTwoPos(int);

    int getPlayer1XPos(); //
    int getPlayer2XPos(); //

    // Bola
    int getBallXPos();
    void setBallXPos(int);

    int getBallYPos();
    void setBallYPos(int );

    int getBallXDir();
    void setBallXDir(int );

    int getBallYDir();
    void setBallYDir(int);


    // HERRAMIENTAS
    char controlInput(); // maneja la entrada del teclado sin búfer de terminal
    int kbhit(void);
    char* encryption( char* msg, char* key);
    char* decrypt( char* msg, char* key);
    char getRandomChar();
    void setDifficulty(int n_difficulty);

    // Lado del servidor
    char * getAddress();
    void setAddress(char * n_address);
    bool getServerStatus();
    void setServerStatus(bool n_status);

    char * handlingMessage(char * msg, char * resp);

    // Hilos
    static void *rcvMessageHelper(void *);
    char * rcvMessage(char * msg, char * resp);
    static void *rcvMessageHelperP2(void *);
    char * rcvMessageP2(char * msg, char * resp);
    char * rcvMessageFrom(int id_socket, struct sockaddr_in client, char * msg);
    void sendMessageTo(int id_socket, struct sockaddr_in client, char * msg);

    int getPort();
    void setPort(int n_port);

    void setServerTCP(); // establece con opciones predeterminadas
    void setServerUDP(); // establece con opciones predeterminadas

};

//menu
void initGame();

void setSpeed();
void setAddressHelper();
void setPortHelper();
void setGameMode();

void showControls();

void displayName();
void displayMenu();
void displayOptions();
void serverOptions();
void playerOptions();
void displaySpeed();
void displayAbout();
//player
#include <netinet/in.h> // socket struct
 
class Player : public virtual Game { //hibrido

private:
    char name[32] = "192.168.0.1";
    int playerScore = 0;
    int playerSide = getPlayers(); // 1 o 2

    char key[31] = "@Soket_Pong_Para_Cdd_UTEM@";// 
    int socket_fd;
    struct sockaddr_in server;

public:
    Player();
    ~Player();
    void joinGame(); // Los jugadores solicitan ingresar al juego al lado predeterminado.
    void connectionUDP(); // Establece la conexión con el servidor y permanece conectado mientras el cliente se está ejecutando
    void connectionTCP();
    void closeConnection(); // cierra el zócalo
    void recvHandler(char *); // Maneja los datos recibidos del servidor
    char* createMessage(char * msg); // Crea un mensaje para enviar cada vez que se presiona una tecla para actualizar la condición de los jugadores.
    void sendMessage();
    int getPaddleXPos();
    int getPlayerSide();
    char * getPaddle();
    void sendPosY();
    static void *listenServerHelper(void *);
    void listenServer(char* buff);
    static void *sendToServerHelper(void *);
    void sendToServer();
    
};

class Core : public Board, public Player {

private:
public:
    Core();
    ~Core();
    void playerHandler();
    void serverHandler();
    void *updatesScore(void *, int , int);

};

//Tablero
class Board : virtual public Game{

private:


    const int width = 84;
    const int height = 20;
    const int scoreBoard = 4;

    const char* playerOnePaddle = "123456";
    const char* playerTwoPaddle = "123456";

    char pongBall = 'O';

public:
    Board();
    ~Board();

    // Juego en sí
    void gameMenu(); // muestra el menú para mostrar reglas y mostrar información sobre el juego y los creadores
    void showBoard(int, int); // muestra el tablero: jugadores, pelota, puntos y más.

    // Bola
    void setBallPos(int posX, int posY);
    void resetBall();
    void ballHandler(int, int); // bota la pelota y la desplaza
};

