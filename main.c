#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


struct {
    char * name;
    int linenumber;
} labels[20];


int labelcounter = 0;
int linecount = -1;
int startCount = -1;

char * convertLabelAddress(char * labelname);

//function to convert positive int to binary (ONLY INPUT POSITIVE INT, OUTPUT POSITIVE BINARY)
char * intToBin(int numberToConvert, int binSize){
    char binary[binSize+1];
    binary[binSize]=0;
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
        return "";//output nothing this time around - just need convert input hexi to bin
    }
    else if ( strcmp(command, ".FILL")==0){
        return "";//output nothing this time around - just need convert input hex or int to bin
    }
    else if ( strcmp(command, ".BLKW")==0){
        return "";//output nothing this time around - print 16 zeroes, param times
    }
    else if ( strcmp(command, ".STRINGZ")==0){
        return "";//output nothing this time around - print each letter's ascii val as 16 bit bin
    }
    else if ( strcmp(command, "HALT\n")==0){
        return "1111000000100101\n";//calling TRAP x25   (25 is 8bit hexi)
    }
    // registers
    else if (command[0] == 'R') {
        int numberToConvert = command[1] - 48;
        char * result = intToBin(numberToConvert,3);

        return strdup(result);
    }
    else {
        return "";
    }
}


// converts label name to proper 9 bit offset
char * convertLabelAddress(char * labelname) {
    for (int i = 0; i < 20; i++){
        if ( strcmp(labelname, labels[i].name)==0) {//finding correct label

            // diff between label adress and pc
            int lineDiff = (labels[i].linenumber - linecount);
            //printf(" %d %d %d ", linecount, labels[i].linenumber, lineDiff);
            if(lineDiff < 0){//if negative
                return negateBinary(intToBin(-lineDiff, 9), 9);
            }
            else{//if positive
                return intToBin(lineDiff,9);
            }
        }
    }
}

int binToDeci(char * binary){
    int result=0;
    for(int i = 0; i<strlen(binary); i++){
        if(binary[i]=='1'){
            result += (int) pow(2, (double)strlen(binary)-1-i);
        }
    }
    return result;
}
char * symbolTable(FILE * filePointer){

    int bufferLength = 60;
    char input[bufferLength];

    while (fgets(input, bufferLength, filePointer)!=NULL){
        char * command = strtok(input, " ");

        if(strcmp(command,".ORIG")==0){
            command = strtok(NULL, "\n");
            command = hexiToBinary(command);
            startCount = binToDeci(command);
            linecount = startCount-1;
        }
        else if (!(strcmp(command, "ADD") == 0 ||
            (command[0] == 'B' && command[1]=='R')||
            strcmp(command, "ST") == 0 ||
            strcmp(command, "LD") == 0 ||
            strcmp(command, "LDR") == 0 ||
            strcmp(command, "NOT") == 0 ||
            strcmp(command, ".ORIG")==0 ||
            strcmp(command, ".FILL")==0 ||
            strcmp(command, ".BLKW")==0 ||
            strcmp(command, ".STRINGZ")==0 ||
            strcmp(command, ".END\n")==0 ||
            strcmp(command, "HALT\n")==0 || //newline cuz this input has no parameters just like the
            command[0] == 'R')){
            command[strlen(command)-1] = 0;
            labels[labelcounter].name = strdup(command);
            labels[labelcounter].linenumber = linecount;
            linecount--;
            labelcounter++;
        }
        linecount++;
    }

    FILE * symbolTable = fopen("symbolTable.txt", "w");
    for (int i = 0; i < 20; ++i) {
        if((labels[i].name != NULL)){
            fprintf(symbolTable, "%s\t\tx%x\n",labels[i].name,labels[i].linenumber);
        }
    }
}

