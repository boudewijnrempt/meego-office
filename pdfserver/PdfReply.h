#ifndef PDFREPLY_H
#define PDFREPLY_H

#include <QtCore/QHash>
#include <QtCore/QByteArray>
#include <QtCore/QUrl>

class PdfReply
{
public:
    enum Status {
        Undefined_Status = 0,
        OK_Status = 200,
        Error_Status = 400,
        NotFound_Status = 404,
        InternalError_Status = 500,
    };

    enum ContentType {
        Image_ContentType,
        Text_ContentType,
        Html_ContentType,
    };

    PdfReply();
    virtual ~PdfReply();

    Status status();

    void setStatus(Status status);
    void setContentType(ContentType type);
    void setData(const QByteArray &data);
    void setProperty(const QString &name, const QString &value);
    void setErrorString(const QString &error);

    qint64 write(QIODevice &out);

private:
    Status m_status;
    ContentType m_contentType;

    QHash<QString, QString> m_properties;
    QByteArray m_data;
};

#endif // PDFREPLY_H
