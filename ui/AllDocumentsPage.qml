import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

AppPage {
    id: viewByNamePage;

    property alias model: picker.model;

    pageTitle: qsTr("All Documents");

    anchors.fill: parent;

    DocumentPicker {
        id: picker;
        anchors.fill: parent;
    }

    actionMenuModel: [ qsTr("By Name"), qsTr("By Type") ];
    actionMenuPayload: [ DocumentListModel.GroupByName, DocumentListModel.GroupByDocType ];
    actionMenuTitle: qsTr("Sort");

    onActionMenuTriggered: {
        console.log("ActionMenuTriggered: " + selectedItem);
        model.groupBy(selectedItem);
    }
}