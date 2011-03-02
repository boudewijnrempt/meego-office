import QtQuick 1.0

Rectangle {
    id: root
    signal clicked()
    property alias image: icon.source
    property string borderPosition

    width: icon.width + 10
    height: icon.height 
    color: "transparent"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }

    Rectangle { 
        id: border
        color: "#b5b5b5"
        width: 1
        height: root.height 
        visible: borderPosition != ""
        anchors.left: borderPosition == "left" ? root.left : root.right
    }

    Image {
        id: icon
        width: 32
        height: 32
        anchors.centerIn: parent
    }

    states: [
        State {
            name: "pressed"
            when: mouseArea.pressed
            PropertyChanges { target: root; color: "#ccd8ff"; }
        }
    ]
}
