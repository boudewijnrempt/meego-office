#ifndef PDFSERVERTHREAD_H
#define PDFSERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>

class PdfDocumentCache;
class PdfReply;

class PdfServerThread : public QThread
{
    Q_OBJECT

public:

    explicit PdfServerThread(PdfDocumentCache *documentCache, int socketDescriptor, QObject *parent = 0);
    virtual void run();

    QString url() { return m_url; }

signals:

    void sigError(QTcpSocket::SocketError error);

private:

    /**
     * open a document to get the number of pages
     * @param url The request url. Uses 
     * @return utf-8 encoded bytearray filled with
     * 0: url=$url
     * 1: numberofpages=$number of pages
     * 2: pagelayout=$pagelayout*/
    void open(const QUrl &url, PdfReply &reply);

    /**
     * render the page on a png and return it
     * @param uri: 4 components
     * 0: "getpage"
     * 1: absolute path to pdf file on the server
     * 2: pagenumber
     * 3: zoomlevel
     * @return utf-8 encoded bytearray filled with
     * 0: url=$url
     * 1: pagenumber=$pagenumber
     * 2: zoomlevel=$zoomlevel
     * 3: orientation=$orientation
     * 4: imagesize=size of the png image
     * 5: -----------
     * 6: byte array representing a PNG image
     */
    void getpage(const QUrl &url, PdfReply &reply);

    /**
     * retrieve the PDF thumbnail or create one from the page
     * @param uri: 5 components
     * 0: "thumbnail"
     * 1: absolute path to pdf file on the server
     * 2: pagenumber
     * 3: width
     * 4: height
     * @return bytearray filled with
     * 0: url=$url
     * 1: pagenumber=$pagenumber
     * 2: width=$actual_width
     * 3: height=$actual_height
     * 4: imagesize=size of the png image
     * 5: -----------
     * 6: byte array representing a PNG image of the thumbnail
     */
    void thumbnail(const QUrl &url, PdfReply &reply);

    /**
     * Find the rectangles in postscript points that surround the matches
     * for the search string on the given page.
     * For now, the search string is not escaped or encoded, which might give
     * weird results if it contains a ?, or if it contains newlines.
     *
     * @param uri: 4 components
     * 0: "search"
     * 1: absolute path to pdf file on the server
     * 2: pagenumber
     * 3: search string
     * @return bytearray filled with
     * 0: url=$url
     * 1: pagenumber=$pagenumber
     * 2: searchstring=$searchstring.
     * 3: -----------
     * 4: left, top, right, bottom
     * n: left, top, right, bottom
     */
    void search(const QUrl &url, PdfReply &reply);

    /**
     * return the text under the given area. For now, the text is not encoded
     * before returning.
     *
     * @param uri: 2 components
     * 0: "text"
     * 1: absolute path to pdf file on the server
     * 2: pagenumber
     * 3: left
     * 4: top
     * 5: right
     * 6: bottom
     * @return bytearray filled with
     * 0: url=$url
     * 1: pagenumber=$pagenumber
     * 2: left=$left
     * 3: top=$top
     * 4: right=$right
     * 5: bottom=$bottom
     * 7: text=$text
     */
    void text(const QUrl &url, PdfReply &reply);

    /**
     * return a list of links on the given page. URL links return the url,
     * local links the pagenumber. Url's are designated by type=url, local links
     * by type="page".
     *
     * @param uri: 2 components
     * 0: "text"
     * 1: absolute path to pdf file on the server
     * 2: pagenumber
     * @return bytearray filled with
     * 0: url=$url
     * 1: pagenumber=$pagenumber
     * 2: numberoflinks=number of discovered links
     * 3: -----------
     * 4: top,left,right,bottom,type,target
     * n: top,left,right,bottom,type,target
     */
    void links(const QUrl &url, PdfReply &reply);

    void dpi( qreal& dpiX, qreal& dpiY, qreal zoomlevel);

    int m_socketDescriptor;
    PdfDocumentCache *m_documentCache;
    QString m_url;
};

#endif // PDFSERVERTHREAD_H
