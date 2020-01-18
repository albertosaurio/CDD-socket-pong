#include "menu.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "pthread.h"
#include <pthread.h>
//kbhit
#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <random>
// Implementación del lado del servidor del modelo cliente-servidor UDP
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
//  socket
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>// socket struct
#include <arpa/inet.h>
#include <netdb.h>
// strings y librerias estándar
#include <cctype>
/*  VARIABLES GLOBALES */
#define PORT	 7777
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2
static pthread_mutex_t player_mutex; // evita la interrupción de hilos.
#define BUFFER_SIZE 512


using namespace std;
int north = -1,// dIRECCIONES
    south = 1,
    east = 1,
    west = -1,
    straight = 0;

Board::Board() {}// CONSTRUCTOR
Board::~Board() {}// DESTRUCTOR
void Board::setBallPos(int posX, int posY) {
    setBallXPos(posX);//Obtiene la posición X actual de la pelota en el tablero
    setBallYPos(posY);//Obtiene la posición Y actual de la pelota en el tablero
}
void Board::resetBall() {
    int xMid = width/2,
        yMid = (height + scoreBoard)/2 ;
    setBallPos( xMid, yMid);//Establece la nueva pocision de la pelota en X e Y
}
void Board::ballHandler(int p1_pos, int p2_pos) {

    int board = height + scoreBoard,
        x = getBallXPos(),
        y = getBallYPos(),
        yDir  = getBallYDir(),
        xDir  = getBallXDir();
        setPlayerOnePos(p1_pos);
        setPlayerTwoPos(p2_pos);
    //      Muro
    if( y == scoreBoard + 1){
        if( (y + yDir) == scoreBoard){
            setBallYDir(south);
        }
    }
    if( y == scoreBoard + height - 1){
        if( (y + yDir) == board){
            setBallYDir(north);
        }
    }
    //      paletas de los jugadores
    // Player1
    if( x + xDir == 4) {
        if( getPlayerOneYPos() == y  || getPlayerOneYPos() + 1 == y ){ 
            setBallXDir(east);
            setBallYDir(north);
        }
        else if ( getPlayerOneYPos() + 2 == y  || getPlayerOneYPos() + 3 == y){
            setBallXDir(east);
            setBallYDir(straight);
            }
        else if ( getPlayerOneYPos() + 4 == y  || getPlayerOneYPos() + 5 == y){
            setBallXDir(east);
            setBallYDir(south);
        }
    }
    // Player2
    if( x + xDir == width - 4) {
        if( getPlayerTwoYPos() == y  || getPlayerTwoYPos() + 1 == y ){
            setBallXDir(west);
            setBallYDir(north);
        }
        else if ( getPlayerTwoYPos() + 2 == y  || getPlayerTwoYPos() + 3 == y){
            setBallXDir(west);
            setBallYDir(straight);
        }
        else if ( getPlayerTwoYPos() + 4 == y  || getPlayerTwoYPos() + 5 == y){
            setBallXDir(west);
            setBallYDir(south);
        }
    }
    //      Puntuacion
    // P1
    if( x == (width - 1) ){
        setPlayerOneScore( getPlayerOneScore() + 1);
        resetBall();
        setBallXDir(west);
    }
    // P2
    if( x == 1 ){
        setPlayerTwoScore( getPlayerTwoScore() + 1 );
        resetBall();
        setBallXDir(east);
    }

    int nextX = getBallXPos() + getBallXDir(),//movimiento de la pelota en X
        nextY = getBallYPos() + getBallYDir();//movimiento de la pelota en Y
    setBallPos(nextX, nextY);
}
void Board::showBoard(int p1_pos, int p2_pos) {
    system("clear");
    system("color 2F");
    fflush(stdin);
    int k1 = 0,
        k2 = 0;
    for (int i = 0; i <= (height + scoreBoard); i++){
        if ( i == scoreBoard/2) {
            cout << "#\tpuntaje jugador 1: "<< getPlayerOneScore() <<"  \t\t" << "  #\tpuntaje jugador 2 : " << getPlayerTwoScore() <<"\t\t  ";
        }
        for ( int j = 0; j <= (width); j++){
            // bola
            if( i == getBallYPos() && j == getBallXPos()) {
                cout << pongBall;
                continue;
            }
            // dibujo del tablero
            if(i == 0 || i == (height + scoreBoard) || i == scoreBoard ){
                if(i == scoreBoard/2) {
                    continue;
                }
                else {cout << "#";}
            }
            else if ( j == 0 || j == (width) || (j == width/2 && i <= scoreBoard) ){
                cout << "#";
            }
            else if ( i == scoreBoard/2 ) {continue;}
            // linea media de la malla
            else if ( j == width/2 && i > scoreBoard){
                cout << "|";}
            // jugadores
           else if( j == 4){
                if(  i >= p1_pos && (p1_pos + 6) <= (height + scoreBoard) && i > scoreBoard){
                    if( k1 < 6) {
                        cout << playerOnePaddle[k1];
                        k1++;
                    }
                    else{cout << " ";}
                }
                else{cout << " ";}
           }
          else if( (width - j) == 4){
                if(  i >= p2_pos && (p2_pos + 6) <= (height + scoreBoard) && i > scoreBoard){
                    if( k2 < 6) {
                        cout << playerTwoPaddle[k2];
                        k2++;
                    }
                    else{cout << " ";}
                }
                else{cout << " ";}
           }
            else {cout << " ";}
        }
        cout << endl;
    }
    std::cout.flush();
    fflush(stdin);
}
//procesamiento
using namespace std;
Core::Core() {
}

