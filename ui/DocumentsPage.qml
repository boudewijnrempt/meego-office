import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

AppPage {
    id: documentsPage;

    anchors.fill: parent;
    pageTitle: qsTr("All Documents");

    property alias filter: picker.filter;
    property alias showType: picker.showType;

    DocumentPicker {
        id: picker;
        anchors.fill: parent;

        showHeader: true;

        onSelected: {
            settings.currentFile = filePath;
            settings.currentName = filePath.substr(filePath.lastIndexOf("/") + 1);
            window.actionMenuActive = false;
            window.addPage(viewDocumentPageComponent);
        }
    }

    Component { id: viewDocumentPageComponent; ViewDocumentPage { } }

    actionMenuModel: [ qsTr("All"), qsTr("Recently Added"), qsTr("Favourites"), qsTr("Last Viewed") ];
    actionMenuPayload: [ 1, 2, 3, 4 ];

    onActionMenuTriggered: {
        console.log("ActionMenuTriggered: " + selectedItem);
    }

    function openFile(file) {
        settings.currentFile = file;
        settings.currentName = file.substr(file.lastIndexOf("/") + 1);
        window.actionMenuActive = false;
        window.addPage(viewDocumentPageComponent);
    }
}