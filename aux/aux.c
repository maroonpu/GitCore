#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>


#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0


#define MMAP_PATH   "/dev/mem"

#define RALINK_GPIO_DIR_IN      0
#define RALINK_GPIO_DIR_OUT     1

#define RALINK_REG_PIOINT       0x690
#define RALINK_REG_PIOEDGE      0x6A0
#define RALINK_REG_PIORENA      0x650
#define RALINK_REG_PIOFENA      0x660
#define RALINK_REG_PIODATA      0x620
#define RALINK_REG_PIODIR       0x600
#define RALINK_REG_PIOSET       0x630
#define RALINK_REG_PIORESET     0x640

#define RALINK_REG_PIO6332INT       0x694
#define RALINK_REG_PIO6332EDGE      0x6A4
#define RALINK_REG_PIO6332RENA      0x654
#define RALINK_REG_PIO6332FENA      0x664
#define RALINK_REG_PIO6332DATA      0x624
#define RALINK_REG_PIO6332DIR       0x604
#define RALINK_REG_PIO6332SET       0x634
#define RALINK_REG_PIO6332RESET     0x644

#define RALINK_REG_PIO9564INT       0x698
#define RALINK_REG_PIO9564EDGE      0x6A8
#define RALINK_REG_PIO9564RENA      0x658
#define RALINK_REG_PIO9564FENA      0x668
#define RALINK_REG_PIO9564DATA      0x628
#define RALINK_REG_PIO9564DIR       0x608
#define RALINK_REG_PIO9564SET       0x638
#define RALINK_REG_PIO9564RESET     0x648


static uint8_t* gpio_mmap_reg = NULL;
static int gpio_mmap_fd = 0;

static int gpio_mmap(void)
{
    if ((gpio_mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
        fprintf(stderr, "unable to open mmap file");
        return -1;
    }

    gpio_mmap_reg = (uint8_t*) mmap(NULL, 1024, PROT_READ | PROT_WRITE,
        MAP_FILE | MAP_SHARED, gpio_mmap_fd, 0x10000000);
    if (gpio_mmap_reg == MAP_FAILED) {
        perror("foo");
        fprintf(stderr, "failed to mmap");
        gpio_mmap_reg = NULL;
        close(gpio_mmap_fd);
        return -1;
    }

    return 0;
}

int mt76x8_gpio_get_pin(int pin)
{
    uint32_t tmp = 0;

    /* MT7621, MT7628 */
    if (pin <= 31) {
        tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIODATA);
        tmp = (tmp >> pin) & 1u;
    } else if (pin <= 63) {
        tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO6332DATA);
        tmp = (tmp >> (pin-32)) & 1u;
    } else if (pin <= 95) {
        tmp = *(volatile uint32_t *)(gpio_mmap_reg + RALINK_REG_PIO9564DATA);
        tmp = (tmp >> (pin-64)) & 1u;
        tmp = (tmp >> (pin-24)) & 1u;
    }
    return tmp;

}

//是不是数字
int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}

//intCaseSensitive=0大小写不敏感
int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
    if (intCaseSensitive)
        return !strcmp(s1, s2);
    else
        return !strcasecmp(s1, s2);
}

//intCaseSensitive=0大小写不敏感
int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
{
    if (intCaseSensitive)
        return (int) strstr(haystack, needle);
    else
        return (int) strcasestr(haystack, needle);
}

pid_t GetPIDbyName_implements(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
    char chrarry_CommandLinePath[100]  ;
    char chrarry_NameOfProcess[300]  ;
    char* chrptr_StringToCompare = NULL ;
    pid_t pid_ProcessIdentifier = (pid_t) -1 ;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;

    int (*CompareFunction) (const char*, const char*, int) ;

    if (intExactMatch)
        CompareFunction = &strcmp_Wrapper;
    else
        CompareFunction = &strstr_Wrapper;


    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return (pid_t) -2 ;
    }

    // Loop while not NULL
    while ( (de_DirEntity = readdir(dir_proc)) )
    {
        if (de_DirEntity->d_type == DT_DIR)
        {
            if (IsNumeric(de_DirEntity->d_name))
            {
                strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
                strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
                strcat(chrarry_CommandLinePath, "/cmdline") ;
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  //open the file for reading text
                if (fd_CmdLineFile)
                {
                    fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; //read from /proc/<NR>/cmdline
                    fclose(fd_CmdLineFile);  //close the file prior to exiting the routine

                    if (strrchr(chrarry_NameOfProcess, '/'))
                        chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
                    else
                        chrptr_StringToCompare = chrarry_NameOfProcess ;

                    //printf("Process name: %s\n", chrarry_NameOfProcess);
                    //这个是全路径，比如/bin/ls
                    //printf("Pure Process name: %s\n", chrptr_StringToCompare );
                    //这个是纯进程名，比如ls

                    //这里可以比较全路径名，设置为chrarry_NameOfProcess即可
                    if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
                    {
                        pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
                        closedir(dir_proc) ;
                        return pid_ProcessIdentifier ;
                    }
                }
            }
        }
    }
    closedir(dir_proc) ;
    return pid_ProcessIdentifier ;
}

//简单实现
pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName)
{
        return GetPIDbyName_implements(cchrptr_ProcessName, 0,0);//大小写不敏感
}

int main(int argc, char **argv)
{
    // int counter = 0,counterTmp = 0;

    if (gpio_mmap())
        return -1;

    pid_t childPid = fork();


    if(childPid != 0)
    {   
        //parent
        while(1)
        {
            if(!mt76x8_gpio_get_pin(14))
            {
                if(GetPIDbyName_Wrapper("arecord") != -1)
                {
                    kill(GetPIDbyName_Wrapper("arecord"),SIGKILL);
                    kill(GetPIDbyName_Wrapper("aplay"),SIGKILL);
                }

                // while(1)
                // {
                //     if(mt76x8_gpio_get_pin(14))
                //     {
                //         counter++;
                //         break;
                //     }
                // }
                                
            }
            // //pull out
            // if(!mt76x8_gpio_get_pin(14))
            //     kill(childPid+2*(counter+1),SIGKILL);

        }
    }   
    else
    {  
        //child
        while(1)
        {
            // printf("child:%d\n",counter);
            // sleep(1);
            // //insert
            if(mt76x8_gpio_get_pin(14))
            {
                system("arecord -c 2 -r 16000 | aplay -c 2 -r 16000");
                while(mt76x8_gpio_get_pin(14));
            }
        }
    } 

    close(gpio_mmap_fd);

    return 0;
}