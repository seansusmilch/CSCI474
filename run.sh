SOURCE="./p2.c"
COMPILED="./a.out"

rm $COMPILED
echo "Compiling $SOURCE"
gcc $SOURCE -g

echo "Executing $COMPILED"
echo "--------------------"
$COMPILED bookexample.txt 4 15 20
# valgrind --leak-check=full $COMPILED bookexample.txt 3 10 20
# valgrind $COMPILED bookexample.txt 4 15 20