Core::~Core() {
}

void updatesScore(void* p, int p1Score, int p2Score ){
    Game* a = (Game *)p;
    if(a->getPlayerOneScore() != p1Score )
        a->setPlayerOneScore(p1Score);
    if(a->getPlayerTwoScore() != p2Score )
        a->setPlayerTwoScore(p2Score);
    p=a;

}
void Core::serverHandler() {
    Game Server;
    Server.setAddress( getAddress() );
    Server.setPort( getPort() );
    Server.setServerUDP();
    pthread_t recv_thread,
            recvP2_thread;

    pthread_create(&recv_thread, NULL, Server.rcvMessageHelper,&Server);
    pthread_create(&recvP2_thread, NULL, Server.rcvMessageHelperP2,&Server);

    while(getPlayerOneScore() < 3 && getPlayerTwoScore() < 3) {

        ballHandler( Server.getPlayerOneYPos(), Server.getPlayerTwoYPos() );
        Server.setBallXPos( getBallXPos() );
        Server.setBallYPos( getBallYPos() );
        Server.setPlayerOneScore( getPlayerOneScore());
        Server.setPlayerTwoScore( getPlayerTwoScore());
        std::this_thread::sleep_for(std::chrono::milliseconds(getFPS()));
    }

    pthread_join(recv_thread, NULL);
    pthread_join(recvP2_thread, NULL);
}

void Core::playerHandler() {

    Player Client;
    Client.connectionUDP();
    Client.joinGame();

    auto maxTime = 20; // segundos
    //contador inicia por 20 segundos
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    while(!Client.getServerStatus()){
        system("clear");
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        cout << "\nElapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]";
        cout << "\nWaiting for more connections..." << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        Client.sendMessage(); // asks and updates

        if(maxTime == std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()){
            system("clear");
cout << "\n\n\n\t\t "
cout << "\n\t\tNadie se conecto al servidor"
cout << "\n\nvolviendo al menu principal..." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            Client.~Player();
            initGame();
        }
        fflush(NULL);
    }
    pthread_t listen_thread,
              p1Send_thread;

    pthread_create(&listen_thread, NULL, Client.listenServerHelper,&Client);
    pthread_create(&p1Send_thread, NULL, Client.sendToServerHelper,&Client);
//    int i = 0;
    while(getPlayerOneScore() < 3 && getPlayerTwoScore() < 3) {
        fflush(NULL);
        fflush(stdin);
        fflush(stdout);
        setBallYPos(Client.getBallYPos());
        setBallXPos(Client.getBallXPos());
        setPlayerOneScore( Client.getPlayerOneScore());
        setPlayerTwoScore( Client.getPlayerTwoScore());
        ballHandler( Client.getPlayerOneYPos(), Client.getPlayerTwoYPos() );
        showBoard( Client.getPlayerOneYPos(), Client.getPlayerTwoYPos() );
        std::cout.flush();
        fflush(NULL);
    }
    // unir hilos
    pthread_join(listen_thread, NULL);
    pthread_join(p1Send_thread, NULL);
    // conecciones cerradas del socket;
    Client.~Player();
    initGame();
}
using namespace std;
Game::Game() {
//    cout << "\n\n\n\t\tcargando...";
    system("clear");
}
Game::~Game() {
    system("clear");
    cout << "\nnGracias por jugar!";
}
int Game::getFPS() {
    return FPS;
}
void Game::setFPS(int n_fps) {
    FPS = n_fps;
}
bool Game::getServerStatus() {
    return serverStatus;
}
void Game::setServerStatus( bool n_status) {
    serverStatus = n_status;
}
int Game::getPort(){
    return  serverPort;
}
void Game::setPort(int n_port) {
    serverPort = n_port;
}
char * Game::getAddress() {
    return (char *) serverAddress;
}
void Game::setAddress(char *n_address) {
    strcpy(serverAddress, n_address);
}
int Game::getPlayers() {
    return players_count;
}
void Game::setPlayers() {
    players_count++;
}
int Game::getPlayerOneYPos() {
    return playerOnePos;
}
int Game::getPlayerTwoYPos() {
    return playerTwoPos;
}
int Game::getPlayerOneScore() {
    return playerOneScore;
}
int Game::getPlayerTwoScore(){
    return playerTwoScore;
}
void Game::setPlayerOnePos(int n_pos) {
    playerOnePos = n_pos;
}
void Game::setPlayerTwoPos(int n_pos) {
    playerTwoPos = n_pos;
}
void Game::setPlayerOneScore(int n_score) {
    playerOneScore = n_score;
}
void Game::setPlayerTwoScore(int n_score){
    playerTwoScore = n_score;
}
int Game::getPlayer1XPos() {
    return scoreBoard;
}
int Game::getPlayer2XPos() {
    return (width - 4 );
}

int Game::getBallXPos(){
    return ballXPosition;
}
void Game::setBallXPos(int n_pos){
    ballXPosition = n_pos;
}

int Game::getBallYPos(){
    return ballYPosition;
}
void Game::setBallYPos(int n_pos){
    ballYPosition = n_pos;
}

int Game::getBallXDir(){
    return ballXDirection;
}
void Game::setBallXDir(int n_xDir){
    ballXDirection = n_xDir;
}

int Game::getBallYDir(){
    return ballYDirection;
}
void Game::setBallYDir(int n_yDir){
    ballYDirection = n_yDir;
}

