/*
* Description: Basic shell implementation. Shoud be able to preform the basic 
* unix commands ls, pwd, mkdir, cd, cp,. mv, rm, and cat.
*
* Author: Miles Wright
*
* Date: 10/22/2019
*
* Notes: 
* 1. <add notes we should consider when grading>
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
/*---------------------------------------------------------------------------*/
/*-----------------------------Functions-------------------------------------*/


void listDir(){
	// unix ls command
	char *pwd = (char *)malloc(sizeof(char) * 1024);
	DIR *dir;
	struct dirent *dp;
	int name_len;

	getcwd(pwd, 1024);
	dir = opendir(pwd);
	while(dir){
		if((dp = readdir(dir)) != NULL) {

			name_len = strlen(dp->d_name);
			write(STDOUT_FILENO, dp->d_name, strlen(dp->d_name));
			write(STDOUT_FILENO, " ", 1);
		}
		else{
			break;
		}
	}
	write(STDOUT_FILENO, "\n", 1);
	closedir(dir);
	free(pwd);
}

void showCurrentDir() {
	// unix pwd command
	char *pwd = (char *)malloc(sizeof(char) * 1024);
	getcwd(pwd, 1024);
	write(STDOUT_FILENO, pwd, strlen(pwd));
	write(STDOUT_FILENO, "\n", 1);
	free(pwd);
}

void makeDir(char *dirName) {
	// unix mkdir command
	if(dirName[strlen(dirName) - 1] == '\n'){
		dirName[strlen(dirName) - 1] = '\0';
	}
	char *pwd = (char *)malloc(sizeof(char) * 1024);
	getcwd(pwd, 1024);
	strcat(pwd, "/");
	strcat(pwd, dirName);
	mkdir(pwd, ACCESSPERMS);
	free(pwd);


}

void changeDir(char *dirName) {
	// cd command
	if(dirName[strlen(dirName) - 1] == '\n'){
		dirName[strlen(dirName) - 1] = '\0';
	}
	char *pwd = (char *)malloc(sizeof(char) * 1024);
	getcwd(pwd, 1024);
	strcat(pwd, "/");
	strcat(pwd, dirName);
	chdir(pwd);
	free(pwd);

}

void copyFile(char *sourcePath, char *destinationPath) {
	// cp command
	
	if(destinationPath[strlen(destinationPath) - 1] == '\n'){
		destinationPath[strlen(destinationPath) - 1] = '\0';
	}
	int in_file = open(sourcePath, O_RDONLY);
	int end = strlen(sourcePath);
	int i;
	char *newDestination = (char *)malloc(sizeof(char) * 1000);;
	strcpy(newDestination, destinationPath);
	strcat(newDestination, "/");
	for(i = end; i > 0; i--){
		if(&sourcePath[i] == "/"){
			strcat(newDestination, sourcePath + strlen(sourcePath) - i);
			break;
		}
		else if(i == 1){
			strcat(newDestination, sourcePath);
			break;
		}

	}
	int out_file = open(newDestination, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
	char *buff = (char *)malloc(sizeof(char) * 1024 + 1);
	size_t buff_size = 1024;
	ssize_t line_size = 0;
	int buff_len = 0;
	
	line_size = read(in_file, buff, buff_size);

	while(line_size > 0){
		// needed to null terminate the string to prevent error.	
		buff[line_size] = '\0';
		buff_len = strlen(buff);
		write(out_file, buff, buff_len);
		line_size = read(in_file, buff, buff_size);
	}
	close(in_file);
	close(out_file);
	free(buff);
}

void moveFile(char *sourcePath, char *destinationPath) {
	// mv command
	if(destinationPath[strlen(destinationPath) - 1] == '\n'){
		destinationPath[strlen(destinationPath) - 1] = '\0';
	}
	int in_file = open(sourcePath, O_RDONLY);
	int out_file = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
	char *buff = (char *)malloc(sizeof(char) * 1024 + 1);
	size_t buff_size = 1024;
	ssize_t line_size = 0;
	int buff_len = 0;
	
	line_size = read(in_file, buff, buff_size);

	while(line_size > 0){
		// needed to null terminate the string to prevent error.	
		buff[line_size] = '\0';
		buff_len = strlen(buff);
		write(out_file, buff, buff_len);
		line_size = read(in_file, buff, buff_size);
	}
	close(in_file);
	close(out_file);
	free(buff);
	deleteFile(sourcePath);
}

void deleteFile(char *filename) {
	// rm command
	if(filename[strlen(filename) - 1] == '\n'){
		filename[strlen(filename) - 1] = '\0';
	}
	remove(filename);
	
}

void displayFile(char *filename) {
	// cat command
	
	if(filename[strlen(filename) - 1] == '\n'){
		filename[strlen(filename) - 1] = '\0';
	}
	int in_file = open(filename, O_RDONLY);
	char *buff = (char *)malloc(sizeof(char) * 1024 + 1);
	size_t buff_size = 1024;
	ssize_t line_size = 0;
	int buff_len = 0;
	
	line_size = read(in_file, buff, buff_size);

	while(line_size > 0){
		// needed to null terminate the string to prevent error.	
		buff[line_size] = '\0';
		buff_len = strlen(buff);
		write(STDOUT_FILENO, buff, buff_len);
		line_size = read(in_file, buff, buff_size);
	}
	close(in_file);
	free(buff);
		
}
/*-----------------------------Program End-----------------------------------*/
