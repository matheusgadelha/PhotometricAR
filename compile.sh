
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
#RENDERING LIBRARY CREATION
#-------------------------------------------------------------------------------
cd rendering
make

if [ "$?" = "0" ]; then
	echo "\n\t***INFO: Rendering library successfully created at ./lib/librendering.a\n"
else
	echo "\n\tERROR: Creating rendering library\n"
fi

make clean
cd ..

#-------------------------------------------------------------------------------
#TRACKING & RENDERING TEST PROGRAM CREATION
#-------------------------------------------------------------------------------
cd tst
make

if [ "$?" = "0" ]; then
	echo "\n\t***INFO: Test programs for tracking & rendering succesfully created at ./bin/tst-track\n"
else
	echo "\n\tERROR: Creating test programs\n"
fi

make clean
cd ..

#-------------------------------------------------------------------------------
#END OF COMPILING SCRIPT
#-------------------------------------------------------------------------------
echo "\n\t***FINISHED\n"
