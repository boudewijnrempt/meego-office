import QtQuick 1.0

Rectangle {
    id: root
    default property alias actions: row.children

    height: row.height + 10
    color: "#cccccc"

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 10
    }
}

