rk2:
	gcc -g -Wall -Wextra main.c file.c csv.c json.c convert.c -o build/rk2
	
rk3:
	gcc -g -Wall -Wextra rk3.c csv.c json.c file.c -o build/rk3

