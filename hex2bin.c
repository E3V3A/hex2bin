/*
---------------------------------------------------------------------------
  hex2bin converts an Intel hex file to binary.
---------------------------------------------------------------------------
*/

#define PROGRAM "hex2bin"
#define VERSION "2.5.1"

#include "common.h"

#define NO_ADDRESS_TYPE_SELECTED 0
#define LINEAR_ADDRESS 1
#define SEGMENTED_ADDRESS 2

const char *Pgm_Name = PROGRAM;

int main (int argc, char *argv[]) {

    // line input from file
    char Line[MAX_LINE_SIZE];

    /* flag that a file was read */
    bool Fileread;

    /* cmd-line parameter # */
    char *p;

    int result;

    /* Application specific */
    unsigned int First_Word, Address, Segment, Upper_Address;
    unsigned int Type;
    unsigned int Offset = 0x00;
    unsigned int temp;

    /* 
    We will assume that when one type of addressing is selected, it will be valid for all the
    current file. Records for the other type will be ignored. 
    */
    unsigned int Seg_Lin_Select = NO_ADDRESS_TYPE_SELECTED;

    unsigned int temp2;

    byte	Data_Str[MAX_LINE_SIZE];

    fprintf (stdout,PROGRAM" v"VERSION", Copyright (C) 2017 Jacques Pelletier & contributors\n\n");

    if (argc == 1)
        usage();

    strcpy(Extension, "bin"); /* default is for binary file extension */

	ParseOptions(argc, argv);

    /* when user enters input file name */

    /* Assume last parameter is filename */
    GetFilename(Filename,argv[argc -1]);

    /* Just a normal file name */
    NoFailOpenInputFile (Filename);
    PutExtension(Filename, Extension);
    NoFailOpenOutputFile(Filename);
    Fileread = true;

    /*
    When the hex file is opened, the program will read it in 2 passes.
    The first pass gets the highest and lowest addresses so that we can allocate the right size.
    The second pass processes the hex data.

    To begin, assume the lowest address is at the end of the memory.
    While reading each records, subsequent addresses will lower this number.
    At the end of the input file, this value will be the lowest address.

    A similar assumption is made for highest address. It starts at the
    beginning of memory. While reading each records, subsequent addresses will raise this number.
    At the end of the input file, this value will be the highest address. 
    */
    Lowest_Address  = (unsigned int)-1;
    Highest_Address = 0;
    Records_Start   = 0;
    Segment         = 0;
    Upper_Address   = 0;
    Record_Nb       = 0;    // Used for reporting errors
    First_Word      = 0;

    /* Check if are set Floor and Ceiling Address and range is coherent*/
    VerifyRangeFloorCeil();

    /* get highest and lowest addresses so that we can allocate the rintervallo incoerenteight size */
    do {
        unsigned int i;

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

        if (--i != 0) {

            if (Line[i] == '\n') Line[i] = '\0';

            /* Scan the first two bytes and nb of bytes.
               The two bytes are read in First_Word since its use depend on the
               record type: if it's an extended address record or a data record.
               */
            result = sscanf (Line, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
            if (result != 4) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);

            p = (char *) Data_Str;

            /* If we're reading the last record, ignore it. */
            switch (Type) {

            /* Data record */
            case 0:
                if (Nb_Bytes == 0)
                    break;

                Address = First_Word;

                if (Seg_Lin_Select == SEGMENTED_ADDRESS) {
                    Phys_Addr = (Segment << 4) + Address;
                } else {
                    // LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
                    // Upper_Address = 0 as specified in the Intel specifications, 
                    // until an extended address record is read.
                    Phys_Addr = ((Upper_Address << 16) + Address);
                }

                if (Verbose_Flag) fprintf(stderr,"Physical Address: %08X\n",Phys_Addr);

                /* Floor address */
                if (Floor_Address_Setted) {
                    /* Discard if lower than Floor_Address */
                  if (Phys_Addr < (Floor_Address - Starting_Address)) {
                    if (Verbose_Flag) fprintf(stderr,"Discard physical address less than %08X\n",Floor_Address - Starting_Address);
                    break; 
                  }
                }
                /* Set the lowest address as base pointer. */
                if (Phys_Addr < Lowest_Address)
                    Lowest_Address = Phys_Addr;

                /* Same for the top address. */
                temp = Phys_Addr + Nb_Bytes -1;

                /*Ceiling address */
                if (Ceiling_Address_Setted) {
                    /* Discard if higher than Ceiling_Address */
                    if (temp  > (Ceiling_Address +  Starting_Address)) {
                      if (Verbose_Flag) fprintf(stderr,"Discard physical address greater than: %08X\n", Ceiling_Address + Starting_Address);
                      break;
                    }
                }

                if (temp > Highest_Address)
                    Highest_Address = temp;
                if (Verbose_Flag) fprintf(stderr,"Highest_Address: %08X\n", Highest_Address);
                break;


            case 1:
                if (Verbose_Flag) fprintf(stderr,"End of File record\n");
                break;

            case 2:
                /* First_Word contains the offset. It's supposed to be 0000 so
                   we ignore it. */

                /* First extended segment address record ? */
                if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
                    Seg_Lin_Select = SEGMENTED_ADDRESS;

                /* Then ignore subsequent extended linear address records */
                if (Seg_Lin_Select == SEGMENTED_ADDRESS) {
                    result = sscanf (p, "%4x%2x",&Segment,&temp2);
                    if (result != 2) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
                    if (Verbose_Flag) fprintf(stderr,"Extended Segment Address record: %04X\n",Segment);

                    /* Update the current address. */
                    Phys_Addr = (Segment << 4);
                } else {
                    fprintf(stderr,"Ignored extended linear address record %d\n", Record_Nb);
                }
                break;

            case 3:
                if (Verbose_Flag) fprintf(stderr,"Start Segment Address record: ignored\n");
                break;

            case 4:
                /* First_Word contains the offset. It's supposed to be 0000 so
                   we ignore it. */

                /* First extended linear address record ? */
                if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
                    Seg_Lin_Select = LINEAR_ADDRESS;

                /* Then ignore subsequent extended segment address records */
                if (Seg_Lin_Select == LINEAR_ADDRESS) {
                    result = sscanf (p, "%4x%2x",&Upper_Address,&temp2);
                    if (result != 2) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
                    if (Verbose_Flag) fprintf(stderr,"Extended Linear Address record: %04X\n",Upper_Address);
                    /* Update the current address. */
                    Phys_Addr = (Upper_Address << 16);
                    if (Verbose_Flag) fprintf(stderr,"Physical Address: %08X\n",Phys_Addr);
                } else {
                    fprintf(stderr,"Ignored extended segment address record %d\n", Record_Nb);
                }
                break;

            case 5:
                if (Verbose_Flag) fprintf(stderr,"Start Linear Address record: ignored\n");
                break;

            default:
                if (Verbose_Flag) fprintf(stderr,"Unknown record type: %d at %d\n",Type,Record_Nb);
                break;
            }
        }
    }
    while (!feof (Filin));

    if (Address_Alignment_Word)
        Highest_Address += (Highest_Address - Lowest_Address) + 1;

    Allocate_Memory_And_Rewind();

    Segment = 0;
    Upper_Address = 0;
    Record_Nb = 0;

    /* Read the file & process the lines. */
    /* repeat until EOF(Filin) */
    do {
        unsigned int i;

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

        //fprintf(stderr,"Record: %d; length: %d\n", Record_Nb, i);

        if (--i != 0) {

            if (Line[i] == '\n') Line[i] = '\0';

            /* 
            Scan the first two bytes and nb of bytes.
            The two bytes are read in First_Word since its use depend on the
            record type: if it's an extended address record or a data record.
            */
            result = sscanf (Line, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
            if (result != 4) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);

            Checksum = Nb_Bytes + (First_Word >> 8) + (First_Word & 0xFF) + Type;

            p = (char *) Data_Str;

            /* If we're reading the last record, ignore it. */
            switch (Type)
            {
            /* Data record */
            case 0:
                if (Nb_Bytes == 0)
                {
                    fprintf(stderr,"0 byte length Data record ignored\n");
                    break;
                }

                Address = First_Word;

                if (Seg_Lin_Select == SEGMENTED_ADDRESS)
                    Phys_Addr = (Segment << 4) + Address;
                else
                    /* LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
                       Upper_Address = 0 as specified in the Intel spec. until an extended address
                       record is read. */
                    if (Address_Alignment_Word)
                        Phys_Addr = ((Upper_Address << 16) + (Address << 1)) + Offset;
                    else
                        Phys_Addr = ((Upper_Address << 16) + Address);

                /* Check that the physical address stays in the buffer's range. */
                if ((Phys_Addr >= Lowest_Address) && (Phys_Addr <= Highest_Address))
                {
                    /* The memory block begins at Lowest_Address */
                    Phys_Addr -= Lowest_Address;

					p = ReadDataBytes(p);

                    /* Read the Checksum value. */
                    result = sscanf (p, "%2x",&temp2);
                    if (result != 1) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);

					/* Verify Checksum value. */
                    Checksum = (Checksum + temp2) & 0xFF;
                    VerifyChecksumValue();
                }
                else
                {
                    if (Seg_Lin_Select == SEGMENTED_ADDRESS)
                        fprintf(stderr,"Data record skipped at %4X:%4X\n",Segment,Address);
                    else
                        fprintf(stderr,"Data record skipped at %8X\n",Phys_Addr);
                }

                break;

            /* End of file record */
            case 1:
                /* Simply ignore checksum errors in this line. */
                break;

            /* Extended segment address record */
            case 2:
                /* First_Word contains the offset. It's supposed to be 0000 so
                   we ignore it. */

                /* First extended segment address record ? */
                if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
                    Seg_Lin_Select = SEGMENTED_ADDRESS;

                /* Then ignore subsequent extended linear address records */
                if (Seg_Lin_Select == SEGMENTED_ADDRESS)
                {
                    result = sscanf (p, "%4x%2x",&Segment,&temp2);
                    if (result != 2) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);

                    /* Update the current address. */
                    Phys_Addr = (Segment << 4);

                    /* Verify Checksum value. */
                    Checksum = (Checksum + (Segment >> 8) + (Segment & 0xFF) + temp2) & 0xFF;
					VerifyChecksumValue();
                }
                break;

            /* Start segment address record */
            case 3:
                /* Nothing to be done since it's for specifying the starting address for
                   execution of the binary code */
                break;

            /* Extended linear address record */
            case 4:
                /* First_Word contains the offset. It's supposed to be 0000 so
                   we ignore it. */

                if (Address_Alignment_Word)
                {
                    sscanf (p, "%4x",&Offset);
                    Offset = Offset << 16;
                    Offset -= Lowest_Address;

                }
                /* First extended linear address record ? */
                if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
                    Seg_Lin_Select = LINEAR_ADDRESS;

                /* Then ignore subsequent extended segment address records */
                if (Seg_Lin_Select == LINEAR_ADDRESS)
                {
                    result = sscanf (p, "%4x%2x",&Upper_Address,&temp2);
                    if (result != 2) fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);

                    /* Update the current address. */
                    Phys_Addr = (Upper_Address << 16);

                    /* Verify Checksum value. */
                    Checksum = (Checksum + (Upper_Address >> 8) + (Upper_Address & 0xFF) + temp2)
                               & 0xFF;
					VerifyChecksumValue();
                }
                break;

            /* Start linear address record */
            case 5:
                /* Nothing to be done since it's for specifying the starting address for
                   execution of the binary code */
                break;
            default:
                fprintf(stderr,"Unknown record type\n");
                break;
            }
        }
    }
    while (!feof (Filin));
    /*-----------------------------------------------------------------------------*/

    fprintf(stdout,"Binary file start = %08X\n",Lowest_Address);
    fprintf(stdout,"Records start     = %08X\n",Records_Start);
    fprintf(stdout,"Highest address   = %08X\n",Highest_Address);
    fprintf(stdout,"Pad Byte          = %X\n",  Pad_Byte);

	WriteMemory();

#ifdef USE_FILE_BUFFERS
    free (FilinBuf);
    free (FiloutBuf);
#endif

    fclose (Filin);
    fclose (Filout);

    if (Status_Checksum_Error && Enable_Checksum_Error)
    {
        fprintf(stderr,"Checksum error detected.\n");
        return 1;
    }

    if (!Fileread)
        usage();
    return 0;
}
