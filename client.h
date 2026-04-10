#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDialog>
#include <QDataStream>
#include <QLocalSocket>
#include <QLineEdit>
#include <QLabel>
#include <QLineSeries>
#include <QProcess>

class Client : public QDialog
{
    Q_OBJECT
public:
    Client(QWidget *parent = nullptr);
private slots:
    void requestNewFortune();
    void readFortune();
    void displayError(QLocalSocket::LocalSocketError socketError);
    void enableGetFortuneButton();

private:
    QLineEdit *hostLineEdit;
    QPushButton *getFortuneButton;
    QLabel *statusLabel;

    QLocalSocket *socket;
    QDataStream in;
    quint32 blockSize;

    QString currentFortune;

    QLineSeries *m_series;
    QChart *m_chart;
    QProcess *m_serverProcess;
};

#endif // CLIENT_H
