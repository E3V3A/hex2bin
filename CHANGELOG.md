
### hex2bin

  20040617 Alf Lacis: Added pad byte (may not always want FF). \
  Added 'break;' to remove GNU compiler warning about label at end of compound statement \
  Added PROGRAM & VERSION strings.

  20071005 PG: Improvements on options parsing \
  20091212 JP: Corrected crash on 0 byte length data records \
  20100402 JP: Corrected bug on physical address calculation for extended linear address record. \
  ADDRESS_MASK is now calculated from MEMORY_SIZE

  20120125 Danny Schneider: \
  Added code for filling a binary file to a given Max_Length relative to Starting Address if Max-Address is larger than Highest-Address \
  20120509 Yoshimasa Nakane: modified error checking (also for output file, JP) \
  20141005 JP: added support for byte swapped hex files \
           corrected bug caused by extra LF at end or within file \
  20141008 JP: removed junk code \
  20141121 Slucx: added line for removing extra CR when entering file name at run time.  \
  20141122 Simone Fratini: small feature added \
  20150116 Richard Genoud (Paratronic): correct buffer overflows/wrong results with the -l flag \
  20150122 JP: added support for different check methods \
  20150221 JP: rewrite of the checksum write/force value \
  20150804 JP: added batch file option \
  20160923 JP: added code for checking filename length \
  20170418 Simone Fratini: added option -t and -T to obtain shorter binary files \

### mot2bin

20040617 Alf Lacis: Added pad byte (may not always want FF). \
         Added initialisation to Checksum to remove GNU \
         compiler warning about possible uninitialised usage \
         Added 2x'break;' to remove GNU compiler warning about label at \
         end of compound statement \
         Added PROGRAM & VERSION strings. \

20071005 PG: Improvements on options parsing \
20091212 JP: Corrected crash on 0 byte length data records \
20100402 JP: ADDRESS_MASK is now calculated from MEMORY_SIZE \

20120125 Danny Schneider: \
         Added code for filling a binary file to a given Max_Length relative to \
         Starting Address if Max-Address is larger than Highest-Address \
20120509 Yoshimasa Nakane: \
         modified error checking (also for output file, JP) \
20141005 JP: added support for byte swapped hex files \
         corrected bug caused by extra LF at end or within file \
20141121 Slucx: added line for removing extra CR when entering file name at run time. \
20150116 Richard Genoud (Paratronic): correct buffer overflows/wrong results with the -l flag \
20150122 JP: added support for different check methods \
20150221 JP: rewrite of the checksum write/force value \
20150804 JP: added batch file option \


