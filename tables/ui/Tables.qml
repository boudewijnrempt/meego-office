import Qt 4.7
import org.calligra.mobile 1.0

//ViewStack
Item
{
    id: root;

    property string file: "";
    
    width: 1024;
    height: 600;
    
    OpenFileView {
        id: view;
	anchors.fill: parent;
        
        onOpenFileDialog: {
            fileDialog.visible = true;
        }
        onOpenFile: {
           root.openFile(view.file);
        }
    }

    OpenFileDialog {
	id: fileDialog;
        anchors.fill: parent;
	visible: false;
        filter: "*.ods *.xls"
    }

    TableView {
        id: tableView;
        anchors.fill: parent;
        visible: false;
    }

    AboutDialog {
        id: aboutDialog;
        icon: "image://icon/kspread";
        title: "Calligra Tables";
        opacity: 0;
    }

    function openFile(file) {
        fileDialog.visible = false;
        tableView.visible = true;

        if(file) {
            tableView.file = file;
        } else {
            tableView.file = root.file;
        }
        view.recentFiles.addFile(tableView.file);
    }

    function pop() {
        tableView.visible = false;
        fileDialog.visible = false;
    }
}

