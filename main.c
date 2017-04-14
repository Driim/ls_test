#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>

char const * sperm(__mode_t mode) {
    static char local_buff[16] = {0};
    int i = 0;
    // user permissions
    if ((mode & S_IRUSR) == S_IRUSR) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWUSR) == S_IWUSR) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXUSR) == S_IXUSR) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // group permissions
    if ((mode & S_IRGRP) == S_IRGRP) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWGRP) == S_IWGRP) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXGRP) == S_IXGRP) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // other permissions
    if ((mode & S_IROTH) == S_IROTH) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWOTH) == S_IWOTH) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXOTH) == S_IXOTH) local_buff[i] = 'x';
    else local_buff[i] = '-';
    return local_buff;
}


int main(int argc, char* argv[])
{
    DIR * dir_desc;
    char *dir_path;
    struct dirent* file_dirent;
    struct stat     statbuf;

    /* если нет папки, то отображаем текущую */
    if(argc == 1) {
        dir_path = getcwd(NULL, 0);
    } else {
        dir_path = argv[1];
    }

    if(dir_path == NULL) {
        printf("Some things go wrong, dir path is NULL\n");
        exit(EXIT_FAILURE);
    }

    dir_desc = opendir(dir_path);
    if(!dir_desc) {
        printf("unable to open dir %s\n", dir_path);
        exit(EXIT_FAILURE);
    }

    if(chdir(dir_path) < 0) {
        printf("unable to change dir to %s\n", dir_path);
        exit(EXIT_FAILURE);
    }

    while((file_dirent = readdir(dir_desc))) {
        struct passwd *pwd;
        struct group *grp;
        struct tm *tm;
        char datestring[256];

        if (!strcmp(file_dirent->d_name, ".")) {
            continue;
        }

        if (!strcmp(file_dirent->d_name, "..")) {  
            continue;
        }

        if (stat(file_dirent->d_name, &statbuf) < 0) {
            printf("unable to stat file %s\n", file_dirent->d_name);
            continue;
        }

        /* Print out type, permissions, and number of links. */
        /* Печатаем  тип, разрешения и количество ссылок */
        printf("%10.10s", sperm(statbuf.st_mode));
        printf("%4d", statbuf.st_nlink);

        /* Печатаем имя владельца, если было найдено */
        pwd = getpwuid(statbuf.st_uid);
        if (pwd != NULL)
            printf(" %-8.8s", pwd->pw_name);
        else
            printf(" %-8d", statbuf.st_uid);

        /* Печатаем имя группы, если было найдено */
        grp = getgrgid(statbuf.st_gid);
        if (grp != NULL)
            printf(" %-8.8s", grp->gr_name);
        else
            printf(" %-8d", statbuf.st_gid);

        /* Hfpvth */
        printf(" %9jd", (intmax_t)statbuf.st_size);

        /* Дата */
        tm = localtime(&statbuf.st_mtime);
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

        printf(" %s %s\n", datestring, file_dirent->d_name);
    }

    exit(EXIT_SUCCESS);
}