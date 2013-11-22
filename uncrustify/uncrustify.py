import os
import re
import sys

# Record the current directory.
curdir = os.getcwd()
uncrustifycommand = curdir + os.sep + 'uncrustify.exe -c ' + curdir + os.sep + 'uncrustify.cfg --replace '

isAll = False
# Parse arguments.
for arg in sys.argv :
    if arg == '-all' :
        isAll = True
        break

if not isAll :
    # Only run uncrustify on the changed files, use svn to get the changed file list.
    os.chdir(os.pardir)
    os.chdir(os.pardir)
    os.chdir(os.pardir)
    isGit = False
    if os.path.exists('.svn') :
        os.system('svn status -q > filelist.txt')
    else : # should be a git-svn depot
        isGit = False
        os.system('svn status -q > filelist.txt')

    # If file exists, open and read it to memory.
    if os.path.isfile('filelist.txt') :
        filelist = open('filelist.txt', 'r')
        filetext = filelist.read()
        if isGit :
            filetext = filetext.replace('/', '\\')
        filelist.close()
        os.system('del -q filelist.txt')
    else :
        isAll = True

    os.chdir(curdir)

# The loopdir function to run uncrustify.
def loopdir(startdir, startreldir) :
    # Go to the start directory.
    os.chdir(startdir)
    
    # Loop the subfolders & files.
    for item in os.listdir(os.curdir) :
        if os.path.isdir(item) :
            # Skip the .svn folders.
            resdir = re.match('.svn$', item)
            if resdir is None:
                # Recursively call.
                loopdir(item, os.path.join(startreldir, item))
                # Back to the parent directory.
                os.chdir(os.pardir)
        else :
            # Check if the filename ends with .h, .hpp or .cpp.
            if item == 'resource.h':
                print(os.path.join(os.curdir,item))
                print('Ignore resource.h whose content would be deleted by uncrustify!')
            else:
                resfile = re.search('\.h$|\.hpp$|\.cpp$', item)
                if resfile is not None :
                    # Run uncrustify with replacing the existing files directly.
                    index = 0
                    if not isAll :
                        relname = os.path.join(startreldir, item)
                        index = filetext.find(relname)
                    if index > -1 :
                        print( os.path.join(os.getcwd(), item) )
                        unccommand = uncrustifycommand + '"' + item + '"'
                        os.system(unccommand)
                        # Delete the temporary files generated by uncrustify.
                        os.system('del -q "' + item + '.unc-backup"*')

# Call loopdir function to loop the given directory recursively.
loopdir( os.path.join("../", ''), 'BloodRain3D')
os.chdir(curdir)