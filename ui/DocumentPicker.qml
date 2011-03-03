import QtQuick 1.0
import org.calligra.mobile 1.0

Item {
    id: root
    property alias model : documentListView.model
    signal selected(int index, string filePath)

    TitleBar {
        id: titleBar
        width: root.width
        title: qsTr("Calligra Mobile")

        rightArea: [
            // ## The two buttons below should become a popup component
            ToolButton {
                id: groupBy
                image: "image://icon/bookmarks-organize";
                borderPosition: "left"

                Menu {
                    id: menu
                    parent: root
                    actions: [
                        Action { text: qsTr("Group By Name"); onTriggered: model.groupBy(DocumentListModel.GroupByName) },
                        Action { text: qsTr("Group By Type"); onTriggered: model.groupBy(DocumentListModel.GroupByDocType) }
                    ]
                }
                onClicked: {
                    var pos = groupBy.mapToItem(root, groupBy.width/2, groupBy.height)
                    menu.show(pos.x - menu.contentWidth/2, pos.y)
                           }
            },
            ToolButton {
                id: aboutButton
                image: "image://icon/help-about";
                borderPosition: "left"
                onClicked: aboutDialog.opacity = 1;
            }
            ]
    }

    ListView {
        id: documentListView
        clip: true

        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom 

        model: DocumentListModel { }
        delegate: Rectangle {
            id: delegate
            
            width: ListView.view.width
            height: text.height + 10
            
            color: index % 2 ? "transparent" : "#eeeeee"

            Text {
                id: text
                text: model.fileName
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.selected(index, model.filePath)
            }
        }

        section.property: "sectionCategory"
        section.criteria: ViewSection.FullString
        section.delegate : Rectangle {
            width: parent.width
            height: text.height + 5
            color: "#dfbd9f"

            Text {
                id: text
                anchors.left: parent.left
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
                text: section
            }
        }
    }

    AboutDialog {
        id: aboutDialog
        icon: "image://icon/kspread" // ##: Show Calligra icon here
        title: "Calligra Mobile"
        opacity: 0
    }
}
