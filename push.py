import os
import sys
import time

# os.system("find . -name '*.exe' | xargs rm -vr")

message = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
message = " \"" + message + "\" "

repository = " git@github.com:Spirit599/xyLog.git "
branch = " master "

if len(sys.argv) >= 2:
	message = ""
	n = len(sys.argv)
	for i in range(1, n):
		message += sys.argv[i] + " "
	message = message[:-1]
	message = " \"" + message + "\" "


os.system("git add .")
os.system("git status")
os.system("git commit -m" + message)
os.system("git push" + repository + branch)

os.system("pause")
