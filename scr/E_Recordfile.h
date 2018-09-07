

extern void get_BR_Point(void);
extern void writeBackRec(unsigned char*dat, int len, unsigned char mode);
extern void clr_Record_point(void);
extern int Get_Record_point(unsigned char *headp, unsigned char mode);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern unsigned char LibGetMKey(unsigned char SamSlot, unsigned char *data, unsigned char mode);
extern unsigned char Lib3DESKey(unsigned char *data, unsigned char *outKey, unsigned char mode);
extern void lib_clr_Record_point(void);
extern unsigned char libWriteRecord(unsigned int cFeaddr, unsigned char *buffer, unsigned char mode);

