import Qt 4.7
import org.calligra.mobile 1.0

Rectangle {
    id: root
    width: 1024
    height: 800

    state: "showingDocumentPicker"

    DocumentPicker {
        id: documentPicker
        anchors.fill: parent

        onSelected: {
            model.addRecent(index)
            viewLoader.openFile(filePath) 
            root.state = "showingDocumentViewer"
        }
    }
    
    ViewLoader {
        id: viewLoader
        anchors.fill: parent

        onViewingFinished: root.state = "showingDocumentPicker"
    }

    states: [
        State {
            name: "showingDocumentPicker"
            PropertyChanges {
                target: documentPicker
                opacity: 1
            }
            PropertyChanges {
                target: viewLoader
                scale: 0
            }
            StateChangeScript {
                name: "unloadViewer"
                script: if (viewLoader.sourceComponent) viewLoader.sourceComponent = undefined
            }
        },
        State {
            name: "showingDocumentViewer"
            PropertyChanges {
                target: documentPicker
                opacity: 0
            }
            PropertyChanges {
                target: viewLoader
                scale: 1
            }
        }
            ]

    transitions: [
        Transition {
            to: "showingDocumentPicker"
            SequentialAnimation {
                PropertyAction { target: documentPicker; property: "opacity" }
                NumberAnimation { target: viewLoader; property: "scale"; duration: 400 }
                ScriptAction { scriptName: "unloaderViewer" }
            }
        },
        Transition {
            to: "showingDocumentViewer"
            SequentialAnimation {
                NumberAnimation { target: viewLoader; property: "scale"; duration: 400 }
                PropertyAction { target: documentPicker; property: "opacity"  }
            }
        }
            ]
}

