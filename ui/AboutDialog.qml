import QtQuick 1.0
import MeeGo.Components 0.1

ModalDialog {
    title: qsTr("About MeeGo Office Suite");

    showCancelButton: false;
    height: window.height / 2;

    content: Column {
        anchors.fill: parent;

        Image {
            anchors.horizontalCenter: parent.horizontalCenter;
            source: "/usr/share/icons/hicolor/128x128/apps/meego-office-suite.png";
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "MeeGo Office Suite\nVersion " + MEEGO_OFFICE_VERSION;
            font.pixelSize: 16;
            horizontalAlignment: Text.AlignHCenter;
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "\nPowered by the Calligra Engine\nVersion " + CALLIGRA_VERSION_STRING;
            font.pixelSize: 16;
            horizontalAlignment: Text.AlignHCenter;
        }
    }
}
