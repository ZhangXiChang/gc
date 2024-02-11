#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    mUi = new Ui::MainWindow();
    mUi->setupUi(this);
    mUi->DisconnectTheCentralServerButton->setEnabled(false);
    mUi->HubServerConnectionStatusLabel->setText("离线");
    mUi->HubServerConnectionStatusLabel->setStyleSheet("color: red;");
    mUi->HubServerSelectionBox->addItem("北瓜中枢服务端", QVariant::fromValue(std::pair<std::string, uint32_t>("127.0.0.1", 10270)));
    mUi->CommunicationPanel->hide();
    mUi->MessagePanel->hide();
    mUi->P2PListenStop->setEnabled(false);
    mUi->P2PDisconnectButton->setEnabled(false);

    setFixedSize(size().width() - mUi->MessagePanel->size().width() - 10, size().height() - mUi->CommunicationPanel->size().height());

    connect(mUi->ConnectToTheCentralServerButton, &QPushButton::clicked, this,
            [this]()
            {
                mUi->HubServerSelectionBox->setEnabled(false);
                mUi->ConnectToTheCentralServerButton->setEnabled(false);
                mUi->DisconnectTheCentralServerButton->setEnabled(true);

                mClient = new hv::TcpClient();
                auto ip = mUi->HubServerSelectionBox->currentData().value<std::pair<std::string, uint32_t>>().first;
                auto port = mUi->HubServerSelectionBox->currentData().value<std::pair<std::string, uint32_t>>().second;
                mClientSocket = mClient->createsocket(port, ip.c_str());
                if (mClientSocket < 0)
                {
                    mUi->PromptMessageLabel->setText("创建连接中枢服务端的套接字失败！");
                    mUi->PromptMessageLabel->setStyleSheet("color: red;");
                    return;
                }
                mClient->onConnection = [this](const hv::SocketChannelPtr &channel)
                {
                    if (channel->isConnected())
                    {
                        mHubPeerServer = channel;
                        mUi->HubServerConnectionStatusLabel->setText("在线");
                        mUi->HubServerConnectionStatusLabel->setStyleSheet("color: green;");
                    }
                    else
                    {
                        mHubPeerServer.reset();
                        mUi->HubServerConnectionStatusLabel->setText("离线");
                        mUi->HubServerConnectionStatusLabel->setStyleSheet("color: red;");
                    }
                };
                mClient->onMessage = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                {
                    std::vector<char> packet(buf->size());
                    memcpy(packet.data(), buf->data(), buf->size());
                    DataPacket *data_packet = (DataPacket *)packet.data();
                    mUi->InformationBox->append(data_packet->body);
                };
                unpack_setting_t unpack_setting{};
                unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
                unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
                unpack_setting.length_field_offset = offsetof(DataPacket, body_len);
                unpack_setting.length_field_bytes = sizeof(size_t);
                unpack_setting.length_field_coding = ENCODE_BY_LITTEL_ENDIAN;
                unpack_setting.body_offset = offsetof(DataPacket, body);
                mClient->setUnpack(&unpack_setting);
                reconn_setting_t reconn{};
                reconn_setting_init(&reconn);
                reconn.min_delay = 1000;
                reconn.max_delay = 10000;
                reconn.delay_policy = 2;
                mClient->setReconnect(&reconn);
                mClient->start();
            });
    //
    connect(mUi->DisconnectTheCentralServerButton, &QPushButton::clicked, this,
            [this]()
            {
                mUi->HubServerSelectionBox->setEnabled(true);
                mUi->ConnectToTheCentralServerButton->setEnabled(true);
                mUi->DisconnectTheCentralServerButton->setEnabled(false);
                delete mClient;
                mClient = nullptr;
            });
    //
    connect(mUi->CommunicationPanelButton, &QPushButton::clicked, this,
            [this]()
            {
                if (mUi->CommunicationPanel->isHidden() && mUi->MessagePanel->isHidden())
                {
                    setFixedSize(size().width() + mUi->MessagePanel->size().width() + 10, size().height() + mUi->CommunicationPanel->size().height());
                    mUi->CommunicationPanel->show();
                    mUi->MessagePanel->show();
                }
                else
                {
                    mUi->CommunicationPanel->hide();
                    mUi->MessagePanel->hide();
                    setFixedSize(size().width() - mUi->MessagePanel->size().width() - 10, size().height() - mUi->CommunicationPanel->size().height());
                }
            });
    //
    connect(mUi->P2PListen, &QPushButton::clicked, this,
            [this]()
            {
                mUi->P2PListen->setEnabled(false);
                mUi->P2PListenStop->setEnabled(true);
                mUi->IsPublicP2PServer->setEnabled(false);
                mUi->P2PConnectionButton->setEnabled(false);
                mUi->P2PConnectionLineEdit->setEnabled(false);

                mP2PTempClient = new hv::TcpClient();
                if (mHubPeerServer.has_value())
                {
                    auto ip = mUi->HubServerSelectionBox->currentData().value<std::pair<std::string, uint32_t>>().first;
                    auto port = mUi->HubServerSelectionBox->currentData().value<std::pair<std::string, uint32_t>>().second + 1;
                    mP2PTempClientSocket = mP2PTempClient->createsocket(port, ip.c_str());
                }
                else
                {
                    mUi->InformationBox->append("未连接中枢服务端！");
                    return;
                }
                so_reuseaddr(mP2PTempClientSocket);
                if (mP2PTempClientSocket < 0)
                {
                    mUi->InformationBox->append("临时P2P套接字创建失败！");
                    return;
                }
                mP2PTempClient->onMessage = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                {
                    auto localAddr = channel->localaddr();
                    auto localPort = localAddr.substr(localAddr.find(':') + 1);
                    mP2PTempClient->closesocket();
                    mP2PServer = new hv::TcpServer();
                    mP2PServerSocket = mP2PServer->createsocket(std::stoi(localPort));
                    if (mP2PServerSocket < 0)
                    {
                        mUi->InformationBox->append("P2P套接字创建失败！");
                        return;
                    }
                    mP2PServer->onConnection = [this](const hv::SocketChannelPtr &channel)
                    {
                        if (channel->isConnected())
                        {
                            mP2PPeerClientList.push_back(channel);
                            mUi->InformationBox->append("一个新的P2P客户端连接");
                        }
                        else
                        {
                            mP2PPeerClientList.erase(std::remove(mP2PPeerClientList.begin(), mP2PPeerClientList.end(), channel), mP2PPeerClientList.end());
                            mUi->InformationBox->append("一个P2P客户端断开连接");
                        }
                    };
                    mP2PServer->onMessage = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                    {
                        std::vector<char> data(buf->size());
                        memcpy(data.data(), buf->data(), buf->size());
                        mUi->InformationBox->append(std::string(data.data()).c_str());
                        for (auto &&i : mP2PPeerClientList)
                        {
                            i->write(data.data(), data.size());
                        }
                    };
                    mP2PServer->onWriteComplete = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                    {
                        std::vector<char> data(buf->size());
                        memcpy(data.data(), buf->data(), buf->size());
                        mUi->InformationBox->append(std::string(data.data()).c_str());
                        mUi->TextInputBox->clear();
                    };
                    mP2PServer->setThreadNum(4);
                    mP2PServer->start();
                    mUi->InformationBox->append("P2P套接字创建完成！");

                    std::vector<char> data(buf->size());
                    memcpy(data.data(), buf->data(), buf->size());
                    mUi->InformationBox->append(("你的P2P端口为：" + std::string(data.data())).c_str());
                };
                reconn_setting_t reconn;
                reconn_setting_init(&reconn);
                reconn.min_delay = 1000;
                reconn.max_delay = 10000;
                reconn.delay_policy = 2;
                mP2PTempClient->setReconnect(&reconn);
                mP2PTempClient->start();

                if (mUi->IsPublicP2PServer->isChecked())
                {
                }
            });
    //
    connect(mUi->P2PListenStop, &QPushButton::clicked, this,
            [this]()
            {
                mUi->P2PListen->setEnabled(true);
                mUi->P2PListenStop->setEnabled(false);
                mUi->IsPublicP2PServer->setEnabled(true);
                mUi->P2PConnectionButton->setEnabled(true);
                mUi->P2PConnectionLineEdit->setEnabled(true);
                delete mP2PServer;
                mP2PServer = nullptr;
                delete mP2PTempClient;
                mP2PTempClient = nullptr;
            });
    //
    connect(mUi->P2PConnectionButton, &QPushButton::clicked, this,
            [this]()
            {
                mUi->P2PConnectionButton->setEnabled(false);
                mUi->P2PDisconnectButton->setEnabled(true);
                mUi->P2PListen->setEnabled(false);
                mUi->IsPublicP2PServer->setEnabled(false);
                mUi->P2PConnectionLineEdit->setEnabled(false);

                auto addrstr = mUi->P2PConnectionLineEdit->text().toStdString();
                auto ip = addrstr.substr(0, addrstr.find(':'));
                auto port = addrstr.substr(addrstr.find(':') + 1);
                if (ip.empty() || port.empty())
                {
                    mUi->InformationBox->append("请输入正确的IP地址和端口号！");
                    return;
                }
                mP2PClient = new hv::TcpClient();
                mP2PClientSocket = mP2PClient->createsocket(std::stoi(port), ip.c_str());
                if (mP2PClientSocket < 0)
                {
                    mUi->InformationBox->append("创建P2P套接字失败！");
                    return;
                }
                mP2PClient->onConnection = [this](const hv::SocketChannelPtr &channel)
                {
                    if (channel->isConnected())
                    {
                        mP2PPeerServer = channel;
                        mUi->InformationBox->append("P2P连接成功！");
                    }
                    else
                    {
                        mP2PPeerServer.reset();
                        mUi->InformationBox->append("P2P断开连接！");
                    }
                };
                mP2PClient->onMessage = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                {
                    std::vector<char> data(buf->size());
                    memcpy(data.data(), buf->data(), buf->size());
                    mUi->InformationBox->append(std::string(data.data()).c_str());
                };
                reconn_setting_t reconn;
                reconn_setting_init(&reconn);
                reconn.min_delay = 1000;
                reconn.max_delay = 10000;
                reconn.delay_policy = 2;
                mP2PClient->setReconnect(&reconn);
                mP2PClient->start();
            });
    //
    connect(mUi->P2PDisconnectButton, &QPushButton::clicked, this,
            [this]()
            {
                mUi->P2PConnectionButton->setEnabled(true);
                mUi->P2PDisconnectButton->setEnabled(false);
                mUi->P2PListen->setEnabled(true);
                mUi->IsPublicP2PServer->setEnabled(true);
                mUi->P2PConnectionLineEdit->setEnabled(true);
                delete mP2PClient;
                mP2PClient = nullptr;
            });
    //
    connect(mUi->TextInputBox, &QLineEdit::returnPressed, this,
            [this]()
            {
                if (mP2PPeerClientList.size() != 0)
                {
                    auto InputText = mUi->TextInputBox->text().toStdString();
                    for (auto &&i : mP2PPeerClientList)
                    {
                        i->write(InputText.c_str(), InputText.size());
                    }
                }
                else if (mP2PPeerServer.has_value())
                {
                    auto InputText = mUi->TextInputBox->text().toStdString();
                    mP2PPeerServer.value()->write(InputText.c_str(), InputText.size());
                    mUi->TextInputBox->clear();
                }
                else
                {
                    mUi->InformationBox->append("请先建立P2P通信！");
                }
            });
}
MainWindow::~MainWindow()
{
    delete mP2PClient;
    delete mP2PServer;
    delete mP2PTempClient;
    delete mClient;
    delete mUi;
}
