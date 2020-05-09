#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
int PC = 0;
//function to convert positive int to binary (ONLY INPUT POSITIVE INT, OUTPUT POSITIVE BINARY)
char * intToBin(int numberToConvert, int binSize){
    char binary[binSize+1];
    binary[binSize]=0;
    binary[0] = '0';
    //for loops set char [1-4]
    for (int i = binSize; i > 0; i--){
        int testval = (int)pow(2, i-1);
        if (numberToConvert >= testval) {
            binary[binSize-i] = '1';
            numberToConvert -= testval;
        }
        else {
            binary[binSize-i] = '0';
        }
    }
    return strdup(binary);
}

//function to convert from positive to negative binary (INPUT POSITIVE BINARY) (OUTPUT NEGATIVE BINARY (2'S COMP)
char * negateBinary(char * binary, int binSize) {
    //bitwise NOT
    for(int i = 0; i < binSize; i++){
        if(binary[i]=='0'){
            binary[i] = '1';
        }
        else{
            binary[i] = '0';
        }
    }
    //ADD 1
    for (int i = binSize-1; i > 0; i--) {
        if(binary[i]=='1'){
            binary[i] = '0';
        }
        else{
            binary[i] = '1';
            break;
        }
    }
    return strdup(binary);
}
char * convertImmVal(char * command, int binSize){
    //input has # before number
    if (command[1] != '-') {// if NOT negative
        int numberToConvert=0;
        for(int i = 1; i < strlen(command); i++){
            numberToConvert += (command[i]-48) * pow(10,strlen(command)-i-1);
        }
        char * result = intToBin(numberToConvert,binSize);

        return strdup(result);
    }
    else { //if negative number
        //two's compliment calc by convert numeric value to binary, do bitwise NOT and add one.
        int numberToConvert=0;
        for(int i = 2; i < strlen(command); i++){
            numberToConvert += (command[i]-48) * pow(10,strlen(command)-i-1);
        }
        char * result = intToBin(numberToConvert,binSize);
        result = negateBinary(result,binSize);

        return strdup(result);
    }
}

char * hexiToBinary(char * hexi){
    hexi = hexi+1;//increment to avoid the x before the hexi value
    int numToConvert;
    char result[17];
    result[16] = 0;
    for(int i = 0; i < 4; i++){
        if(hexi[i]>='0' && hexi[i]<= '9')
        numToConvert = hexi[i]-48;
        else{
            numToConvert = hexi[i]-55;
        }
        char * fourBin = intToBin(numToConvert,4);
        for(int j = 0; j < 4; j++){
            result[j+i*4] = fourBin[j];
        }
    }
    return strdup(result);
}

//function to convert one part of command to part of final instruction
char * convertCommand(char * command) {
    if ( strcmp(command, "ADD") == 0) {
        return "0001";
    }
    else if (command[0] == 'B' && command[1]=='R') {
        char result[] = "0000000";
        for(int i = 0; i < strlen(command); i++){
            if(command[i]=='n'){result[4]='1';}
            if(command[i]=='z'){result[5]='1';}
            if(command[i]=='p'){result[6]='1';}
        }
        return strdup(result);
    }
    else if ( strcmp(command, "ST") == 0) {
        return "0011";
    }
    else if ( strcmp(command, "LD") == 0) {
        return "0010";
    }
    else if ( strcmp(command, "LDR") == 0) {
        return "0110";
    }
    else if ( strcmp(command, "NOT") == 0) {
        return "1001";
    }
    else if ( strcmp(command, ".ORIG")==0){
        return "";//output nothing this time around - just need convert hexi to bit
    }
    else if ( strcmp(command, ".FILL")==0){
        return "";//output nothing this time around - just need convert hexi to bit
    }
    else if ( strcmp(command, ".BLKW")==0){
        return "";//output nothing this time around - print 16 zeroes, param times
    }
    else if ( strcmp(command, ".STRINGZ")==0){
        return "";//output nothing this time around - print 16 zeroes, param times
    }
    // registers
    else if (command[0] == 'R') {
        int numberToConvert = command[1] - 48;
        char * result = intToBin(numberToConvert,3);

        return strdup(result);
    }
    return "The command couldn't be found";
}




