all:
	g++ -std=c++11 *.cpp -o assignment1 -g -O3

run: all
	./assignment1 start1.txt goal1.txt bfs bfs1.txt 
	./assignment1 start1.txt goal1.txt dfs dfs1.txt 
	./assignment1 start1.txt goal1.txt iddfs iddfs1.txt 
	./assignment1 start1.txt goal1.txt astar astar1.txt 

	./assignment1 start2.txt goal2.txt bfs bfs2.txt 
	./assignment1 start2.txt goal2.txt dfs dfs2.txt 
	./assignment1 start2.txt goal2.txt iddfs iddfs2.txt 
	./assignment1 start2.txt goal2.txt astar astar2.txt 

	./assignment1 start3.txt goal3.txt bfs bfs3.txt 
	./assignment1 start3.txt goal3.txt dfs dfs3.txt 
	./assignment1 start3.txt goal3.txt astar astar3.txt 

zip:
	zip ./assignment1.zip *.cpp *.h goal*.txt start*.txt Makefile ./*.pdf
