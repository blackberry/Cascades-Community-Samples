import bb.cascades 1.2

Sheet {
    id: propertyEditSheet

    function onEditChanged(val) {
        title.title = val ? "Edit" : "Add";
        textFieldName.visible = ! val;
        labelName.visible = val;
    }

    function onScopeChanged(val) {
        scopeApplication.selected = scopeApplication.value == val;
        scopeVendor.selected = scopeVendor.value == val;
    }

    function onNameChanged(val) {
        textFieldName.text = val;
        labelName.text = val;
    }

    function onValueChanged(val) {
        value.text = val;
    }

    onCreationCompleted: {
        app.propertyIsEditChanged.connect(onEditChanged);
        onEditChanged(app.propertyIsEdit);

        app.propertyScopeChanged.connect(onScopeChanged);
        onScopeChanged(app.propertyScope);

        app.propertyNameChanged.connect(onNameChanged);
        onNameChanged(app.propertyName);

        app.propertyValueChanged.connect(onValueChanged);
        onValueChanged(app.propertyValue);

    }

    Page {
        titleBar: TitleBar {
            id: title
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    propertyEditSheet.close();
                }
            }
            acceptAction: ActionItem {
                title: "Save"
                onTriggered: {
                    if (app.saveProperty(scope.selectedValue, textFieldName.text, value.text)) {
                        propertyEditSheet.close();
                    }
                }
            }
            title: "TBA"
        }
        Container {
            DropDown {
                id: scope
                title: "Scope"
                options: [
                    Option {
                        id: scopeVendor
                        selected: app.propertyScope == "vendor"
                        text: "Vendor"
                        value: "vendor"
                    },
                    Option {
                        id: scopeApplication
                        text: "Application"
                        value: "application"
                    }
                ]
            }
            Label {
                id: labelName
                text: "Label Name"
                visible: app.isPropertyEdit
            }
            TextField {
                id: textFieldName
                text: "Text Field Name"
                hintText: "Name"
                visible: ! app.isPropertyEdit
            }
            TextField {
                id: value
                text: app.propertyValue
                hintText: "Value"
            }
        }
    }
}