#include "../../../include/Server.hpp"

// データが渡されたときの処理
void Server::handleClientData(int clientFd)
{
    char buffer[1024];
    int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0)
    {
        disconnectClient(clientFd);
        return;
    }
    buffer[bytesRead] = '\0';    // readしたもんの末尾に\0をつけるようなもん。
    std::string message(buffer); // 渡された文字にコマンドがあるかチェックするため別のに入れる。

    std::cout << GRE << message << WHI << std::endl;

    if (message.find("EXIT") == 0)
        disconnectClient(clientFd);
    else if (message.rfind("PING", 0) == 0)
    { // message.find()の代わりにrfindを使うと、文字列の先頭から検索する意図が明確になります
        // PINGコマンドのパラメータ部分を抽出
        std::string parameter = message.substr(message.find(" ") + 1);

        // PONGにそのままパラメータを含めて応答
        std::string response = "PONG " + parameter;

        send(clientFd, response.c_str(), response.length(), 0);
    }

    std::cout << "Received: " << buffer << std::flush; // サーバー側が出力

    // 単純なエコー応答（IRC形式）このsendがあると送ってくれた相手に送り返せる
    // std::string response = ":server PRIVMSG client :" + std::string(buffer) + "\r\n";
    // send(clientFd, response.c_str(), response.length(), 0);
}