void *Game::rcvMessageHelper(void *p){
    Game *a = (Game *)p;
    char buffer[BUFFER_SIZE],
            resp[128];
    while( a->getPlayerOneScore() < 3 && a->getPlayerTwoScore() < 3){
        a->rcvMessage(buffer, resp);
        a->sendMessageTo(a->p1Sock, a->p1Client, resp);
        p=a; 
    }
    pthread_exit(NULL);
    free(a);
    delete(a);
    return NULL;
}

char* Game::rcvMessage(char * buffer, char * resp) {
    unsigned int len = sizeof(servaddr);

    if ( recvfrom(sockfd, (char *)buffer, BUFFER_SIZE,
                  0, ( struct sockaddr *) &p1Client,
                  &len) < 0 ) {
        perror("recvfrom");
    }
    else{
        buffer[len] = '\0';

        cout << "\n\n mensaje del cliente: " << inet_ntoa(p1Client.sin_addr);
        cout << "\n enviado del puerto " << ntohs(p1Client.sin_port);
        printf("\nCliente : %s", buffer);
        printf("\nCliente descifrado : %s\n", decrypt(buffer, key) );
        fflush(stdin);
        cout << "Mensaje manejado :" << handlingMessage(buffer, resp);
        cout << "\n Servidor :" << resp;
        fflush(stdin);
        cout << "\nservidor descifrado :  " << decrypt(resp, key) << endl;
        std::cout.flush();
    }
    return encryption(resp, key);
}

char* Game::rcvMessageP2(char * buffer, char * resp) {

    unsigned int len = sizeof(servaddr);

    if ( recvfrom(sockfd, (char *)buffer, BUFFER_SIZE,
                  0, ( struct sockaddr *) &p2Client,
             &len) < 0 ) {
        perror("recvfrom");
    }
    else{
        buffer[len] = '\0';
        cout << "\n\n Mensaje de la dirección del cliente: " << inet_ntoa(p2Client.sin_addr);
        cout << "\ny enviado desde el puerto: " << ntohs(p2Client.sin_port);
        printf("\nCliente : %s", buffer);
        printf("\nCliente descifrado: %s\n", decrypt(buffer, key) );
        fflush(stdin);
        cout << "Mensaje manejado :" << handlingMessage(buffer, resp);
        cout << "\n Servidor :" << resp;
        fflush(stdin);
        cout << "\nservidor descifrado :  " << decrypt(resp, key) << endl;
        std::cout.flush();
    }
    return encryption(resp, key);
}
void *Game::rcvMessageHelperP2(void *p){
    Game *a = (Game *)p;
    char buffer[BUFFER_SIZE],
            resp[128];
    while( a->getPlayerOneScore() < 3 && a->getPlayerTwoScore() < 3){
        a->rcvMessageP2(buffer, resp);
        a->sendMessageTo(a->p2Sock, a->p2Client, resp);
        p=a;
    }
    pthread_exit(NULL);
    free(a);
    delete(a);
    return NULL;
}

