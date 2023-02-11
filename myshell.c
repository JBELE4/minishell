/*	AUTORES:
	SERGIO REY MORALES
	JAVIER BURGOS CHAMORRO	*/
//gcc myshell.c simplelinkedlist.c TElemento.c libparser_64.a -no-pie -o myshell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "parser.h"
#include "simplelinkedlist.h"


void SignalHandler();

int main(){
	signal(SIGINT, SignalHandler); //Tratamos SIGINT
	signal(SIGQUIT, SignalHandler); //Tratamos SIGQUIT
	
    char buf[1024];
    tline* line;
	int i, j;
	int pid;

	int fd; //Descriptor para abrir el fichero para la redirección de salida
	int fd1; //Descriptor para abrir el fichero para la redirección de entrada
	int fd2; //Descriptor para abrir el fichero para la redirección de error


	int ent, sal, err;
	ent = dup(0);
	sal = dup(1);
	err = dup(2);

	TLinkedList background; //Lista para los elementos del background
	crearVacia(&background); //Creamos vacía la lista background
	
    printf("msh> ");

    while(fgets(buf, 1024, stdin)){
		line = tokenize(buf);
		if(line==NULL)	continue;
		
		//Redirección de entrada
		if(line->redirect_input != NULL){
            printf("redirección de entrada: %s\n", line->redirect_input);
			fd1 = open(line->redirect_input, O_RDONLY);
			if(fd1<0){
				printf("%s: Error. %s\n",line->redirect_input, strerror(errno));
				close(fd1);
				continue;
			}else{
				dup2(fd1, 0);
			}
		}

		//Redirección de salida
		if(line->redirect_output != NULL){
			printf("redirección de salida: %s\n", line->redirect_output);
            fd = open(line->redirect_output, O_WRONLY | O_CREAT | O_TRUNC, 0766); //el propietario del fichero tiene todos los permisos
			if(fd<0){
				printf("%s: Error. %s\n",line->redirect_input, strerror(errno));
				close(fd);
				continue;
			}else{
				dup2(fd, 1);
			}
		}

		//Redirección de errores
		if(line->redirect_error != NULL){
			printf("redirección de error: %s\n", line->redirect_error);
            fd2 = open(line->redirect_error, O_WRONLY | O_CREAT | O_TRUNC, 0766);//el propietario del fichero tiene todos los permisos
			if(fd2<0){
				printf("%s: Error. %s\n",line->redirect_input, strerror(errno));
				close(fd2);
				continue;
			}else{
				dup2(fd2, 2);
			}
		}

		
		if(line->ncommands == 1){ //Un mandato
			if(strcmp(line->commands[0].argv[0], "cd") == 0){ //Mandato cd
				char *dir;
				if(line->commands[0].argv[1]==NULL){
					dir = getenv("HOME");
				}else{
					dir = line->commands[0].argv[1];
				}
				int check = chdir(dir);
				if(check != 0){
					printf("No se encuentra el directorio: %s\n", dir);
				}
			}else if(strcmp(line->commands[0].argv[0], "jobs") == 0){ //Mandato jobs
				mostrar(background);
			}else if(strcmp(line->commands[0].argv[0], "fg") == 0){ //Mandato fg
				if(line->commands[0].argv[1]==NULL){
					if(esVacia(background)){
						fprintf(stderr, "%s: actual: No se encuentra ese job\n", line->commands[0].argv[0]);
					}else{
						TElemento aux;
						ultimo(background, &aux);
						printComando(aux);
						int pidAux = devolverPid(aux);
						int idAux = devolverId(aux);
						eliminar(idAux, &background); //trabajo actual
						waitpid(pidAux, NULL, 0);
					}
				}else{
					int id = atoi(line->commands[0].argv[1]);
					if(contiene(id, background)){
						TElemento aux = capturar(id, background);
						printComando(aux);
						int pidAux = devolverPid(aux);
						eliminar(id, &background);
						waitpid(pidAux, NULL, 0);
					}else{
						fprintf(stderr, "%s: %s: No se encuentra ese job\n", line->commands[0].argv[0], line->commands[0].argv[1]);
					}
				}
		 	}else{
				pid = fork();
				if(pid < 0){
					fprintf(stderr, "Fallo en el fork\n");
					exit(-1);
				}else if(pid==0){
					execv(line->commands[0].filename,line->commands[0].argv);
					fprintf(stderr, "%s: No se encuentra el mandato\n", line->commands[0].argv[0]);
					exit(-1);
				}else if(line->background){ 
					dup2(sal, 1);
					TElemento aux;
					crear((longitud(background)+1), pid, buf, &aux);
					insertarFinal(aux, &background);
					print(aux);
				}else{
					waitpid(pid, NULL, 0);
				}
			}

		}else if(line->ncommands >= 2){ //Varios mandatos

			//Creamos un proceso intermedio
			int pidIntermedio = fork();

			if(pidIntermedio==0){

				//Reservamos espacio para los descriptores que se necesitan para los pipes
				int **pipesDes = (int **) malloc ((line->ncommands-1) * sizeof(int *));

				//Creamos los pipes necesarios
				for(i=0; i<line->ncommands-1; i++){
					pipesDes[i] = (int *) malloc (2*sizeof(int)); //Reservamos espacio para un descriptor de entrada y otro de salida para cada pipe
					if(pipe(pipesDes[i]) < 0){
						fprintf(stderr, "Fallo al crear el pipe: %s\n", strerror(errno));
					}
				}

				//Reservamos espacio para almacenar los pid de los procesos hijo
				int *childPids = (int *) malloc (line->ncommands * sizeof(int));
				//Creamos un proceso hijo por cada mandato
				for(i=0; i<line->ncommands; i++){
					pid = fork();
					if(pid < 0){
						fprintf(stderr, "Fallo en el fork\n");
						exit(-1);
					}else if(pid==0){
						if(i==0){ //Primer mandato
							for(j=1; j<line->ncommands-1; j++){ //Cerramos los descriptores que no vamos a usar 
								close(pipesDes[j][1]); //Descriptor de entrada
								close(pipesDes[j][0]); //Descriptor de salida
							}
							close(pipesDes[0][0]);
							dup2(pipesDes[0][1], 1); //Redireccionamos la salida estándar de este proceso al pipe
						}

						//Mandatos intermedios
						if(i>0 && i<line->ncommands-1){	
							if(i==1 && line->ncommands != 3){ //Segundo mandato
								for(j=i+1; j<line->ncommands-1; j++){ //Cerramos los descriptores que no vamos a usar
									close(pipesDes[j][1]);
									close(pipesDes[j][0]);
								}						
							}
							if(i==line->ncommands-2 && line->ncommands != 3){ //Penúltimo mandato
								for(j=0; j<i-1; j++){ //Cerramos los descriptores que no vamos a usar 
									close(pipesDes[j][1]);
									close(pipesDes[j][0]);	
								}
							}
							if(i!=1 && i!=line->ncommands-2 && line->ncommands != 3){ //Si no es el segundo mandato ni el penúltimo
								//Cerramos los descriptores que no vamos a usar
								for(j=0; j<i-1; j++){ 
									close(pipesDes[j][1]);
									close(pipesDes[j][0]);
								}
								for(j=i+1; j<line->ncommands-1; j++){
									close(pipesDes[j][1]);
									close(pipesDes[j][0]);
								}
							}
							close(pipesDes[i-1][1]);
							dup2(pipesDes[i-1][0],0); //Redireccionamos la entrada estándar de este proceso al pipe anterior
							close(pipesDes[i][0]);
							dup2(pipesDes[i][1],1); //Redireccionamos la salida estándar de este proceso al pipe i
						}

						//Último mandato
						if(i==line->ncommands-1){
							for(j=0; j<line->ncommands-2; j++){ //Cerramos los descriptores que no vamos a usar
								close(pipesDes[j][1]);
								close(pipesDes[j][0]);
							}
							close(pipesDes[i-1][1]);
							dup2(pipesDes[i-1][0],0);
						}
						execv(line->commands[i].filename, line->commands[i].argv);
						fprintf(stderr, "%s: No se encuentra el mandato\n", line->commands[i].argv[0]);
						exit(-1);
					}else{
						childPids[i] = pid;
					}
				}

				//Cerramos los descriptores de cada pipe
				for(i=0; i<line->ncommands-1; i++){
					close(pipesDes[i][1]);
					close(pipesDes[i][0]);
				}

				for(i=0; i<line->ncommands; i++){
					waitpid(childPids[i], NULL, 0);
				}

				//Liberamos memoria
				for(i=0; i<line->ncommands-1; i++){ 
					free(pipesDes[i]);
				}
				free(pipesDes);
				free(childPids);
				exit(-1);
			}else if(line->background){
				dup2(sal, 1);
				TElemento aux;
				crear((longitud(background)+1), pidIntermedio, buf, &aux);
				insertarFinal(aux, &background);
				print(aux);
			}else{
				waitpid(pidIntermedio, NULL, 0);
			}		
		}

		//Miramos que procesos del background han terminado y los eliminamos
		if(!esVacia(background)){
			TLinkedList copiaBackground;
			copiar(background, &copiaBackground);
			while(longitud(copiaBackground)>0){
				TElemento aux;
				primero(copiaBackground, &aux);
				int auxpid = devolverPid(aux);
				int auxid = devolverId(aux);
				if(waitpid(auxpid, NULL, WNOHANG)==auxpid){
					eliminar(auxid, &background);
				}
				eliminar(auxid, &copiaBackground);				
			}
			destruir(&copiaBackground);
		}
		
		//Abrimos la entrada y salidas estándar para poder seguir interactuando correctamente con la consola
		dup2(ent, 0);
		dup2(sal, 1);
		dup2(err, 2);

		printf("msh> ");
        
    }
	destruir(&background);
    return 0;
}

void SignalHandler(){
	printf("\nmsh> ");
	fflush(stdout);
}
