# Remove the existing /out directory for LinkedListArray if it exists
Remove-Item -Recurse -Force .\out

# Create a new /out directory for LinkedListArray
New-Item -ItemType Directory -Path .\out

# Compile LinkedListArray.c into an object file with warnings enabled
gcc -Wall -g  -I..\LinkedList -c LinkedListArray.c -o .\out\LinkedListArray.o

# Compile LinkedListsTest.c into an object file with warnings enabled
gcc -Wall -g -I..\LinkedList -c LinkedListArrayTests.c -o .\out\LinkedListArrayTests.o

# Compile LinkedList.c from the LinkedList folder 
gcc -Wall -g -I..\LinkedList -c ..\LinkedList\LinkedList.c -o .\out\LinkedList.o

# Link the object files (including the LinkedList.o) to create the final executable
gcc -g .\out\LinkedListArray.o .\out\LinkedListArrayTests.o .\out\LinkedList.o -o .\out\LinkedListArrayTests.exe
