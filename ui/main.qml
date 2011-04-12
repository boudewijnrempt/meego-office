import QtQuick 1.0
import org.calligra.mobile 1.0
import MeeGo.Components 0.1

Window {
    id: window
    signal documentOpened(variant newView)

    bookMenuModel: [ qsTr("All Documents"), qsTr("Presentations"), qsTr("Spreadsheets"), qsTr("Text Documents") ];
    bookMenuPayload: [ allDocumentsPageComponent, presentationsPageComponent, spreadsheetsPageComponent, textDocumentsPageComponent ];
    
    Component { id: allDocumentsPageComponent; DocumentsPage { } }
    Component { id: presentationsPageComponent; DocumentsPage { pageTitle: qsTr("Presentations"); filter: DocumentListModel.Presentations; } }
    Component { id: spreadsheetsPageComponent; DocumentsPage { pageTitle: qsTr("Spreadsheets"); filter: DocumentListModel.Spreadsheets; } }
    Component { id: textDocumentsPageComponent; DocumentsPage { pageTitle: qsTr("Text Documents"); filter: DocumentListModel.TextDocuments; } }

    Component.onCompleted: { switchBook(allDocumentsPageComponent); }

    Settings {
        id: settings;
    }

//     state: "showingDocumentPicker"

//     function open(filePath) {
//         documentPicker.model.addRecent(filePath)
//         var returned = viewLoader.setFile(filePath)
//         if(returned === false) {
//             root.state = "showingDocumentPicker"
//             // We should probably have another state here, showing some kind of error message
//             // about the file attempted opened not being an accepted format or something...
//             return
//         }
//         showingDocumentViewerTransition.enabled = false
//         root.state = "showingDocumentViewer"
//         showingDocumentViewerTransition.enabled = true
//         root.documentOpened(viewLoader.controller)
//     }

//     ScreenOrientation {
//         target: root
//     }

//     DocumentPicker {
//         id: documentPicker
// //         width: root.width
// //         height: root.height
// 
//         onSelected: {
//             model.addRecent(index)
//             viewLoader.setFile(filePath)
//             root.documentOpened(viewLoader.controller)
//             root.state = "showingDocumentViewer"
//         }
//     }
//     
//     ViewLoader {
//         id: viewLoader
//         width: root.width
//         height: root.height
// 
//         onViewingFinished: {
//             root.state = "showingDocumentPicker"
//         }
//     }
// 
//     states: [
//         State {
//             name: "showingDocumentPicker"
//             PropertyChanges {
//                 target: documentPicker
//                 x: 0
//             }
//             PropertyChanges {
//                 target: viewLoader
//                 x: root.width
//             }
//             StateChangeScript {
//                 name: "unloadViewer"
//                 script: viewLoader.unloadDocument()
//             }
//         },
//         State {
//             name: "showingDocumentViewer"
//             PropertyChanges {
//                 target: documentPicker
//                 x: -root.width
//             }
//             PropertyChanges {
//                 target: viewLoader
//                 x: 0
//             }
//             StateChangeScript {
//                 name: "loadDocument";
//                 script: viewLoader.loadDocument();
//             }
//         }
//     ]
// 
//     transitions: [
//         Transition {
//             to: "showingDocumentPicker"
//             SequentialAnimation {
//                 NumberAnimation { property: "x"; duration: 400 }
//                 ScriptAction { scriptName: "unloaderViewer" }
//             }
//         },
//         Transition {
//             id: showingDocumentViewerTransition
//             property bool enabled: true // QTBUG-14488
//             to: enabled ? "showingDocumentViewer" : ""
//             SequentialAnimation {
//                 NumberAnimation { property: "x"; duration: 400 }
//                 PauseAnimation { duration: 100; }
//                 ScriptAction { scriptName: "loadDocument"; }
//             }
//         }
//     ]
}

