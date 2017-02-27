
import threading
import queue


class MultiRequest:
    DEFAULT_NUM_THREADS = 4

    def __init__(self, handler, num_threads=DEFAULT_NUM_THREADS):
        self.num_threads = num_threads
        self.threads = []
        self.queue = queue.Queue()
        self.handler = handler


    def get(self, requests):
        if not self.threads:
            for i in range(self.num_threads):
                t = threading.Thread(target=self.run)
                t.daemon = True
                t.start()
                self.threads.append(t)

        for (index,r) in enumerate(requests):
            self.queue.put((index,r))


    def run(self):
        '''called in a thread to process requests by calling the handler'''
        while 1:
            (index,url) = self.queue.get()
            self.handler(index, url)
