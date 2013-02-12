import bb.cascades 1.0
import "../tart.js" as Tart

MenuDefinition {
    signal triggerSettingsPage
    signal triggerHelpPage

    settingsAction: SettingsActionItem {
        onTriggered: {
            triggerSettingsPage();
        }
    }
    helpAction: HelpActionItem {
        onTriggered: {
            triggerHelpPage();
        }
    }
}
