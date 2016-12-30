all: sudoku gen
sudoku: sudoku.cc wrapper.cc sudoku.h
	c++ sudoku.cc wrapper.cc -o sudoku -std=c++11
gen: sudoku.cc gen_wrapper.cc sudoku.h rander.h
	c++ sudoku.cc gen_wrapper.cc -o gen -std=c++11
