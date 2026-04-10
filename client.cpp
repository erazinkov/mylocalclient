#include "client.h"

#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGuiApplication>
#include <QTimer>
#include <QMessageBox>

#include "data.h"

#include <QChartView>
#include <QChart>


Client::Client(QWidget *parent)
    : QDialog(parent),
      hostLineEdit(new QLineEdit("fortune")),
      getFortuneButton(new QPushButton(tr("Get Fortune"))),
      statusLabel(new QLabel(tr("This examples requires that you run the "
                                "Local Fortune Server example as well."))),
      socket(new QLocalSocket(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_serverProcess = new QProcess(this);

    m_series = new QLineSeries();
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setAnimationOptions(QChart::AllAnimations);
    m_chart->legend()->setVisible(false);
    m_chart->createDefaultAxes();
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLabel *hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostLineEdit);

    statusLabel->setWordWrap(true);

    getFortuneButton->setDefault(true);
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    QPushButton *serverButton = new QPushButton(tr("Server"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(serverButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_5_10);

    connect(serverButton, &QPushButton::clicked,
            this, [=]()
    {
        QString serverString{"/home/egor/projects/local-socket/local-server/build/mylocalserver"};
        auto state{m_serverProcess->state()};
        switch (state) {
        case QProcess::NotRunning:
            m_serverProcess->start(serverString, {});
            break;
            case QProcess::Starting:
            case QProcess::Running:
            break;
        }
        if (state == QProcess::NotRunning) {

        }
        m_serverProcess->start(serverString, {});
    });

    connect(hostLineEdit, &QLineEdit::textChanged,
            this, &Client::enableGetFortuneButton);
    connect(getFortuneButton, &QPushButton::clicked,
            this, &Client::requestNewFortune);
    connect(quitButton, &QPushButton::clicked, this, &Client::close);

    connect(socket, &QLocalSocket::connected, this, [](){
        qDebug() << "connected";
    });
    connect(socket, &QLocalSocket::disconnected, this, [](){
        qDebug() << "disconnected";
    });

    connect(socket, &QLocalSocket::readyRead, this, &Client::readFortune);
    connect(socket, &QLocalSocket::errorOccurred, this, &Client::displayError);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostLineEdit, 0, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    mainLayout->addWidget(chartView, 4, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    hostLineEdit->setFocus();
}

void Client::requestNewFortune()
{
    getFortuneButton->setEnabled(false);
    blockSize = 0;
    socket->abort();
    socket->connectToServer(hostLineEdit->text());
}

void Client::readFortune()
{
    qDebug() << "readFortune";
    qDebug() << blockSize;
    if (blockSize != 0) {
        // Relies on the fact that QDataStream serializes a quint32 into
        // sizeof(quint32) bytes
        if (socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize || in.atEnd())
        return;

    QString nextFortune;
    Data data;
//    in >> nextFortune;
    in >> data;
    nextFortune = data.str;
    m_series->replace(data.points);
    qDebug() << m_series->points();
    QAbstractAxis *axisX = m_chart->axes(Qt::Horizontal).first();
    QAbstractAxis *axisY = m_chart->axes(Qt::Vertical).first();

    axisX->setRange(-5.0, 5.0);
    axisY->setRange(0.0, 5'000);

    axisX = nullptr;
    axisY = nullptr;
    if (nextFortune == currentFortune) {
        QTimer::singleShot(0, this, &Client::requestNewFortune);
        return;
    }

    currentFortune = nextFortune;
    statusLabel->setText(currentFortune);
    getFortuneButton->setEnabled(true);
}

void Client::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        QMessageBox::information(this, tr("Local Fortune Client"),
                                 tr("The host was not found. Please make sure "
                                    "that the server is running and that the "
                                    "server name is correct."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Local Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the server name "
                                    "is correct."));
        break;
    case QLocalSocket::PeerClosedError:
        break;
    default:
        QMessageBox::information(this, tr("Local Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }

    getFortuneButton->setEnabled(true);
}

void Client::enableGetFortuneButton()
{
    getFortuneButton->setEnabled(!hostLineEdit->text().isEmpty());
}