void Game::sendMessageTo(int id_socket, struct sockaddr_in client, char * resp) {

    if (  sendto(sockfd, (const char *)resp, strlen(resp),
                 MSG_DONTWAIT, (const struct sockaddr *) &client,
                    id_socket)< 0){
        perror("sendto");

    }
    else{
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 6 ppm
    }
}
int Game::kbhit(void){
    fflush(stdin);
    fflush(stdout);
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);;
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF){
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
char Game::controlInput() {
    struct termios old_tio, new_tio;
    unsigned char c;
    tcgetattr(STDIN_FILENO,&old_tio);
    new_tio=old_tio;
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    c=getchar();
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    return c;
}
char * Game::encryption(char* msg,  char* key) {

    if (!strlen(key))
        return msg;
    if (!strlen(msg))
        return msg;
    for(long unsigned int i = 0; i < strlen(msg); i++){

        msg[i] ^= key[i % strlen(key)]; 
    }
    return msg;
}
char * Game::decrypt(char *msg, char* key) {
    return encryption(msg, key); // reversed process
}
char Game::getRandomChar(){

    std::default_random_engine generator(1234);
    std::uniform_int_distribution<int> distribution(31,120);
    srand(time(NULL));
    char c = static_cast<unsigned char>( distribution(generator) );
    return c;
}
//dificultad del juego -> velocidad del juego
void Game::setDifficulty(int n_difficulty) {
    switch(n_difficulty){
        case 1:
            setFPS(150); // easy
            break;
        case 2:
            setFPS(50); // Medium
            break;
        case 3:
            setFPS(25); // Hard
            break;
        case 4:
            setFPS(10); // Progressive
            break;
    }
}

char* Game::handlingMessage(char * msg, char * resp) {

    if( msg[0] == '?' || msg[1] == '?') {
        setPlayers();
        resp[0] = '?';
        resp[1] = getPlayers() + '0';
        resp[2] = '+';
        resp[3] = gameMode + '0';
        resp[4] = '\0';
        if(getPlayers() == 1){
            memset(&p1Client, 0, sizeof(p1Client));
            p1Client.sin_family = cliaddr.sin_family;
            p1Client.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
            p1Client.sin_port = cliaddr.sin_port;
            p1Sock = sizeof(p1Client);
        }
        if( getPlayers() == 2){
            memset(&p2Client, 0, sizeof(p2Client));
            p2Client.sin_family = cliaddr.sin_family;
            p2Client.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
            p2Client.sin_port = cliaddr.sin_port;
            p2Sock = sizeof(p2Client);
        }

        return encryption(resp, key);
    }
    else if (msg[0] == '1' || msg[0] == '2'){
        if(msg[0] == '1'){
            if(p1UpKey == msg[2] ) {
                if(playerOnePos >= scoreBoard + 2)
                    playerOnePos--;
            }
            if(p1DownKey == msg[2])
                if(playerOnePos <= height + scoreBoard - 7)
                    playerOnePos++;
        }
        else {
            if(p2UpKey == msg[2] ) {
                if(playerTwoPos >= scoreBoard + 2)
                    playerTwoPos--;
            }
            if(p2DownKey == msg[2])
                if(playerTwoPos <= height + scoreBoard - 7)
                    playerTwoPos++;
        }
    }
        resp[0] = '!';
        if(playerOnePos >= 10){
            resp[1] = getPlayerOneYPos()/10 + '0';
            resp[2] = getPlayerOneYPos()%10 + '0';
            resp[3] = '+';

            if(playerTwoPos >= 10){
                resp[4] =  getPlayerTwoYPos()/10 + '0';
                resp[5] =  getPlayerTwoYPos()%10 + '0';
                resp[6] = '+';

                if( ballXPosition >= 10){
                    resp[7] = getBallXPos()/10 + '0';
                    resp[8] = getBallXPos()%10 + '0';
                    resp[9] = '+';
                    if(ballYPosition >= 10) {
                        resp[10] = getBallYPos()/10 + '0';
                        resp[11] = getBallYPos()%10 + '0';
                        resp[12] = '+';
                        resp[13] = '\0';

                    }
                    else{
                        resp[10] = getBallYPos() + '0';
                        resp[11] = '+';
                        resp[12] = '\0';


                    }

                    return encryption(resp, key);
                }
                else{
                    resp[7] = getBallXPos() + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';


                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }

                    return encryption(resp, key);
                }
            }
            else{
                resp[4] =  getPlayerTwoYPos() + '0';
                resp[5] = '+';

                if( ballXPosition >= 10){
                    resp[6] = getBallXPos()/10 + '0';
                    resp[7] = getBallXPos()%10 + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';
                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    return encryption(resp, key);
                }
                else{
                    resp[7] = getBallXPos() + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';
                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    return encryption(resp, key);
                }
            }
        }
        else {
            resp[1] = getPlayerOneYPos() + '0';
            resp[2] = '+';
            if(playerTwoPos >= 10){
                resp[3] =  getPlayerTwoYPos()/10 + '0';
                resp[4] =  getPlayerTwoYPos()%10 + '0';
                resp[5] = '+';

                if( ballXPosition >= 10){
                    resp[6] = getBallXPos()/10 + '0';
                    resp[7] = getBallXPos()%10 + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';
                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    return encryption(resp, key);
                }
                else{
                    resp[6] = getBallXPos() + '0';
                    resp[7] = '+';
                    if(ballYPosition >= 10) {
                        resp[8] = getBallYPos()/10 + '0';
                        resp[9] = getBallYPos()%10 + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    else{
                        resp[8] = getBallYPos() + '0';
                        resp[9] = '+';
                        resp[10] = '\0';
                    }
                    return encryption(resp, key);
                }
            }
            else{
                resp[4] =  getPlayerTwoYPos() + '0';
                resp[5] = '+';

                if( ballXPosition >= 10){
                    resp[6] = getBallXPos()/10 + '0';
                    resp[7] = getBallXPos()%10 + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';
                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    return encryption(resp, key);
                }
                else{
                    resp[7] = getBallXPos() + '0';
                    resp[8] = '+';
                    if(ballYPosition >= 10) {
                        resp[9] = getBallYPos()/10 + '0';
                        resp[10] = getBallYPos()%10 + '0';
                        resp[11] = '+';
                        resp[12] = '\0';
                    }
                    else{
                        resp[9] = getBallYPos() + '0';
                        resp[10] = '+';
                        resp[11] = '\0';
                    }
                    return encryption(resp, key);
                }
            }
        }
}

void Game::setServerUDP() {
    hostent * record = gethostbyname(getAddress());
    if(record == NULL){
        cout <<"\n" << getAddress() <<" no está disponible";
        exit(1);
    }
    in_addr * addressHost = (in_addr * )record->h_addr;
    char* ip_address = inet_ntoa(* addressHost);
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
//            perror("creacion del socket fallida");
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    memcpy(&servaddr.sin_addr, record->h_addr_list[0], record->h_length);
//        servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0  ){
//            perror("bind failed");
        perror("bind");
        exit(EXIT_FAILURE);
    }
    unsigned int len;
    len = sizeof(servaddr);
    system("clear");
    char buffer[BUFFER_SIZE],
            resp[32];
    int n;
    unsigned int  m = sizeof(n);
    getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);
    cout << "\nTamano del Socket = " << n << endl;

    cout << "El servidor se está ejecutando en: "
         << "\n\tdireccion : " << getAddress()
         << "\n\t\t: " << ip_address
         << "\n\t\t: " << inet_ntoa(servaddr.sin_addr)
         << "\n\tPuerto    : " << getPort()
         << "\n\tPuerto  : " << ntohs(servaddr.sin_port)
         << "\n\tVelocidad   : " << getFPS();

    int lennn, trysize, gotsize, err;
    lennn = sizeof(int);
    trysize = 1048576+32768;
    do {
        trysize -= 32768;
        setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,(char*)&trysize,lennn);
        err = getsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,(char*)&gotsize,(socklen_t*)&lennn);
        if (err < 0) { perror("getsockopt"); break; }
    } while (gotsize < trysize);
    printf("Tamaño establecido en %d\n",gotsize);

    trysize = 1048576+32768;
    gotsize = 0;
    do {
        trysize -= 32768;
        setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(char*)&trysize,lennn);
        err = getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(char*)&gotsize,(socklen_t*)&lennn);
        if (err < 0) { perror("getsockopt"); break; }
    } while (gotsize < trysize);
