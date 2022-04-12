SOURCE="./p2.c"
COMPILED="./a.out"

rm $COMPILED
echo "Compiling $SOURCE"
gcc $SOURCE

echo "Executing $COMPILED"
echo "--------------------"
$COMPILED bookexample.txt 4 14 20