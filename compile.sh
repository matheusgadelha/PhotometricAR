
#-------------------------------------------------------------------------------
#TRACKING LIBRARY CREATION
#-------------------------------------------------------------------------------
cd tracking
make

if [ "$?" = "0" ]; then
	echo "\n\t***INFO: Tracking library successfully created at ./lib/libtracking.a\n"
else
	echo "\n\tERROR: Creating tracking library\n"
fi

make clean
cd ..


#-------------------------------------------------------------------------------
#TRACKING TEST PROGRAM CREATION
#-------------------------------------------------------------------------------
cd tst
make

if [ "$?" = "0" ]; then
	echo "\n\t***INFO: Test program for tracking succesfully created at ./bin/tst-track\n"
else
	echo "\n\tERROR: Creating tracking test program\n"
fi

make clean
cd ..

#-------------------------------------------------------------------------------
#END OF COMPILING SCRIPT
#-------------------------------------------------------------------------------
echo "\n\t***FINISHED\n"
