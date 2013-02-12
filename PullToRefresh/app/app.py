
import tart

class App(tart.Application):
    def onUiReady(self):
        pass

    def onGetGlobalStream(self):
        from adn.adn import Adn
        app = Adn()
        self.stream = app.globalStream()
        validStream = True
        for item in self.stream['data']:
            if 'html' in item:
                item['html'] = item['html'].replace("<br>", "<br/>")
                validStream = True
            else:
                validStream = False
        if validStream:
            tart.send('receivedGlobalStream', stream=self.stream)
            import tempfile
            temp_dir = tempfile.gettempdir()
            import os
            for the_file in os.listdir(temp_dir):
                filepath = os.path.join(temp_dir, the_file)
                try: 
                    if os.path.isfile(filepath):
                        os.unlink(filepath)
                except Exception as e:
                    print(e)
            self.getAvatarImages()
        else:
            self.onGetGlobalStream()

    def getAvatarImages(self):
        from .multirequest import MultiRequest
        multiRequester = MultiRequest(self.mapGetImage, 5)
        requests = list()
        for (index,item) in enumerate(self.stream['data']):
            if 'user' in item and 'avatar_image' in item['user']:
                requests.append(item['user']['avatar_image']['url'])
        multiRequester.get(requests)

    def mapGetImage(self, index, url):
        import requests
        import tempfile
        r = requests.get(url)
        f = tempfile.NamedTemporaryFile(delete=False)
        try:
            f.write(r.content)
            filename = f.name
            avatar_image = dict()
            avatar_image['url'] = filename
            avatar_image['index'] = index
            tart.send('updateAvatarImage', avatar_image=avatar_image)
        finally:
            f.close()