// test function

int main() {

/*
    ADD R1, R1, #3 (1 bits for finding register) (5 bits)
    NOT R1, R2

    ST  SR(3 bits) PCoffset9 (9 bits)
    LD R1, #-3 (PCOffset) (9 bits)
    BR  |n|z|p|(3 bits) PCoffset9 (9 bits) husk at tage til eftertanke

    LDR DR (3 bits) BaseR (3 bits) offset6 (6 bits)
*/

    FILE * filePointer;
    int bufferLength = 60;
    char input[bufferLength];

    filePointer = fopen("C:\\Users\\Jacob Berg Eriksen\\CLionProjects\\MaskinJ\\maskin.txt", "r");

    if(filePointer== NULL){
        printf("filepoint is null\n");
    }

    while (fgets(input, bufferLength, filePointer)!=NULL){
        char delim[] = " ";

        printf("%s\n", input);

        char * token = strtok(input, delim);
        // convert the command
        printf("%s", convertCommand(token));

        if (strcmp(token, "ADD") == 0) {
            // 1. DR
            printf("%s",convertCommand(strtok(NULL, delim)));
            // 2. SR1
            token = strtok(NULL, delim);
            printf("%s", convertCommand(token));

            // 3. SR2 register value or immediate value
            token = strtok(NULL, "\n");
            if (token[0] == 'R') {
                printf("%s", "000");
                printf("%s\n", convertCommand(token));
            }
            else if(token[0] == '#'){
                printf("%s%s\n", "1", convertImmVal(token, 5));
            }
        }
        else if (strcmp(token, "NOT") == 0) {
            //1. register
            printf("%s",convertCommand(strtok(NULL, delim)));
            // 2. register value
            token = strtok(NULL, delim);
            printf("%s%s\n", convertCommand(token), "111111");
        }
        else if (strcmp(token, "ST") == 0) {
            //1. DR
            printf("%s",convertCommand(strtok(NULL, delim)));
            // 2. offset convert
            token = strtok(NULL, "\n");
            printf("%s\n", convertImmVal(token, 9));
        }
        else if (strcmp(token, "LD") == 0) {
            // 1. Register
            printf("%s",convertCommand(strtok(NULL, delim)));
            // 2. offset convert
            token = strtok(NULL, "\n");
            printf("%s\n", convertImmVal(token, 9));
        }
        else if (token[0]=='B' && token[1]=='R') {
            //1. offset convert
            printf("%s\n", convertImmVal(strtok(NULL, "\n"), 9));
        }
        else if (strcmp(token, "LDR") == 0) {
            //1. Register
            printf("%s",convertCommand(strtok(NULL, delim)));
            // baseR set.
            token = strtok(NULL, delim);
            printf("%s%s\n", convertCommand(token), convertImmVal(strtok(NULL, delim), 6));
        }
        else if (strcmp(token, ".ORIG") == 0){
            //convert hexi to 16 bit binary
            token = strtok(NULL, delim);
            printf("%s\n", hexiToBinary(token));
        }
        else if (strcmp(token, ".FILL") == 0){
            //convert hexi to 16 bit binary
            token = strtok(NULL, delim);
            if(token[0]=='x') {
                printf("%s\n", hexiToBinary(token));
            }
            else if(token[0]=='#'){
                printf("%s\n", convertImmVal(token, 16));
            }
        }
        else if (strcmp(token, ".BLKW") == 0){
            token = strtok(NULL, "\n");
            int blkwSize = 0;
            for(int i = 0; i < strlen(token); i++){
                blkwSize += (token[i]-48) * pow(10,strlen(token)-i-1);
            }
            for(int i = 0; i < blkwSize; i++){
                printf("0000000000000000\n");
            }
        }
        else if (strcmp(token, ".STRINGZ") == 0){
            token = strtok(NULL, "\"");
            int length = 0;
            int charVal = 0;
            length = strlen(token);
            for(int i = 0; i <length; i++){//loop through each char in word
                charVal = token[i];
                printf("char: %c bin: %s\n",token[i], intToBin(charVal,16));
            }

        }
    }
    return 0;
}
