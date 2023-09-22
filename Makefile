#
# PROGRAM:    Project 2
# PROGRAMMER: Alex Bertolasi
# LOGON ID:   z1915589
# DATE DUE:   09/27/2023
#

# Compiler variables
GXX = g++
CXXFLAGS = -Wall -Werror -std=c++11

# Rule to link object code files to create executable file
z1915589_project2: z1915589_project2.o
	$(CXX) $(CXXFLAGS) -o z1915589_project2 z1915589_project2.o

# Rules to compile source code files to object code
z1915589_project2.o: z1915589_project2.cc

# Pseudo-target to remove object code and executable files
clean:
	-rm *.o z1915589_project2