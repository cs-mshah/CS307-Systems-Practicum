routine: myshell diningPhil matrixMul

myshell: Q1/myshell.cpp
		g++ -o myshell Q1/myshell.cpp

diningPhil: Q2/DiningStudents.cpp
		g++ -o DiningStudents Q2/DiningStudents.cpp -pthread

matrixMul: Q3/*
		g++ -o sequential Q3/sequential.cpp -lpthread
		g++ -o multithread1 Q3/multithread1.cpp -lpthread
		g++ -o multithread2 Q3/multithread2.cpp -lpthread
		g++ -o multithread3 Q3/multithread3.cpp -lpthread