import QtQuick 1.0

Rectangle {
    id: root
    property alias text: text.text
    signal clicked()

    width: Math.max(text.width, 80)
    height: text.height + 10

    color: mouseArea.pressed ? Qt.darker("#767676", 1.2) : "#767676"
    radius: 3
    smooth: true

    Text {
        id: text
        anchors.centerIn: parent
        color: "white"
        font.bold: true
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
