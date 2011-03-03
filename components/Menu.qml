import QtQuick 1.0

Item {
    id: root
    anchors.fill: parent
    state: "hidden"
    property alias contentWidth: content.width

    property list<Action> actions

    Rectangle {
        id: backgroundFader
        anchors.fill: parent
        color: "#000000"
        opacity: 0.5

        MouseArea { anchors.fill: parent; onClicked: root.hide() }
    }

    Image {
        id: arrowHead
        source: "resources/arrowhead.png"
    }

    Rectangle {
        id: content
        height: actions.length * 30 // ## Get this from fontmetrics
        clip: true
        width: 120
        border { color: "white"; width: 1 }
        radius: 5
        color: "white"

        ListView {
            id: listView
            model: actions
            interactive: false
            anchors.fill: parent

            delegate: Text {
                width: ListView.view.width
                text: "  " + model.modelData.text
                height: 30 // ## implicitHeight + 15
                verticalAlignment: Text.AlignVCenter

                Rectangle {
                    id: bottomBorder
                    width: parent.width
                    anchors.top: parent.bottom
                    height: 1
                    color: "gray"
                    opacity: index == root.actions.length - 1 ? 0 : 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: { model.modelData.trigger(); root.hide() }
                }
            }
        }
    }

    function show(x, y) {
        arrowHead.x = x + contentWidth/2 - arrowHead.width/2
        arrowHead.y = y
        content.x = x
        content.y = y + arrowHead.height - 2
        state = "visible"
    }

    function hide() {
        state = "hidden"
    }

    states: [
        State {
            name: "visible"
            PropertyChanges {
                target: root
                opacity: 1
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: root
                opacity: 0
            }
        }
    ]

    Behavior on opacity { NumberAnimation { duration: 200 } }
}

