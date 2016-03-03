#
# The purpose of this script is to facilitate the simulation of VSS "Shared Files" or "Links"
# by means of the svn switch command in conjunction with svn properties.
#
# This script scans a directory tree in a breadth-first order and switches each file/folder
# that has a property with the name specified by the PROPERTY_OBJET_SVN_LINK variable
# to the URL specified by the property.
# The property specifies a relative URL.
# For example, if the value of the property of the file at     svn://host/repo/trunk/project/test.cpp     is      "../common/tests"
# then the file will be switched to svn://host/repo/trunk/common/tests/test.cpp
# Notice that it is not a standard way to treat relative URLs
# When the script swithes a directory, the tree rooted a the directory before the swith, is not scanned.
#
# See also svnLinksContentDestroyer.py for automatically creating messages that tell user about files/folders that should be switched.

import pysvn
import os

PROPERTY_OBJET_SVN_LINK = "objet:link"
SVN_META_DATA_DIR = ".svn"

def getTargetURL(client, path):
	try:
		props = client.propget(PROPERTY_OBJET_SVN_LINK, path)
	except pysvn.ClientError:
		return ""
	if 1 != len(props):
		return ""
	parentdir = os.path.split(path)[0]	
	entry = client.info(parentdir)	
	url = entry.url	
	unixpath = path.replace('\\','/')	
	relativeURL = props.items()[0][1]	
	while "../" == relativeURL[:3]:
		url = os.path.split(url)[0]
		relativeURL = relativeURL[3:]
	url += "/" + relativeURL + "/" + os.path.split(unixpath)[1]
	return url



def switchAllObjetSvnLinks(client, path):
	for root, dirs, files in os.walk(path):
		print "Scanning %s" % root
		dirs[:] = [dir for dir in dirs if dir != client.get_adm_dir()]
		# TODO: skip non-WC dirs

		for dir in dirs:
			try:
				dirpath = os.path.join(root, dir)
				url = getTargetURL(client, dirpath)
				#change each space to be %20, to work with more then one word in a name
				url = url.replace(" ", "%20")
				if "" != url:
					print "Switching %s to %s" % (dirpath, url)
					client.switch(dirpath, url)
					dirs.remove(dir)
			except pysvn.ClientError, e:
				# convert to a string
				print str(e)
		
		for file in files:
			try:
				filepath = os.path.join(root, file)
				url = getTargetURL(client, filepath)
				#change each space to be %20, to work with more then one word in a name
				url = url.replace(" ", "%20")
				
				if "" != url:
					print "Switching %s to %s" % (filepath, url)
					client.switch(filepath, url)
			except pysvn.ClientError, e:
				# convert to a string
				print str(e)

try:
	path = raw_input('Enter path to process. Default: %s\n' % os.getcwd())
	if "" == path:
		path = os.getcwd()
	client = pysvn.Client()
	client.set_adm_dir(SVN_META_DATA_DIR)
	switchAllObjetSvnLinks(client, path)
except Exception, e:
	print "Exception: %s" % str(e)
except Error, e: # TODO: is there such thing ?
	print "Error: %s" % str(e)

dummy = raw_input("Enter to exit...")