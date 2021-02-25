#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class CSEM {
private :
    union semun{ // 用于信号操作的共同体
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    };
    int sem_id; // 信号量描述符号
public:
    bool init(key_t key); // 如果信号量已经存在，获取信号量； 如果信号量不存在，则创建信号量并初始化。
    bool wait(); // 等待信号量
    bool post(); // 产生信号量
    bool destory(); // 销毁信号量
};

int main(int argc, char **argv) {
    CSEM sem;

    // 初始信号量
    if (sem.init(0x5000) == false) {
        printf("sem.init failed.\n");
        return -1;
    }
    printf("sem.init ok\n");

    // 等待信号量，等待成功后，将持有信号量，我感觉信号量也像锁一样
    if (sem.wait() == false) {
        printf("sem.wait failed.\n");
        return -1;
    }
    printf("sem.wait ok\n");

    sleep(30); // 在sleep的过程中，其他需要这个信号量的程序将会等待锁

    // 挂出信号量，释放锁
    if (sem.post() == false) {
        printf("sem.post failed.\n");
        return -1;
    }
    printf("sem.post  ok\n");

    // 销毁信号量
    // if(sem.destory() == false) 
    // {
    //     printf("sem.destory failed.\n");
    //     return -1;
    // }
    // printf("sem.destory ok\n");
}

bool CSEM::init(key_t key){
    // 获取信号量
    if((sem_id=semget(key, 1, 0640)) == -1){
        // 如果信号量不存在，创建它
        if(errno == 2){
            if((sem_id = semget(key, 1, 0640 | IPC_CREAT)) == -1) {
                perror("init 1 semget()");
                return false;
            }
            // 信号量创建成功后，还需要把它初始化成可用的状态
            union semun sem_union;
            sem_union.val = 1;
            if(semctl(sem_id, 0, SETVAL, sem_union) < 0) {
                perror("init semctl()");
                return false;
            }
        }else {
            perror("init 2 semget()");
            return false;
        }
    }
    return true;
}

bool CSEM::destory() {
    if(semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("destroy semctl()");
        return false;
    }
    return true;
}

bool CSEM::wait() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; 
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_b, 1) == -1) {
        perror("wait semop()");
        return false;
    }
    return true;
}

bool CSEM::post() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        perror("post semop()");
        return false;
    }
    return true;
}