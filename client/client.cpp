#include "client.h"
#include "ui_client.h"
#include <stdlib.h>

client::client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::client)
{
    ui->setupUi(this);
    socket = new QTcpSocket (this);
    QTcpSocket::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void client::readyRead ()
{
    while (socket->canReadLine ())
    {
        QString line = QString::fromUtf8(socket->readAll()).trimmed();

        ui->textBrowser->append (line);
    }
}

client::~client()
{
    delete ui;
}

void client::connect ()
{
    ip = ui->ipLine->displayText ();
    port = ui->portLine->displayText ().toInt ();
    socket->connectToHost (ip, port);

    socket->waitForConnected (3000);

    if (socket->isReadable ())
        ui->textBrowser->append ("Connected!");
}

void client::disconnect ()
{
    socket->close ();
}

void client::on_connect_clicked()
{
    client::connect ();
    ui->lineEdit->setFocus ();
}

void client::on_disconnect_clicked()
{
    client::disconnect ();
    ui->textBrowser->append ("Disconnected!");
}

void client::on_lineEdit_returnPressed()
{
    QString message = ui->lineEdit->displayText ();
    socket->write (QString (message + "\n").toUtf8());
    ui->lineEdit->clear ();
    ui->lineEdit->setFocus ();
}

