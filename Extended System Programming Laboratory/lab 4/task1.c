#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <units.c>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  //////////////my additions//////////////
  char display_mode;
   
  ////////////////////////////////////////
} state;
state myState;

struct fun_desc{
    char *name;
    char (*fun)(state*);
};
////////////////////taken from the unit file////////////////////////////
char* unit_to_decimal_format(int unit_size);
char* unit_to_hexadecimal_format(int unit_size);
void print_units_decimal(FILE* output, char* buffer, int count, state* s) {
    int UnitSize =  s->unit_size;
    char* end = buffer + UnitSize*count;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        fprintf(output, unit_to_decimal_format(UnitSize), var);
        buffer += UnitSize;
    }
}
void print_units_hexadecimal(FILE* output, char* buffer, int count, state* s) {
    int UnitSize =  s->unit_size;
    char* end = buffer + UnitSize*count;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        fprintf(output, unit_to_hexadecimal_format(UnitSize), var);
        buffer += UnitSize;
    }
}

char* unit_to_decimal_format(int unit_size) { 
    static char* formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    return formats[unit_size-1];
} 
char* unit_to_hexadecimal_format(int unit_size) {
    static char* formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats[unit_size-1];
} 
//////////////////////////////////////////////////////////////////////////////////

void Toggle_Debug_Mode(state* s){
    if(s->debug_mode == 0){ // the mode is off
    s->debug_mode = 1;
    printf("Debug flag now on\n");
    }
    else
    {
    s->debug_mode = 0;
    printf("Debug flag now off\n");
    }
    
};

void Set_File_Name(state* s){
    printf("please enter file name:\n");
    char filename[100];
    fgets(filename,100,stdin);

    size_t len = strlen(filename);
    if (filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }


    filename[100 - 1] = '\0';
    
    if(s->debug_mode == 1){ //the debug mode is on
        printf("Debug: file name set to %s \n",filename );
    }
    strncpy(s->file_name, filename, sizeof(s->file_name));
};

void Set_Unit_Size(state* s){
    printf("please enter unit size:\n");
    char in[100];
    fgets(in,100,stdin);
    int size = atoi(in);
    if(size == 1 || size == 2 || size == 4){
        if(s->debug_mode == 1){ //the debug mode is on
        printf("Debug: set size to %i\n",size );
        }
        s->unit_size = size;
    }

};

void Load_Inot_Memory(state* s){
    if(strcmp(s->file_name, "") == 0){
        printf("error: file name is empty\n");
        return;
    }
    FILE* file =  fopen(s->file_name,"r");
    if(file == NULL){
        printf("error: cannot open file\n");
        return;
    }

    printf("enter a location in hexadecimal:\n");
    char loc[100];
    fgets(loc,100,stdin);
    int location;
    int location2;
    sscanf(loc, "%x", &location);
    sscanf(loc, "%d", &location2);
    printf("enter a length in decimal:\n");
    char len[100];
    fgets(len,100,stdin);
    int length;
    sscanf(len, "%d", &length);
    if(s->debug_mode == 1){
        printf("File Name: %s\n", s->file_name);
        printf("Location is: %d\n", location);
        printf("Length: %d\n", length);
    }
    fseek(file, location, SEEK_SET);
    fread(s->mem_buf, s->unit_size, length, file);
    fclose(file);
    
    printf("Loaded %d units into memory\n", length * s->unit_size);
    printf("%x\n", *(s->mem_buf ));
};

void Toggle_Display_Mode(state* s){
    if(s->display_mode == 0){ // the mode is off
    s->display_mode = 1;
    printf("Display flag now on, hexadecimal representation\n");
    }
    else
    {
    s->display_mode = 0;
    printf("Display flag now off, decimal representation\n");
    }
};

void Memory_Display(state* s){
    printf("enter address:\n");
    char in[100];
    fgets(in,100,stdin);
    int addr;
    sscanf(in, "%x", &addr);
    printf("enter length:\n");
    char len[100];
    fgets(len,100,stdin);
    int length; // the same as 'u'
    sscanf(len, "%d", &length);
    printf("the addr is: %d\n", addr);
    printf("the length is: %d\n", length);
    
    if(s->display_mode == 0){
        print_units_decimal(stdout, (s->mem_buf + addr), length, s);
    }
    else{
        print_units_hexadecimal(stdout, s->mem_buf + addr, length, s);
    }

};

void Save_Into_File(state* s) {
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: File name is empty\n");
        return;
    }
   
    FILE* file = fopen(s->file_name, "r+");
    if (file == NULL) {
        printf("Error: Cannot open file\n");
        return;
    }
   
    printf("Please enter <source-address> <target-location> <length>\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
   
    unsigned int source_address, target_location, length;
    sscanf(input, "%x %x %d", &source_address, &target_location, &length);
   
    if (s->debug_mode == 1) {
        printf("File Name: %s\n", s->file_name);
        printf("Source Address: %x\n", source_address);
        printf("Target Location: %#x\n", target_location);
        printf("Length: %d\n", length);
    }
   
    
    fseek(file, target_location, SEEK_SET);
    size_t num_bytes = length * s->unit_size;
   
    // 
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (target_location >= file_size) {
        printf("Error: target-location more than the file size\n");
        fclose(file);
        return;
    }
   
    fseek(file, target_location, SEEK_SET);
    if(source_address == 0){
        fwrite(s->mem_buf, s->unit_size, length, file);
    }else{
    // source_address is a virtual memory address
    fwrite(source_address, s->unit_size, length, file);
    }
   
    fclose(file);
    printf("DONE\n");
}



void Memory_Modify(state* s) {
    printf("Please enter location:\n");
    char location[100];
    char val[100];
    fgets(location, 100, stdin);
    printf("Please enter a val:\n");
    fgets(val, 100, stdin);
   
    int loc;
    int value;
   
    sscanf(location, "%x", &loc);
    sscanf(val, "%x", &value);
   
    if (s->debug_mode == 1) {
        printf("Location: 0x%x\n", loc);
        printf("Val: 0x%x\n", value);
    }
   
    if (loc >= s->mem_count) {
        printf("Invalid memory location.\n");
        return;
    }
   
    
    char* target = s->mem_buf + (loc * s->unit_size);
   
    // Copy the new value to the memory location
    memcpy(target, &value, s->unit_size);
   
    printf("DONE.\n");
}

void Quit(state* s){
    printf("quitting\n");
    exit(0);
};

int main(int argc, char **argv){
struct fun_desc menu[] = { 
{ "Toggle Debug Mode", Toggle_Debug_Mode },
{ "Set File Name", Set_File_Name }, 
{ "Set Unit Size", Set_Unit_Size }, 
{ "Load Into Memory", Load_Inot_Memory }, 
{ "Toggle Display Mode", Toggle_Display_Mode },
{ "Memory Display", Memory_Display },
{ "Save Into File", Save_Into_File },
{ "Memory Modify", Memory_Modify },
{ "Quit", Quit },
{ NULL, NULL } };

char option[5];
state myState;
myState.debug_mode = 0; // its off
myState.display_mode = 0;

while(1){
if(myState.debug_mode == 1){
    printf("Unit Size: %d\n", myState.unit_size);
    printf("File Name: %s\n", myState.file_name);
}
printf("Chose action:\n");
for(int i = 0; i < 9 ; i++){
    printf("%d-", i);
    printf("%s\n", menu[i].name);
}
fgets(option,5,stdin);
int op = option[0] - '0';
printf("the op is: %d\n", op);
menu[op].fun(&myState);
}

}