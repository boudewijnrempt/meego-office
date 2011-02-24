import QtQuick 1.0
import org.calligra.mobile 1.0

ViewStack
{
    id: root;
    width: 1024;
    height: 600;

    property string file: "";

    OpenFileView {
        id: view;

        onOpenFileDialog: {
            var od = Qt.createQmlObject("import org.calligra.mobile 1.0; OpenFileDialog { }", root, "dynamic");
            od.filter = "*.odt *.doc";
            root.push(od);
        }
        onOpenFile: {
            root.openFile(view.file);
        }
    }

    function openFile(file) {
        var comp = Qt.createComponent("WordsView.qml");
        if(comp.status == Component.Ready) {
            var tv = comp.createObject(null);
            if(file) {
                tv.file = file;
            } else {
                tv.file = root.file;
            }
            view.recentFiles.addFile(tv.file);
            root.popAll();
            root.push(tv);
        } else {
            console.log("Error creating table view instance");
            console.log(comp.errorString());
        }
    }
}

