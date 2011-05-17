#include "PdfServerThread.h"
#include "PdfDocument.h"
#include "PdfDocumentCache.h"
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QByteArray>

PdfServerThread::PdfServerThread(PdfDocumentCache *documentCache, int socketDescriptor, QObject *parent)
    : QThread(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_documentCache(documentCache)
{
}

void PdfServerThread::run()
{
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Got an error setting socketdescriptor" << socket.error();
        emit sigError(socket.error());
    }

    // Make sure the headers are all here
    while(!socket.waitForReadyRead(-1)) {
        sleep(10);
    }

    QByteArray answer;
    QString socketError;

    // first line is GET
    if (socket.canReadLine()) {
        QString line = socket.readLine();
        QStringList tokens = line.split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens.length() < 2 || tokens[0] != "GET") {
            socketError = "Not a GET request" + line;
        }
        QStringList uri = tokens[1].split("?");
        if (tokens.length() < 2) {
            socketError = "No Command" + line;
        }

        if (socketError.isEmpty()) {

            QString command = uri[0];
            if (command == "/open") {
                answer = open(uri);
                if (answer.length() == 0) {
                    socketError = "Could not open document: " + line;
                }
            }
            else if (command == "/getpage") {
                answer = getpage(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get page: " + line;
                }
            }
            else if (command == "/thumbnail") {
                answer = thumbnail(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get thumbnail: " + line;
                }
            }
            else if (command == "/search") {
                answer = search(uri);
                if (answer.length() == 0) {
                    socketError = "Could not execute search: " + line;
                }
            }
            else if (command == "/text") {
                answer = text(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get text: " + line;
                }
            }
            else if (command == "/links") {
                answer = links(uri);
                if (answer.length() == 0) {
                    socketError = "Could not get links: " + line;
                }
            }
            else {
                socketError = "Illegal command" + line;
            }
        }
    }
    else {
        socketError = "Could not read line";
    }
    if (socketError.isEmpty()) {
        QString reply("HTTP/1.1 200 Ok\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n");
        socket.write(reply.toUtf8());
        socket.write(answer);
    }
    else {
        qDebug() << socketError;
        QString reply("HTTP/1.1 400 Bad Request\r\n");
        socket.write(reply.toAscii());
    }
    socket.flush();
}


QByteArray PdfServerThread::open(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 2) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    QString s("numPages=%1\n"
              "PageLayout=%2\n");

    s = s.arg(doc->numberOfPages()).arg(doc->pageLayout());
    QMap<QString, QString> infomap = doc->infoMap();
    foreach(const QString key, infomap.keys()) {
        s.append(QString("%1=%2\n").arg(key).arg(infomap[key]));
    }

    answer = s.toUtf8();
    return answer;
}

QByteArray PdfServerThread::getpage(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    return answer;
}

QByteArray PdfServerThread::thumbnail(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    return answer;
}

QByteArray PdfServerThread::search(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    return answer;
}

QByteArray PdfServerThread::text(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 4) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    return answer;
}

QByteArray PdfServerThread::links(const QStringList &uri)
{
    QByteArray answer;

    if (uri.length() != 3) return answer;

    PdfDocument *doc = m_documentCache->document(uri[1]);
    if (!doc || !doc->isValid()) {
        delete doc;
        return answer;
    }

    return answer;
}
