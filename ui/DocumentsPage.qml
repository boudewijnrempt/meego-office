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
            window.showToolBarSearch = false;
	    openFile(filePath, fileType, uuid);
        }
    }

    Component { id: viewDocumentPageComponent; ViewDocumentPage { } }

    actionMenuModel: [ qsTr("About") ]; //qsTr("All"), qsTr("Recently Added"), qsTr("Favourites"), qsTr("Last Viewed") ];
    actionMenuPayload: [ 0 ];

    onActionMenuTriggered: {
        if(selectedItem == 0) {
            aboutDialog.show();
        }
    }

    onActivating: picker.model.refresh();

    function openFile(file, type, uuid) {
        settings.currentFile = file;
        settings.currentUuid = uuid;
        settings.currentName = file.substr(file.lastIndexOf("/") + 1);
        settings.currentType = type;
        window.actionMenuActive = false;
        window.addPage(viewDocumentPageComponent);
    }

    AboutDialog {
        id: aboutDialog;
    }
}
