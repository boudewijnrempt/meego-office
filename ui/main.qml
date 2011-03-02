import Qt 4.7
import org.calligra.mobile 1.0

Rectangle {
    id: root
    width: 1024
    height: 800

    state: "showingDocumentPicker"

    DocumentPicker {
        id: documentPicker
        width: root.width
        height: root.height

        onSelected: {
            model.addRecent(index)
            viewLoader.openFile(filePath) 
            root.state = "showingDocumentViewer"
        }
    }
    
    ViewLoader {
        id: viewLoader
        width: root.width
        height: root.height

        onViewingFinished: root.state = "showingDocumentPicker"
    }

    states: [
        State {
            name: "showingDocumentPicker"
            PropertyChanges {
                target: documentPicker
                x: 0
            }
            PropertyChanges {
                target: viewLoader
                x: root.width
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
                x: -root.width
            }
            PropertyChanges {
                target: viewLoader
                x: 0
            }
        }
            ]

    transitions: [
        Transition {
            to: "showingDocumentPicker"
            SequentialAnimation {
                NumberAnimation { property: "x"; duration: 400 }
                ScriptAction { scriptName: "unloaderViewer" }
            }
        },
        Transition {
            to: "showingDocumentViewer"
            SequentialAnimation {
                NumberAnimation { property: "x"; duration: 400 }
            }
        }
            ]
}

