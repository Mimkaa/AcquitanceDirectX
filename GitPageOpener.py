import os
with open('PageIAmOn.txt') as f:
    first_line = int(f.readline())
page = first_line
os.system(f"start \"\" https://github.com/planetchili/hw3d/commits/master?after=b8ea00f7f236779fc4b8302f6ff73511f3c668fd+{34*page}")