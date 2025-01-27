# Remove the existing /out directory if it exists
Remove-Item -Recurse -Force .\out

# Create a new /out directory
New-Item -ItemType Directory -Path .\out

# Compile LinkedList.c into an object file with warnings enabled, adding the Include directory
gcc -Wall -c .\StringUtils.c -I ../../Include -o .\out\StringUtils.o