//    printf("Size set to %d\n",gotsize);

    while(getPlayers() < 2){
        cout << "\n\n esperando por jugadores...";
		cout<<endl<<"conectados en el servidor : " << getPlayers()<<"de 2 necesarios para iniciar";

        if( (recvfrom(sockfd, (char *)buffer, BUFFER_SIZE,
                      0, ( struct sockaddr *) &cliaddr,
                      &len) ) < 0 ){
            perror("recvfrom");
        }

        buffer[len] = '\0';
        cout << "\n\nmensaje enviado del socket " << ntohs(cliaddr.sin_port);
        cout << "\ny del cliente " << inet_ntoa(cliaddr.sin_addr);
        printf("\nCliente : %s", buffer);
        printf("\nCliente descifrado : %s\n", decrypt(buffer, key) );
        fflush(stdin);
        cout << "Mensaje manejado:" << handlingMessage(buffer, resp);
        cout << "\nServidor :" << resp;
        fflush(stdin);
        printf("\nServidor : %s", resp);
        fflush(stdin);
        printf("\nServidor descifrado: %s", decrypt(resp, key) );
        encryption(resp, key);
        fflush(stdin);
        if( (sendto(sockfd, (const char *)resp, strlen(resp),
                    0, (const struct sockaddr *) &cliaddr,
                    len) ) < 0 ){
            perror("sendto");
        }
    }
    if(getPlayers() == 2){
        char ready[16] = {']', 's','t','a','r','t', '\0'};
        encryption(ready, key);
        cout << "\n\nServidor :" << ready;
        fflush(stdin);
        printf("\nServidor : %s", ready);
        fflush(stdin);
        printf("\nServidor descifrado : %s", decrypt(ready, key) );
        encryption(ready, key);

        sendto(sockfd, (const char *)ready, strlen(ready),
               0, (const struct sockaddr *) &p1Client,
               p1Sock);
        sendto(sockfd, (const char *)ready, strlen(ready),
               0, (const struct sockaddr *) &p2Client,
               p2Sock);
    }
}

void Game::setServerTCP() {
    int sockfd, ret;
    int newSocket;
    struct sockaddr_in cliaddr;
    char buffer[BUFFER_SIZE],
            resp[32];
    pid_t childpid;
    hostent * record = gethostbyname(getAddress());

    if(record == NULL){
        cout <<"\n" << getAddress() <<" no está disponible";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket");
        exit(1);
    }
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    memset(&servaddr, '\0', sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    memcpy(&servaddr.sin_addr, record->h_addr_list[0], record->h_length);
    ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret < 0){
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, 10) == 0){
        printf("[+]recibiendo....\n");
    }else{
        printf("[-]Error en enlace...\n");
    }

    socklen_t addr_size = sizeof(cliaddr);
    while(!getServerStatus()){
        while(getPlayers() < 2){
            newSocket = accept(sockfd, (struct sockaddr*)&cliaddr, &addr_size);
            if(newSocket < 0){
                exit(1);
            }
            else{
                setPlayers();
                if(getPlayers() == 1){
                    memset(&p1Client, 0, sizeof(p1Client));
                    p1Client.sin_family = cliaddr.sin_family;
                    p1Client.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
                    p1Client.sin_port = cliaddr.sin_port;
                    p1Sock = newSocket;
                }
                if( getPlayers() == 2){
                    memset(&p2Client, 0, sizeof(p2Client));
                    p2Client.sin_family = cliaddr.sin_family;
                    p2Client.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
                    p2Client.sin_port = cliaddr.sin_port;
                    p2Sock = newSocket;
                }
            }
        }
        if((childpid = fork()) == 0){
            close(sockfd);
            while(getPlayerOneScore() < 3 && getPlayerTwoScore() < 3){
                recv(newSocket, buffer, BUFFER_SIZE, 0);
                if(strcmp(buffer, ":exit") == 0){
                    printf("Desconectado de %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                    break;
                }else{
                    cout << "\n\nMensaje enviado desde socket: " << cliaddr.sin_port;
                    cout << "\ny desde la dirección del cliente: " << cliaddr.sin_addr.s_addr;
                    cout << "\n mediante: " << cliaddr.sin_family;
                    printf("\nCliente : %s", buffer);
                    printf("\nCliente descifrado : %s", decrypt(buffer, key) );
                    fflush(stdin);
                    cout << "\nMensaje manejado :" << handlingMessage(buffer, resp);
                    cout << "\nServidor :" << resp;
                    fflush(stdin);
                    printf("\nServidor descifrado : %s", decrypt(resp, key) );
                    encryption(resp, key);

                    send(p1Sock, resp, strlen(resp), 0);
                    send(p2Sock, resp, strlen(resp), 0);
                    bzero(buffer, sizeof(buffer));
                    bzero(resp, sizeof(resp));
                }
            }
        }

    }

    close(newSocket);
    return ;
}

using namespace std;

Game Pong;
Core core;

