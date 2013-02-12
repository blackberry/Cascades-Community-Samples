import bb.cascades 1.0
import "../tart.js" as Tart

NavigationPane {
    id: root
    signal addItem(variant entry)

    Menu.definition: AppMenuDefinition {
        id: appMenu
    }

    MainPage { id: mainPage }

    attachedObjects: [
        SettingsPage { id: settingsPage },
        HelpPage { id: helpPage }
    ]

    onCreationCompleted: {
        Tart.init(_tart, Application);

        Tart.register(root);

        Tart.send('uiReady');

        appMenu.triggerSettingsPage.connect(handleTriggerSettingsPage);
        appMenu.triggerHelpPage.connect(handleTriggerHelpPage);
        root.addItem.connect(mainPage.addItem);
        Tart.send('getGlobalStream');
    }

    function handleTriggerSettingsPage() {
        push(settingsPage);
    }

    function handleTriggerHelpPage() {
        push(helpPage);
    }

    function onReceivedGlobalStream(stream) {
        stream.stream.data.forEach(function(entry) {
            root.addItem(entry);
        });
    }

    function onUpdateAvatarImage(avatar_image) {
        var filename = avatar_image.avatar_image.url;
        var index = avatar_image.avatar_image.index;
        mainPage.updateAvatarImage(index, filename);
    }
}