int main() {

    //open input file and run through code to set labels:
    FILE * filePointer;
    int bufferLength = 60;
    char input[bufferLength];
    filePointer = fopen("maskin.txt", "r");
    if(filePointer== NULL){
        printf("filepoint is null\n");
    }
    symbolTable(filePointer);

    //reset filepointer to run rest of program
    filePointer = fopen("maskin.txt", "r");
    if(filePointer== NULL){
        printf("filepoint is null\n");
    }

    //set outputfile:
    FILE * BinaryCode = fopen("BinaryCode.txt", "w");

    linecount = startCount-1;
    while (fgets(input, bufferLength, filePointer)!=NULL){
        char delim[] = " ";

        //printf("%s", input);

        char * token = strtok(input, delim);
        // convert the command
        fprintf(BinaryCode,"%s", convertCommand(token));

        if (strcmp(token, "ADD") == 0) {
            // 1. DR
            fprintf(BinaryCode,"%s",convertCommand(strtok(NULL, delim)));
            // 2. SR1
            token = strtok(NULL, delim);
            fprintf(BinaryCode,"%s", convertCommand(token));

            // 3. SR2 register value or immediate value
            token = strtok(NULL, "\n");
            if (token[0] == 'R') {//if register
                fprintf(BinaryCode,"%s", "000");
                fprintf(BinaryCode,"%s\n", convertCommand(token));
            }
            else if(token[0] == '#'){//if immediate val in format: #number
                fprintf(BinaryCode,"%s%s\n", "1", convertImmVal(token, 5));
            }
        }
        else if (strcmp(token, "NOT") == 0) {
            //1. register
            fprintf(BinaryCode,"%s",convertCommand(strtok(NULL, delim)));
            // 2. register value
            token = strtok(NULL, "\n");
            fprintf(BinaryCode,"%s%s\n", convertCommand(token), "111111");
        }
        else if (strcmp(token, "ST") == 0) {
            //1. DR
            fprintf(BinaryCode,"%s",convertCommand(strtok(NULL, delim)));
            // 2. offset convert
            token = strtok(NULL, "\n");

            if (token[0] == '#') {//if immediate val in format: #number
                fprintf(BinaryCode,"%s\n", convertImmVal(token, 9));
            }
            else {//if label
                fprintf(BinaryCode,"%s\n", convertLabelAddress(token));
            }

        }
        else if (strcmp(token, "LD") == 0) {
            // 1. Register
            fprintf(BinaryCode,"%s",convertCommand(strtok(NULL, delim)));
            // 2. offset convert
            token = strtok(NULL, "\n");

            if (token[0] == '#') { //if immediate val in format: #number
                fprintf(BinaryCode,"%s\n", convertImmVal(token, 9));
            }
            else {//if label
                // inserting offset (label pos - current pos)
                fprintf(BinaryCode,"%s\n", convertLabelAddress(token));
            }
        }
        else if (token[0]=='B' && token[1]=='R') {
            //1. offset convert
            token = strtok(NULL, "\n");
            if(token[0]=='#'){//if immediate val in format: #number
                fprintf(BinaryCode,"%s\n",convertImmVal(token,9));
            }
            else{//if label
                fprintf(BinaryCode,"%s\n", convertLabelAddress(token));
            }
        }
        else if (strcmp(token, "LDR") == 0) {
            //1. Register
            fprintf(BinaryCode,"%s",convertCommand(strtok(NULL, delim)));
            // baseR set.
            token = strtok(NULL, delim);
            fprintf(BinaryCode,"%s%s\n", convertCommand(token), convertImmVal(strtok(NULL, "\n"), 6));
        }
        else if (strcmp(token, ".ORIG") == 0){
            //convert hexi to 16 bit binary
            token = strtok(NULL, delim);
            fprintf(BinaryCode,"%s\n", hexiToBinary(token));
        }
        else if (strcmp(token, ".FILL") == 0){
            //convert hexi to 16 bit binary
            token = strtok(NULL, "\n");
            if(token[0]=='x') {
                fprintf(BinaryCode,"%s\n", hexiToBinary(token));
            }
            else if(token[0]=='#'){
                fprintf(BinaryCode,"%s\n", convertImmVal(token, 16));
            }
        }
        else if (strcmp(token, ".BLKW") == 0){
            token = strtok(NULL, "\n");
            int blkwSize = 0;
            for(int i = 0; i < strlen(token); i++){
                blkwSize += (token[i]-48) * pow(10,strlen(token)-i-1);
            }
            for(int i = 0; i < blkwSize; i++){
                fprintf(BinaryCode,"0000000000000000\n");
            }
        }
        else if (strcmp(token, ".STRINGZ") == 0){
            token = strtok(NULL, "\"");
            int length = 0;
            int charVal = 0;
            length = strlen(token);
            for(int i = 0; i <length; i++){//loop through each char in word
                charVal = token[i];
                fprintf(BinaryCode,"%s\n",intToBin(charVal,16));
              //  printf("char: %c bin: %s\n",token[i], intToBin(charVal,16));
            }
        }
        else if ( strcmp(token, "HALT\n")==0){
            //print is made in convertCommand 1st call
        }
        else if ( strcmp(token, ".END\n")==0){
            //print is made in convertCommand 1st call
        }
        else {
            // if read line is label print nothing and dont count the line (no output instruction)
            linecount--;
        }
        //count every line
        linecount++;
    }
    return 0;
}
