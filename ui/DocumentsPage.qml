import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

AppPage {
    id: documentsPage;

    anchors.fill: parent;
    pageTitle: qsTr("All Documents");

    property alias filter: picker.filter;

    DocumentPicker {
        id: picker;
        anchors.fill: parent;

        onSelected: {
            settings.currentFile = filePath;
            window.addPage(viewDocumentPageComponent);
        }
    }

    Component { id: viewDocumentPageComponent; ViewDocumentPage { } }

    actionMenuModel: [ qsTr("All"), qsTr("Recently Added"), qsTr("Favourites"), qsTr("Last Viewed") ];
    actionMenuPayload: [ 1, 2, 3, 4 ];

    onActionMenuTriggered: {
        console.log("ActionMenuTriggered: " + selectedItem);
        //model.groupBy(selectedItem);
    }
}