#include "LogToFile.h"

FILE* LogToFile::OpenFile(char * fileName) {
    FILE *fp;
    char tempName[50];
    char* directroy = "/tmp/";
    char pid[10];
    sprintf(pid, "%d", getpid());
    strcpy(tempName, directroy);
    strcat(tempName, fileName);
    strcat(tempName, pid);
    //strcat(tempName, ".txt");
    fileName = tempName;
    fp = fopen(fileName, "ab+");
    return fp;
}

void LogToFile::writeStringln(char * fileName, char *prompt, char *buf) {
    FILE *fp = OpenFile(fileName);
    if (fp != NULL) {
        if (strlen(prompt) > 0)
            fputs(prompt, fp);
        if (buf != 0 && strlen(buf) > 0)
            fputs(buf, fp);
        else
            fputs("NULL", fp);
        fputc((char) 13, fp);
        fputc((char) 10, fp);
        fclose(fp);
    }
}

void LogToFile::writeIntln(char *fileName, char *prompt, int i) {
    FILE *fp = OpenFile(fileName);
    if (fp != NULL) {
        fputs(prompt, fp);
        char str[256];
        sprintf(str, "%d", i);
        fputs(str, fp);
        fputc((char) 13, fp);
        fputc((char) 10, fp);
        fclose(fp);
    }
}

void LogToFile::writeCharln(char *fileName, char *prompt, char c) {
    FILE *fp = OpenFile(fileName);
    if (fp != NULL) {
        fputs(prompt, fp);
        fputc(c, fp);
        fputc((char) 13, fp);
        fputc((char) 10, fp);
        fclose(fp);
    }
}

void LogToFile::writeBinaryStreamln(char * fileName, unsigned char *buf, int size) {
    FILE *fp = OpenFile(fileName);
    char str[10];
    int length = 0;
    if (fp != NULL) {
        for (int i = 0; i < size; i++) {
            sprintf(str, "%x", buf[i]);
            length = strlen(str);
            if (length == 1) {
                fputc('0', fp);
                fputs(str, fp);
            } else {
                fputs(str, fp);
            }
            fputc(' ', fp);

        }
        fputc((char) 13, fp);
        fputc((char) 10, fp);
        fclose(fp);
    }
}

void LogToFile::writeBuffer(char *fileName, unsigned char *buf, int size) {
    FILE *fp = OpenFile(fileName);
    if (fp != NULL) {
        for (int i = 0; i < size; i++) {
            fputc(buf[i], fp);
        }
        fclose(fp);
    }
}

int LogToFile::getFileSize(char *fileName) {
    FILE *fp = fopen(fileName,"rb");
    int length = -1;
    if (fp != NULL) {
        fseek(fp,SEEK_SET,SEEK_END);
        length=ftell(fp);
        fclose(fp);
    }
    return length;
}




