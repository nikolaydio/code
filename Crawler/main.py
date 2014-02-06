
import urllib.request
import re
import urllib.parse
import threading
import queue

urls_lock = threading.Lock()
urls = {}
#0 == processed
#1 == queued
url_queue = queue.Queue()
write_queue = queue.Queue(128)

from html.parser import HTMLParser
class CrawlerHTMLParser(HTMLParser):
    def handle_starttag(self, tag, attrs):
        #print("Encountered a start tag:", tag, attrs)
        if tag == "a":
        	for i in attrs:
        		if i[0] == "href":
        			self.crawler.CheckUrl(i[1])
    def handle_endtag(self, tag):
        #print("Encountered an end tag :", tag)
        pass
    def handle_data(self, data):
        #print("Encountered some data  :", data)
        pass
import sys


class CrawlerWorker():
	def CheckUrl(self, url):
		#check for relative urls, generate full ones
		p = re.compile("^[\-a-zA-Z0-9]+(\.[\-a-zA-Z0-9]+)+$")
		p2 = re.compile("")
		if len(url) > 0 and url[0] == "#":
			url = self.sub + self.base + "/" + url
		elif len(url) > 0 and url[0] == "/":
			url = self.sub + self.base + url
		elif p.match(url) != None:
			url = self.sub + self.base + "/" + url
		elif url.find(".") == -1:
			url = self.sub + self.base + "/" + url
		url = url.strip()
		#parse url
		res = urllib.parse.urlparse(url)

		p = re.compile("\..+$")
		extention = p.search(res[2])
		path = "/" + res[2]
		while path.find("//") != -1:
			path = path.replace("//", "/")

		url = res.geturl()

		if extention != None:
			extention = extention.group(0)
			if extention != ".htm" and extention != ".html":
				self.QueueUrl(url, False)
				return
		self.QueueUrl(url, True)

	def QueueUrl(self, url, process):
		global urls, urls_lock, url_queue, write_queue

		urls_lock.acquire()
		meta = urls.get(url, None)
		urls_lock.release()
		if meta == None:
			urls_lock.acquire()
			urls[url] = 1
			urls_lock.release()
			if process:
				url_queue.put(url)
			write_queue.put(url)

	def ProcessPage(self, url):

		self.parser = CrawlerHTMLParser(strict=False)
		self.parser.crawler = self

		res = urllib.parse.urlparse(url)
		self.sub = res[0] + "://"
		self.base = res[1]
		#get the page content
		print("Processing:", url)
		try:
			content = str(urllib.request.urlopen(url).read().decode("utf-8"))
		except UnicodeDecodeError:
			print("Failed to decode: ", url)
			return
		except UnicodeEncodeError:
			print("Failed to encode: ", url)
			return
		except urllib.error.HTTPError:
			print("Http error: ", url)
			return
		except urllib.error.URLError:
			print("Url error: ", url)
			return
		content = content.replace('\n', "")
		content = content.replace('\r', "")
		self.parser.feed(content)




def start_crawler():
	global url_queue, urls
	crawler = CrawlerWorker()
	#ProcessPage("file:///", "C:/Users/Nickie/Desktop/Workspace/Crawler/test", ".html")
	while True:
		url = url_queue.get()

		res = urllib.parse.urlparse(url)
		crawler.ProcessPage(res.geturl())
		urls_lock.acquire();
		urls[url] = 0
		urls_lock.release();

		url_queue.task_done()

def write_process():
	global write_queue
	while True:
		url = write_queue.get()
		f = open('output.txt','a')
		f.write(url)
		f.write('\n')
		f.close()
		write_queue.task_done()
def start():
	global url_queue
	url_queue.put("http://en.wikipedia.org/wiki/Main_Page")
	threads = []
	for i in range(16):
		t = threading.Thread(target=start_crawler)
		t.daemon = True
		t.start()
		threads.append(t)
	write_process()
if __name__ == "__main__":
	start()