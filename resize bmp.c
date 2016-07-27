
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize size infile outfile\n");
        return 1;
    }

    // remember filenames
    char* size = argv[1];
    char* infile = argv[2];
    char* outfile = argv[3];
    
    int n=atoi(size);
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    

   
    int padding1 =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    bi.biWidth = n * bi.biWidth;
    bi.biHeight = n * bi.biHeight;
    
    // determine padding for scanlines
    int padding2 =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    //int padding2 =  (4 - (bi.biWidth * abs(bi.biHeight)) % 4) % 4;
    bi.biSizeImage=((bi.biWidth*sizeof(RGBTRIPLE))+padding2)*abs(bi.biHeight);
      // write outfile's BITMAPFILEHEADER
       
    //printf("%d",bi.biSizeImage);      
    bf.bfSize=bi.biSizeImage+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
   
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < (biHeight/n); i++)
    {
        // iterate over pixels in scanline
        for(int r=0;r<n;r++)
        {
        for (int j = 0; j < (bi.biWidth/n); j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            
            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            for(int r1=0;r1<n;r1++)
            {          
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
            
        }

        // skip over padding, if any
        fseek(inptr, padding1, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding2; k++)
        {
            fputc(0x00, outptr);
        }
        
        fseek(inptr, -((bi.biWidth*sizeof(RGBTRIPLE)/n)+padding1), SEEK_CUR);
        }
        
        fseek(inptr, ((bi.biWidth*sizeof(RGBTRIPLE)/n)+padding1), SEEK_CUR);
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
