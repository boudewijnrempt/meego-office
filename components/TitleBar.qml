import QtQuick 1.0

Rectangle {
    id: root
    property alias title: title.text
    property alias leftArea: leftRow.children
    property alias rightArea: rightRow.children

    height: title.height + 20
    color: "#cccccc"

    Text {
        id: title
        anchors.centerIn: parent
        font.pointSize: 14
    }

    Row {
        id: leftRow
        anchors.left: root.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
    }

    Row {
        id: rightRow
        anchors.right: root.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
    }
}

