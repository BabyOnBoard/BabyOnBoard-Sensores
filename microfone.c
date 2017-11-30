#include<stdio.h>		//lib para usar fprintf() e perror()
#include<stdlib.h>		//lib para usar macros EXIT_SUCCESS e EXIT_FAILURE
#include<unistd.h>		//lib para usar fork(), getpid() e getppid()
#include<sys/types.h>		//lib para usar tipo pid_t
#include<sys/wait.h>		//lib para usar wait()


int main(){
    pid_t child_pid;
    int status;

    /* Cria processo filho */
    child_pid = fork();

    if (child_pid == 0) {
        /* Processo filho. */
        fprintf(stdout, " Iniciando Processo Filho. Possui PID: %d.\n", (int)getpid() );
	fprintf(stdout, "PID do Processo Pai: %d.\n", (int)getppid() );
	fprintf(stdout, "Iniciando processo para gravação de audio pelo microfone");
	system("cd /home/pi/Git/BabyOnBoard-Sensores/");
	system("rec -r 48000 -c1 teste.wav trim 0 5");
		//sleep(4);
	_exit(EXIT_SUCCESS);
    }
    else if (child_pid < 0) {
        /* Se houver erro ao criar processo filho */
	perror("fork");
	exit(EXIT_FAILURE);
    }
    else {
	/* Código do processo pai continua aqui */
	/* Aguarda processo filho terminar e recebe status */
	wait(& status);

	/* Verifica se o processo filho terminou normalmente */
	/* Caso tenha terminado, imprime o status de saída do filho */
	if (WIFEXITED(status)) {
	    fprintf(stdout, "Status de saida do filho: %d.\n", WEXITSTATUS(status) );
        }
	else {
	    fprintf(stdout, "Processo filho terminou de forma anormal.\n");
        }

	/* Imprime dados na tela */
	fprintf(stdout, "Processo pai. meu PID: %d.\n", (int)getpid() );
	fprintf(stdout, "Processo pai. PID do processo filho anterior: %d.\n", (int)child_pid );
	fprintf(stdout, "Iniciando processo de análise do áudio");
	system("cd /home/pi/Git/BabyOnBoard-Sensores/");
	system("sudo python3 ruido.py");
	fprintf(stdout, "Audio analisado com sucesso!!!!");
    }

    return 0;
}