void showControls(){
    system("clear");
    cout << "\n\n\t\t Controles de jugadores " << endl
        << "\t  Player 1  \t|\t  Player 2" << endl
        << "\tarriba   = w\t|\t arriba   = i" << endl
        << "\tabajo = s   \t|\t abajo = k" << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
}
void setAddressHelper(){
    cout << "Ingrese su nueva dirección : " ;
    string new_address;
    cin >> new_address;
    char *cstr = &new_address[0];
    core.setAddress( cstr );

}

void setPortHelper(){
    cout << "Ingrese su nuevo puerto : " ;
    int port;
    cin >> port;
    if(port >= 1024 && port < 65535){
        core.setPort(port);
    }
    else
        cout << "\nEl puerto dado no está en el rango de los puertos usables [1024, 65535[ " << endl;
}

void displayName(){
    system("clear");
    cout << "\n\n"
        << "\n\t\t   Juego Socket-PONG        "
        << "\n\n\n";
    this_thread::sleep_for(chrono::milliseconds(3000));

}
void displayMenu(){
    system("clear");
    system("color 0F");
    cout << "\n\n\n"
         << "\n\t\t    MENU           "
         << "\n\t\t  "
         << "\n\t\t  1 -> JUGAR"
         << "\n\t\t  2 -> OPCIONES"
         << "\n\t\t  3 -> ACERCA DE (README - ABOUT)"
         << "\n\t\t  q -> SALIR"
         << "\n\n\n";
    this_thread::sleep_for(chrono::milliseconds(200));
}
void displayOptions(){

    char key;
    do{
        system("clear");
        system("color 8F");
        cout << "\n\n\n"
             << "\n\t\t   OPCIONES             "
             << "\n\t\t "
             << "\n\t\t  1 -> SERVIDOR  "
             << "\n\t\t  2 -> PLAYER 1"
             << "\n\t\t  3 -> PLAYER 2"
             << "\n\t\t  4 -> VELOCIDAD   "
             << "\n\t\t  q -> SALIR\n\n";
        this_thread::sleep_for(chrono::milliseconds(200));
        key = Pong.controlInput();
        switch(key = Pong.controlInput() ){

            case '1':
               serverOptions();
                cin.sync();
                break;

            case '2':
                playerOptions();
                cin.sync();
                break;
            case('3'):
                cout << "configurar Player 2";
                break;
            case('4'):
                cout << "configurar velocidad del juego";
                break;
            case('q'):
                key ='q';
                break;
            default:
                break;
        }
    }while( (key = Pong.controlInput() ) != 'q');
}
void serverOptions(){
    char key;
    do{
        system("clear");
        cout << "\n\n\t\t Opciones actuales del servidor " << endl
             << "\n\tdireccion: " << core.getAddress() << endl
             << "\tPuerto : " << core.getPort() << endl
             << "\tModo de juego: " << core.getFPS() << endl
             << "\n\n ¿Estás seguro de continuar con la configuración actual?" << endl
             << "\n\t\t Y : si | N : no" << endl;

        if( tolower( Pong.controlInput() ) == 'y'){
            system("clear");
            cout << "Configurando el Servidor...\n" << endl;
            core.serverHandler();
        }
        else {
            system("clear");
            cout << "\n\n\t\tConfigurando el Servidor...\n" << endl
                 << "(1) cambiar direccion" << endl
                 << "(2) cambiar puerto" << endl
                 << "(3) cambiar modo de juego" << endl
                 << "(q) Cambiar dirección" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(200));
        key = Pong.controlInput();

        switch(key = Pong.controlInput() ){
            case '1':
                setAddressHelper();
                break;
            case '2':
               setPortHelper();
                break;
            case '3':
                break;
            case 'q':
                break;
        }
    } while( (key = Pong.controlInput() ) != 'q');
}
void playerOptions(){
    cout << "\n\n\t\t Opciones actuales del servidor " << endl
         << "\n\tdireccion: " << core.getAddress() << endl
         << "\tpuerto: " << core.getPort() << endl
         << "\tModo de juego: " << core.getFPS() << endl
         << "\n\n ¿Estás seguro de continuar con la configuración actual?" << endl
         << "\n\t\t Y : si | N : no" << endl;

    if( tolower( Pong.controlInput() ) == 'y'){
        system("clear");
        cout << "configurarcion Player 1";
        core.playerHandler();
    }
    cin.sync();

}

void displayAbout(){
    char key;
    do{
        system("clear");
        system("color 5F");
        cout << "\n\n\n";
        cout << "\n\t\t   ACERCA DE (README - ABOUT)  ";
        cout << "\n\t  Trabajo de Alberto Salinas Muñoz";
        cout << "\n\t  doy creditos a Nicolas Perez Roa, quien iba a ser mi compañero de trabajo";
		cout << "\n\t  pero por problemas personales no pudimos terminarlo juntos";
		cout << "\n\t  Pero me ayudo durante el desarrollo de este juego";
        cout << "\n\t  somos estudiates de la universidad tecnologica metropolitana,";
        cout << "\n\t  trabajo realizado para el ramo de comunicacion de datos.";
        cout << "\n\t  la base del codeo de los socket's usados fueron proporcionados";
        cout << "\n\t  por el profesor Sebastian Salazar (https://github.com/sebasalazar)";
        cout << "\n_____________00__________";
		cout << "\n____________0000_______";
		cout << "\n___________000000____";
		cout << "\n__________00000000______";
		cout << "\n________000000000000________";
		cout << "\n00000000000000000000000000000__";
		cout << "\n__0000000000_000_0000000000__";
		cout << "\n____00000000_000_00000000___";
		cout << "\n______000000_000_000000___";
		cout << "\n_______00000_000_00000___";
		cout << "\n_______000000000000000_____";
		cout << "\n______00000000000000000_______";
		cout << "\n_____000000000_000000000__________";
		cout << "\n____0000000_______0000000__________";
		cout << "\n___000000___________000000_________";
		cout << "\n__000___________________000_______";
		cout << "\n\n\n\n presiona \"q\" para volver al menu principal ...\n\n";
        this_thread::sleep_for(chrono::milliseconds(200));
        key = Pong.controlInput();
        switch(key){
            case('q'):
                key ='q';
                break;
            default:
                break;
        }
    }while( (key = Pong.controlInput() ) != 'q');
}

