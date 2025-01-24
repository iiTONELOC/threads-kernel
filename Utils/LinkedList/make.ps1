# Remove the existing /out directory if it exists
Remove-Item -Recurse -Force .\out

# Create a new /out directory
New-Item -ItemType Directory -Path .\out

# Compile LinkedList.c into an object file with warnings enabled
gcc -Wall -c .\LinkedList.c -o .\out\LinkedList.o

# Compile LinkedListsTest.c into an object file with warnings enabled
gcc -Wall -c .\LinkedListsTest.c -o .\out\LinkedListsTest.o

# Link the object files to create the final executable
gcc .\out\LinkedList.o .\out\LinkedListsTest.o -o .\out\LinkedListsTest.exe
