import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

AppPage {
    id: recentDocumentsPage;

    pageTitle: qsTr("Recent Documents");
    anchors.fill: parent;
    
    DocumentPicker {
        anchors.fill: parent;
    }
}