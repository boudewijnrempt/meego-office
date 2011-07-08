#ifndef PDFSEARCH_H
#define PDFSEARCH_H

#include <QtCore/QMetaType>
#include <KoFindBase.h>
#include "PDFPage.h"

class PDFDocument;
class PDFSearch : public KoFindBase
{
    Q_OBJECT
public:
    PDFSearch( PDFDocument *document, QObject *parent = 0);
    virtual ~PDFSearch();

    QString currentPattern();

Q_SIGNALS:
    void searchUpdate();

protected:
    virtual void replaceImplementation ( const KoFindMatch& match, const QVariant& value );
    virtual void findImplementation ( const QString& pattern, KoFindBase::KoFindMatchList& matchList );
    virtual void clearMatches();

private:
    class Private;
    const QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void searchFinished());
};

Q_DECLARE_METATYPE(PDFPage*);

#endif // PDFSEARCH_H
