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
            source: "image://systemicon/meego-office-suite";
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "MeeGo Office Suite, Version " + MEEGO_OFFICE_VERSION;
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "\nPowered by the Calligra Engine";
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "Version " + CALLIGRA_VERSION_STRING;
        }
    }
}