void initGame(){

    displayName();
    char key;
    do{
        displayMenu();
        this_thread::sleep_for(chrono::milliseconds(200));
        switch( key = Pong.controlInput() ){
            case '1':
                if(1){
                    system("clear");
                    cout << "Entrando en el juego";
                    showControls();
                    core.playerHandler();
                }
                else{
                }

                break;
            case '2':
                displayOptions();
                cout << "Entrar en Opciones";
                break;
            case('3'):
            	system("color 70");
                cout << "Entering About";
                displayAbout();
                system("color 70");
                break;
            case('q'):
                key ='q';
                break;
            default:
                break;
        }
    }while( (key = Pong.controlInput() ) != 'q');

}

using namespace std;

Player::Player() {
}

Player::~Player() {
    closeConnection();
}

int Player::getPlayerSide() {
    return playerSide;
}
void *Player::listenServerHelper(void *p) {
    Player *a = (Player *)p; 

    char buffer[BUFFER_SIZE];

    while(a->getPlayerOneScore() < 3 && a->getPlayerTwoScore() < 3){
        pthread_mutex_lock(&player_mutex); 
        a->listenServer(buffer);
        p=a;
        pthread_mutex_unlock(&player_mutex);
    }
    pthread_exit(NULL);
    free(a);
    delete(a);
    return NULL;
}
void Player::listenServer(char *buff) {

    unsigned int len;

    if (recvfrom(socket_fd, (char *)buff, BUFFER_SIZE,
                 MSG_WAITALL, ( struct sockaddr *) &server,
             &len) < 0 ){
        perror("recvfrom");

    }
    else{
        buff[len] = '\0';
        recvHandler(buff);

    }


}
void *Player::sendToServerHelper(void *p){
    Player *a = (Player *)p; // cast p to Player type


    while(a->getPlayerOneScore() < 3 && a->getPlayerTwoScore() < 3){
        a->sendToServer();
        p=a;
    }
    pthread_exit(NULL);
    free(a);
    delete(a);
    return NULL;
}
void Player::sendToServer(){

    char resp[65];
    createMessage(resp);

    if(sendto(socket_fd, (const char *) resp, strlen(resp) ,
           0, (const struct sockaddr *) &server,
           sizeof(server)) < 0){
        perror("sendto");
    }
    else{
        std::this_thread::sleep_for(std::chrono::milliseconds(166)); // 6pps
    }
}

char* Player::createMessage( char * mes_buff) {

    mes_buff[0] = getPlayerSide() + '0';
    mes_buff[1] = '+';
    cin.sync();
    if(!kbhit()){ // if key is not pressed, sends actual postion of player K
        mes_buff[2] = '.';
        mes_buff[3] = '+';
        if(getPlayerSide() == 1){
            if( getPlayerOneYPos() >= 10) {
                mes_buff[4] = getPlayerOneYPos()/10 + '0';
                mes_buff[5] = getPlayerOneYPos()%10 + '0';
                mes_buff[6] = '+';
                mes_buff[7] = '\0';
            }
            else{
                mes_buff[4] = getPlayerOneYPos() + '0';
                mes_buff[5] = '+';
                mes_buff[6] = '\0';
            }
        }
        if(getPlayerSide() == 2){
            if( getPlayerTwoYPos() >= 10) {
                mes_buff[4] = getPlayerTwoYPos()/10 + '0';
                mes_buff[5] = getPlayerTwoYPos()%10 + '0';
                mes_buff[6] = '+';
                mes_buff[7] = '\0';
            }
            else{
                mes_buff[4] = getPlayerTwoYPos() + '0';
                mes_buff[5] = '+';
                mes_buff[6] = '\0';
            }
        }
    }
    else{ //si se presiona la tecla, solicite actualizaciones actuales información actual del jugador
        mes_buff[2] = getchar();
        if(getPlayerSide() == 1){
            if( tolower(mes_buff[2])  == 'w'  )
                setPlayerOnePos(getPlayerOneYPos()+1);
            if( tolower(mes_buff[2])  == 's'  )
                setPlayerOnePos(getPlayerOneYPos()-1);
        }
        if(getPlayerSide() == 2){
            if( tolower(mes_buff[2])  == 'i'  )
                setPlayerTwoPos(getPlayerTwoYPos()+1);
            if( tolower(mes_buff[2])  == 'k'  )
                setPlayerTwoPos(getPlayerTwoYPos()-1);
        }
    }
    fflush(NULL);
    fflush(stdin);
    fflush(stdout);
    for(long unsigned int  i = 7 ; i < 32; i++)
        mes_buff[i] = getRandomChar();

    mes_buff[32] = '\0';
    return encryption(mes_buff, key);
}
void Player::recvHandler(char * msg) {
//    system("clear");
//    fflush(stdin);
    // Solicitud de conexión Manejador
    fflush(stdin);
     encryption(msg, key);

    if(msg != NULL){

        if( msg[0] == '?') {  // unirse al juego

            setPlayers(); // actualiza el total de jugadores
            playerSide = msg[1] - '0'; // le da al jugador k su lado número
            setDifficulty( msg[3] + '0');

        }
        if( msg[0] == ']'){ // empezar juego
            setServerStatus(true);
        }
        // Reproductor de datos de reproducción
        if( msg[0] == '!'){  // mientras juego.

            int i = 1,
                temp = 0,
                cont = 1;
            char * aux = msg;
            while( aux[i] != '\0'){
                if(aux[i] == '+'){
                    if (cont == 1){
                        setPlayerOnePos(temp);
                        temp = 0;
                    }
                    if(cont == 2){
                        setPlayerTwoPos(temp);
                        temp = 0;
                    }
                    if(cont == 3){
                        setBallXPos(temp);
                        if(getBallXPos() == (getPlayer1XPos() - 3 ) )
                            setPlayerTwoScore(getPlayerTwoScore() + 1);
                        if(getBallXPos() == (getPlayer2XPos() + 3))
                            setPlayerOneScore(getPlayerOneScore() + 1);
                        temp=0;
                    }
                    if(cont == 4){
                        setBallYPos(temp);
                        temp=0;
                    }
                    cont++;
                    i++;
                    continue;
                }

                if (aux[i] != '+' && temp < 100) {
                    if( aux[i] - '0' >= 0 && aux[i] - '0' <= 9 ){
                        temp *= 10;
                        temp += aux[i] - '0';
                    }
                }
                i++;
            }
        }
    }
    else{
        encryption(msg, key);
        return;
    }

}
void Player::sendMessage() {

    char rcv_buf[BUFFER_SIZE], // buffer para recibir
         msg_buff[33]; 
    createMessage(msg_buff);
        sendto(socket_fd, (const char *) msg_buff, strlen(msg_buff) ,
               0, (const struct sockaddr *) &server,
               sizeof(server));
    unsigned int len;
    fflush(stdin);
    if ( recvfrom(socket_fd, (char *) rcv_buf, BUFFER_SIZE,
                  MSG_DONTWAIT, (struct sockaddr *) &server,
                     &len) < 0 ) {
        len = 1;
    }
    rcv_buf[len] = '\0';
    recvHandler(rcv_buf);
}

