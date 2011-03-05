import Qt 4.7
import org.calligra.mobile 1.0

Rectangle {
    id: root
    width: 1024
    height: 800

    state: "showingDocumentPicker"

    function open(filePath) {
        documentPicker.model.addRecent(filePath)
        viewLoader.setFile(filePath)
        showingDocumentViewerTransition.enabled = false
        root.state = "showingDocumentViewer"
        showingDocumentViewerTransition.enabled = true
    }

    DocumentPicker {
        id: documentPicker
        width: root.width
        height: root.height

        onSelected: {
            model.addRecent(index)
            viewLoader.setFile(filePath)
            root.state = "showingDocumentViewer"
        }
    }
    
    ViewLoader {
        id: viewLoader
        width: root.width
        height: root.height

        onViewingFinished: {
            root.state = "showingDocumentPicker"
        }
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
            StateChangeScript {
                name: "loadDocument";
                script: viewLoader.loadDocument();
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
            id: showingDocumentViewerTransition
            property bool enabled: true // QTBUG-14488
            to: enabled ? "showingDocumentViewer" : ""
            SequentialAnimation {
                NumberAnimation { property: "x"; duration: 400 }
                PauseAnimation { duration: 100; }
                ScriptAction { scriptName: "loadDocument"; }
            }
        }
    ]
}

