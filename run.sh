SOURCE="./p2.c"
COMPILED="./a.out"

rm $COMPILED
echo "Compiling $SOURCE"
gcc $SOURCE -g

echo "Executing $COMPILED"
echo "--------------------"
# $COMPILED bookexample.txt 5 5 5
# valgrind --leak-check=full $COMPILED bookexample.txt 3 10 20
# valgrind $COMPILED bookexample.txt 4 4 4
# $COMPILED randomstream60.txt 4 15 60

# $COMPILED randomstream15.txt 4 15 15
# $COMPILED randomstream30.txt 4 30 30
# $COMPILED randomstream60.txt 4 60 60

# $COMPILED localitystream15.txt 4 15 15
# $COMPILED localitystream30.txt 4 30 30
# $COMPILED localitystream60.txt 4 60 60
