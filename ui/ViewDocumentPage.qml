import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

AppPage {
    id: viewDocumentPage;

    pageTitle: settings.currentName;
    anchors.fill: parent;

    ViewLoader {
        id: view;
        anchors.fill: parent;
    }

    actionMenuModel: [ qsTr("About") ];
    actionMenuPayload: [ 0 ];

    onActionMenuTriggered: {
        if(selectedItem == 0) {
            aboutDialog.show();
        }
    }

    Component.onCompleted: {
        view.setFile(settings.currentFile);
    }

    onActivated: {
        view.loadDocument();
    }

    onDeactivating: {
        window.showToolBarSearch = false;
    }

    onDeactivated: {
        settings.reset();
    }

    AboutDialog {
        id: aboutDialog;
    }
}