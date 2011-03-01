import Qt 4.7
import org.calligra.mobile 1.0

ViewStack
{
    id: root;
    width: 1024;
    height: 800;

    OpenFileView {
        id: view;

        onOpenFileDialog: {
            var od = Qt.createQmlObject("import org.calligra.mobile 1.0; OpenFileDialog { }", root, "dynamic");
            od.filter = "*.odt *.doc *.ods *.xls *.odp *.ppt";
            root.push(od);
        }
        onOpenFile: {
            root.openFile(view.file);
        }
    }

   function openFile(file) {
        var viewmap = { 
            'odt': 'WordsView.qml', 'doc': 'WordsView.qml',
            'ods': 'TableView.qml', 'xls': 'TableView.qml',
            'odp': 'StageView.qml', 'ppt': 'StageView.qml'
        }
 
        var ext = file.substr(-3)
        if (!viewmap.hasOwnProperty(ext)) {
            console.log('Unsupported document type')
            return
        }
        var comp = Qt.createComponent(viewmap[ext])
        if(comp.status == Component.Ready) {
            var tv = comp.createObject(null);
            tv.file = file;
            view.recentFiles.addFile(tv.file);
            root.popAll();
            root.push(tv);
        } else {
            console.log("Error creating view instance");
            console.log(comp.errorString());
        }
    }
}

