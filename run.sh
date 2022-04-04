SOURCE="./p1.c"
COMPILED="./a.out"

rm $COMPILED
echo "Compiling $SOURCE"
gcc $SOURCE

echo "Executing $COMPILED"
echo "--------------------"
$COMPILED with some args