import QtQuick 1.0
import org.calligra.mobile 1.0
import MeeGo.Components 0.1

Window {
    id: window
    signal documentOpened(variant newView)

    bookMenuModel: [ qsTr("All Documents"), qsTr("Presentations"), qsTr("Spreadsheets"), qsTr("Text Documents") ];
    bookMenuPayload: [ allDocumentsPageComponent, presentationsPageComponent, spreadsheetsPageComponent, textDocumentsPageComponent ];
    
    Component { id: allDocumentsPageComponent; DocumentsPage { showType: true; } }
    Component { id: presentationsPageComponent; DocumentsPage { pageTitle: qsTr("Presentations"); filter: DocumentListModel.Presentations; } }
    Component { id: spreadsheetsPageComponent; DocumentsPage { pageTitle: qsTr("Spreadsheets"); filter: DocumentListModel.Spreadsheets; } }
    Component { id: textDocumentsPageComponent; DocumentsPage { pageTitle: qsTr("Text Documents"); filter: DocumentListModel.TextDocuments; } }

    Component.onCompleted: { switchBook(allDocumentsPageComponent); }

    Settings {
        id: settings;
    }

    function open(file) {
        window.pageStack.currentPage.openFile(file);
    }
}

