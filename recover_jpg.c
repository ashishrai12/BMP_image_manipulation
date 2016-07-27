// for jpegs
#include <stdio.h>
#include <stdlib.h>
#include<stdint.h>


int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 1)
    {
        printf("Usage: ./recover\n");
        return 1;
    }
    uint8_t buffer[512];
    char* infile = "card.raw";
    char title[20];
    int count=0;
    FILE* filer = fopen(infile, "r");
    if (filer == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    //While data exists
    while(1)
    { 
    fread(&buffer,512,1,filer);
    if(buffer[0]==0xff && (buffer[1] == 0xd8) && (buffer[2] == 0xff) && ((buffer[3] == 0xe0) || (buffer[3] == 0xe1)))
    {
        sprintf(title,"%03d.jpg",count);//New jpeg
        FILE* filew= fopen(title,"a+");
        do
        {
        fwrite(&buffer,512,1,filew);// Write the 512 bytes
        if(fread(&buffer,512,1,filer)==0)//Read next 512 bytes
        {
        fclose(filer);
        return 0;
        }
        }
        while(!((buffer[0]==0xff) && (buffer[1] == 0xd8) && (buffer[2] == 0xff) && ((buffer[3] == 0xe0) || (buffer[3] ==0xe1))));
        fclose(filew);
        count=count+1;
        //Go back to start of file
        fseek(filer, -512, SEEK_CUR);
    
    }
    
    }
    
    return 0;
    
}

