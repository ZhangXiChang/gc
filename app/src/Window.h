#pragma once
#include <iostream>
#include <optional>

#include <QWidget>

#include <hv/TcpServer.h>
#include <hv/TcpClient.h>

namespace Ui
{
    class Window;
} // namespace Ui

struct DataPacket
{
    size_t body_len;
    char body[1024];
};

class Window : public QWidget
{
private:
    Ui::Window *mUi = nullptr;

    hv::TcpClient *mClient = nullptr;
    int mClientSocket = 0;
    std::optional<hv::SocketChannelPtr> mHubPeerServer;

    hv::TcpClient *mP2PTempClient = nullptr;
    int mP2PTempClientSocket = 0;

    hv::TcpServer *mP2PServer = nullptr;
    int mP2PServerSocket = 0;
    std::vector<hv::SocketChannelPtr> mP2PPeerClientList;

    hv::TcpClient *mP2PClient = nullptr;
    int mP2PClientSocket = 0;
    std::optional<hv::SocketChannelPtr> mP2PPeerServer;

public:
    Window(QWidget *parent = nullptr);
    ~Window();
};
