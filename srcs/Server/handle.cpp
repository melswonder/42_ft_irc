#include "../../include/Server.hpp"

//新しい接続があったとき、STLのfdをpushbackして追加する。
void Server::handleNewConnection() {
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int newFd = accept(_listeningSocketFd,(struct sockaddr *)&clientAddr,&addrLen);
    if (newFd < 0){
        std::cerr << "accept() error: " << std::strerror(errno) << std::endl;
        return ;
    }
    
    if(set_nonblocking(newFd) == -1)
        return ;
    struct pollfd newPollFd;
    newPollFd.fd = newFd;
    newPollFd.events = POLLIN;
    _pollFds.push_back(newPollFd);
    
    std::cout << RED << "New connection Fd:" << newPollFd.fd << " !" << WHI << std::endl;
}

//データが渡されたときの処理
void Server::handleClientData(int clientFd) {
    char buffer[1024];
    int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        disconnectClient(clientFd);
        return;
    }
    buffer[bytesRead] = '\0'; //readしたもんの末尾に\0をつけるようなもん。
    std::string message(buffer); //渡された文字にコマンドがあるかチェックするため別のに入れる。
    
    std::cout<< GRE << message << WHI <<std::endl;
    
    if(message.find("EXIT") == 0){ //よくわからないがサーバー側が終了する...
        disconnectClient(clientFd);
    }else if (message.rfind("PING", 0) == 0) { // message.find()の代わりにrfindを使うと、文字列の先頭から検索する意図が明確になります
    // PINGコマンドのパラメータ部分を抽出
    std::string parameter = message.substr(message.find(" ") + 1);

    // PONGにそのままパラメータを含めて応答
    std::string response = "PONG " + parameter;

    send(clientFd, response.c_str(), response.length(), 0);
}

    std::cout << "Received: " << buffer << std::flush; //サーバー側が出力
    
    // 単純なエコー応答（IRC形式）このsendがあると送ってくれた相手に送り返せる
    // std::string response = ":server PRIVMSG client :" + std::string(buffer) + "\r\n";
    // send(clientFd, response.c_str(), response.length(), 0);
}


//ここから下は仮　コマンド実装なら以下を参考にするべき。
// void Server::handleClientData(int clientFd) {
//     char buffer[1024];
//     int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    
//     if (bytesRead <= 0) {
//         disconnectClient(clientFd);
//         return;
//     }
    
//     buffer[bytesRead] = '\0';
//     std::string message(buffer);
    
//     std::cout << "Received: " << message << std::endl;
    
//     // IRCコマンドの解析と応答
//     if (message.find("NICK") == 0) {
//         // ニックネーム設定の応答
//         std::string response = ":server 001 " + extractNick(message) + " :Welcome to the IRC Network\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
//     else if (message.find("USER") == 0) {
//         // ユーザー登録の応答（通常NICKと組み合わせて使用）
//         // 001応答は既にNICKで送信済み
//     }
//     else if (message.find("PRIVMSG") == 0) {
//         // プライベートメッセージのエコー
//         std::string response = ":" + extractSender(message) + " " + message + "\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
//     else if (message.find("PING") == 0) {
//         // PING応答
//         std::string response = "PONG " + message.substr(5) + "\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
// }

// std::string extractNick(const std::string& nickCommand) {
//     size_t spacePos = nickCommand.find(' ');
//     if (spacePos != std::string::npos) {
//         return nickCommand.substr(spacePos + 1);
//     }
//     return "unknown";
// }

// Received: CAP LS
// NICK hirwatan
// USER hirwatan hirwatan localhost :Hiro Watanabe
