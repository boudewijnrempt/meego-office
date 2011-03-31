import QtQuick 1.0
import org.calligra.mobile 1.0

Rectangle {
    id: root
    signal search(string str)
    signal searchNext()
    signal finished()

    property alias input: textInput;

    height: textInputBorder.height + 20
    color: "#E8EEEE"
    border.color: "#869A99"
    border.width: 3
    radius: 3

    Rectangle {
        id: textInputBorder
        border.color: "#788DBC"
        border.width: 3
        radius: 3

        anchors.right: searchButton.left
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - searchButton.width - 100
        height: textInput.height + 5

        TextInput {
            id: textInput
            width: parent.width - 5
            anchors.centerIn: parent
            onTextChanged: root.search(text)
            Keys.onReturnPressed: root.searchNext()
            Keys.onEnterPressed: root.searchNext()
            Keys.onEscapePressed: root.finished()
        }
    }

    ToolButton {
        id: searchButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        image: "image://icon/search"
        onClicked: root.searchNext()
    }
}

