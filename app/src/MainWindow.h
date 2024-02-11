#pragma once
#include <QWidget>

#include <hv/TcpServer.h>
#include <hv/TcpClient.h>

#include <optional>

namespace Ui
{
    class MainWindow;
} // namespace Ui

struct DataPacket
{
    size_t body_len;
    char body[1024];
};

class MainWindow : public QWidget
{
private:
    Ui::MainWindow *mUi = nullptr;

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