void Player::joinGame() {

    cout << "Unirse al juego ..." << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    char join[BUFFER_SIZE],
            join_request[10] = {'?','R','e','q','u','e','s','t','\0'};

    encryption(join_request, key);

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    if (sendto(socket_fd, (const char *) join_request, strlen(join_request) ,
           0 , (const struct sockaddr *) &server,
           sizeof(server)) < 0 ){
        perror("sendto");
    }

    unsigned int join_size;

    if(recvfrom(socket_fd, (char *) join, BUFFER_SIZE,
             0, (struct sockaddr *) &server,
             &join_size) < 0 ){
        perror("recvfrom");
    }
    fflush(stdin);
    join[join_size] = '\0';
    recvHandler(join);
}

void Player::closeConnection() {
    close(socket_fd);
}

void Player::connectionUDP(){
    hostent * record = gethostbyname(getAddress());
    if(record == NULL){
        cout <<"\n" <<  getAddress() <<" no está disponible";
        exit(1);
    }
    in_addr * addressHost = (in_addr * )record->h_addr;
    char* ip_address = inet_ntoa(* addressHost);
    int port = getPort();
    cout << "\nServer Name Address: " << getAddress() << endl
         << "IPv4 Address: " << ip_address << endl
         << "Port: " << port << endl;

    //  Socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
     if ( socket_fd < 0 ) { 
         cout << "Error al abrir el zócalo";
         perror("socket");
         exit(1);
     }
    // Free and sets server address  information
    memset(&server, '\0', sizeof (server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, ip_address, &server.sin_addr);
}

void Player::connectionTCP() {
    // Averigua si la conexión es válida o no
    hostent * record = gethostbyname(getAddress());
    if(record == NULL){
        cout <<"\n" <<  getAddress() <<" no está disponible";
        exit(1);
    }
    in_addr * addressHost = (in_addr * )record->h_addr;
    char* ip_address = inet_ntoa(* addressHost);
    const char *hello = "Hello from client";

    char buffer[BUFFER_SIZE] = {0};

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( socket_fd < 0){
        printf("\n Error de creación de socket \n");
        return ;
    }
    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(getPort());
    memcpy(&server.sin_addr, record->h_addr_list[0], record->h_length);
    // Convierta direcciones IPv4 e IPv6 de texto a formato binario
    if(inet_pton(AF_INET, ip_address, &server.sin_addr)<=0) {
        printf("\nDirección no válida / Dirección no admitida \n");
        return ;
    }
    cout << "\nNombre del servidor Dirección: " << getAddress() << endl
         << "IPv4 direccion: " << ip_address << endl
         << "IPv4 direccion: " << server.sin_addr.s_addr << endl
         << "Puerto: " << server.sin_port << endl;

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("conectar");
        printf("\nLa conexión falló \n");
        return ;
    }
    while(getPlayerOneScore() < 3 && getPlayerTwoScore() < 3){
        send(socket_fd , hello , strlen(hello) , 0 );
        printf("Hola, mensaje enviado\n");
        read( socket_fd , buffer, BUFFER_SIZE);
        printf("%s\n",buffer );
    }
    return ;
}
