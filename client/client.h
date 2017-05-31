#ifndef CLIENT_H
#define CLIENT_H

#include <QCoreApplication>
#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class client;
}

class client : public QMainWindow
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0);
    ~client();

    void connect ();
    void disconnect ();

    QString ip;
    int port;

private slots:
    void on_connect_clicked();
    void on_disconnect_clicked();
    void on_lineEdit_returnPressed();
    void readyRead ();

private:
    Ui::client *ui;
    QTcpSocket *socket;
};

#endif // CLIENT_H
