import QtQuick 1.0

Rectangle {
    id: root
    signal clicked()
    property alias image: icon.source

    width: 32
    height: 32
    color: "transparent"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }

    Image {
        id: icon
        anchors.fill: parent
    }

    states: [
        State {
            name: "pressed"
            when: mouseArea.pressed
            PropertyChanges { target: root; color: "blue"; }
        }
    ]
}
