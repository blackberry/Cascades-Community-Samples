import bb.cascades 1.2

Page {
    Container {
        layout: DockLayout {
        }

        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Top

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                text: qsTr("Choose theme from the dropdown list") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.TitleText
            }

            DropDown {
                id: themeDropDown
                title: qsTr("Theme") + Retranslate.onLocaleOrLanguageChanged

                Option {
                    text: qsTr("Bright") + Retranslate.onLocaleOrLanguageChanged
                    value: VisualStyle.Bright
                }

                Option {
                    text: qsTr("Dark") + Retranslate.onLocaleOrLanguageChanged
                    value: VisualStyle.Dark
                }
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center
                multiline: true
                visible: Application.themeSupport.theme.colorTheme.style != themeDropDown.selectedValue
                text: qsTr("Click on Save button and restart the application\nto apply theme") + Retranslate.onLocaleOrLanguageChanged
                textStyle.color: Color.Red
            }
        }

        Button {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
            text: "Save theme" + Retranslate.onLocaleOrLanguageChanged

            onClicked: {
                // use "theme" key for property showing what theme to use on application start
                Settings.saveValueFor("theme", VisualStyle.Bright == themeDropDown.selectedValue ? "bright" : "dark");
            }
        }
    }

    onCreationCompleted: {
        var theme = Settings.getValueFor("theme", VisualStyle.Bright == themeDropDown.selectedValue ? "bright" : "dark");
        themeDropDown.setSelectedIndex("bright" == theme ? 0 : 1);
    }
}