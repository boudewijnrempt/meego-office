import QtQuick 1.0

Item {
    id: base;
    anchors.fill: parent;

    property alias icon: icon.source;
    property alias title: title.text;

    Behavior on opacity {
        NumberAnimation { }
    }

    Rectangle {
        anchors.fill: parent;

        opacity: 0.5;

        color: "#000000";
    }

    Rectangle {
        anchors.centerIn: parent;

        width: parent.width / 2;
        height: about_dialog_content.height + 100;

        border.color: "#000000";
        border.width: 2;

        radius: 5;

        Column {
            id: about_dialog_content;
            anchors.centerIn: parent;
            anchors.topMargin: 50;

            spacing: 10;

            Image {
                id: icon;
                
                anchors.horizontalCenter: parent.horizontalCenter;
                source: "image://icon/help-info";
                width: 64;
                height: 64;
            }

            Text {
                id: title;
                
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "An Unknown Application"
                font.bold: true;
                font.pointSize: 20;
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "A Tablet Demo for MeeGo 1.1"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "Version: " + KOFFICE_VERSION_STRING;
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "OK";

                onTriggered: base.opacity = 0;

                image: "image://icon/dialog-ok";
                imageWidth: 32;
                imageHeight: 32;
            }
        }
    }
}
