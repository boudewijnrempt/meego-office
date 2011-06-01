import QtQuick 1.0
import org.calligra.mobile 1.0
import MeeGo.Components 0.1

Window {
    id: window
    signal documentOpened(variant newView)

    bookMenuModel: [ qsTr("All Documents"), qsTr("PDF Documents"), qsTr("Presentations"), qsTr("Spreadsheets"), qsTr("Text Documents") ];
    bookMenuPayload: [ allDocumentsPageComponent, pdfDocumentsPageComponent, presentationsPageComponent, spreadsheetsPageComponent, textDocumentsPageComponent ];
    
    Component { id: allDocumentsPageComponent; DocumentsPage { showType: true; } }
    Component { id: pdfDocumentsPageComponent; DocumentsPage { pageTitle: qsTr("PDF Documents"); filter: DocumentListModel.PDFDocumentType; } }
    Component { id: presentationsPageComponent; DocumentsPage { pageTitle: qsTr("Presentations"); filter: DocumentListModel.PresentationType; } }
    Component { id: spreadsheetsPageComponent; DocumentsPage { pageTitle: qsTr("Spreadsheets"); filter: DocumentListModel.SpreadsheetType; } }
    Component { id: textDocumentsPageComponent; DocumentsPage { pageTitle: qsTr("Text Documents"); filter: DocumentListModel.TextDocumentType; } }

    Component.onCompleted: { switchBook(allDocumentsPageComponent); }

    function openUrl(url) {
        Qt.openUrlExternally(url);
    }

    Settings {
        id: settings;
    }
}

