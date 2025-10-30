run-fox-machines:
	mpicc -o fox main.c fox.c matrix.c -lm && mpirun -np 4 --use-hwthread-cpus --hostfile hostfile.txt --display-map ./fox <input >output.txt

run-fox-one-machine:
	mpicc -o fox main.c fox.c matrix.c -lm && mpirun -np 4 --display-map ./fox <data/input300 >output.txt

run-seq:
	gcc -o seq main_seq.c matrix.c && ./seq <input >output.txt