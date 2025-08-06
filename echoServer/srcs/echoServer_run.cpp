#include "../include/echoServer.hpp"

void echoServer::echoServerRun() {
    std::cout << "Server is running and listening on port " << _port << std::endl;
    while (true) {
        int numEvents = poll(&_pollFds[0], _pollFds.size(), -1);

        if (numEvents < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "poll() error" << std::endl;
            break;
        }

        
        // numEventsが0の場合(タイムアウト時)の処理を省略
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents == 0) {
                continue;
            }

            // 接続街のソケットからのイベント
            if (_pollFds[i].fd == _listeningSocketFd) {
                if (_pollFds[i].revents & POLLIN) { // POLLIN: 読み込み可能
                    handleNewConnection();
                }
            }
            // 既存のクライアントソケットにイベントがあった場合
            else {
                if (_pollFds[i].revents & POLLIN) {
                    handleClientData(_pollFds[i].fd);
                }else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    std::cerr << "Client disconnected unexpectedly" << std::endl;
                    disconnectClient(_pollFds[i].fd);
                }
            }
        }
    }
}