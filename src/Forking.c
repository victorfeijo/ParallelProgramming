#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {

    pid_t pid;

    //Criado Pai e filho
    pid = fork();
    //Se o processo for Pai, então cria outro filho
    if(pid != 0) {
        pid = fork();
    }

    //Agora tendo 1 pai e 2 filhos, se for filho cria outro filho (O avô não entra no if)
    if (pid == 0) {
        pid = fork();
        //Agora tendo 1 avo, 2 pais, e 2 filhos, é preciso que cada pai crie outro filho
        if (pid != 0)
            pid = fork();
    }
    printf("Processos %u Pai %u\n", getpid(), getppid());

    //Aguarda o processo pai
    if (pid != 0)
        wait(NULL);

    return 0;

}
