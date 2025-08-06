#include "../include/echoServer.hpp"

//この関数はヘルパー関数です　後にbind()という関数で、
//ソケット通信に必要な情報を整理し、関数に渡すための手段を踏んでいます
void echoServer::setAddr(int port_number)
{
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;              //IPv4の使用
    _server_addr.sin_addr.s_addr = INADDR_ANY;      //どのIPからも接続を許可
    _server_addr.sin_port = htons(port_number);     //ポート番号設定
    
}

void echoServer::echoServerInit(char *argv[])
{
   
    _port = argv[1]; //有効なportかのチェックは必要　今は省略

    _listeningSocketFd = socket(AF_INET, SOCK_STREAM, 0); //socket これはネットワーク通信のエンドポイント（端点）ソケットを作成する関数
    if (_listeningSocketFd == -1) {
        std::cerr << "Error: Failed to create socket - " << strerror(errno) << std::endl;
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(_listeningSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //作成したソケットのオプションを設定する関数　SOL_SOCKETはソケット全般に関わるオプション SO_REUSEADDRはアドレスの再利用を許可するオプション
        std::cerr << "Error: setsockopt failed - " << strerror(errno) << std::endl;
        close(_listeningSocketFd);
        throw std::runtime_error("setsockopt failed");
    }
    

    // アドレス構造体の設定
    int port_number = std::atoi(argv[1]);
    setAddr(port_number);

    // バインド　作成した特定のソケットにローカルアドレスを割り当てるために使われる。
    // ソケットは作成した時点では単なる「通信窓口」であり、どのネットワークアドレスと関連付けられているかが決まっていない。
    // bindを使うことで、そのソケットを192.168.1.100の8080番ポートといった、具体的な
    //　「住所」と部屋番号に紐付ける。
    if (bind(_listeningSocketFd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
        std::cerr << "Error: Failed to bind to port " << _port << " - " << strerror(errno) << std::endl;
        close(_listeningSocketFd);
        throw std::runtime_error("Bind failed");
    }

    // リッスン bubdが完了したソケットは、まだ「接続を受け入れる準備」が出来ていません
    //　listenを使うことでｍソケットは「待ち受けキュー」を作成し、クライアントからの接続要求を
    // 一時的にとどめておくことができる・
    //　これによりサーバーは複数のクライアントからの接続要求を順番に処理できるようになる。
    if (listen(_listeningSocketFd, 10) == -1) {
        std::cerr << "Error: Failed to listen - " << strerror(errno) << std::endl;
        close(_listeningSocketFd);
        throw std::runtime_error("Listen failed");
    }

    struct pollfd serverPollFd;
    serverPollFd.fd = _listeningSocketFd;
    serverPollFd.events = POLLIN;
    _pollFds.push_back(serverPollFd);

    // std::cout << "=== Server Initialization Complete ===" << std::endl;
}
