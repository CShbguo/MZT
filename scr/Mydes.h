
extern void DES_encrypt(unsigned char *plain_text,unsigned char *key_text,unsigned char *encrypt_text);
extern void DES_decrypt(unsigned char *encrypt_text,unsigned char *key_text,unsigned char *plain_text);
extern void DES3_decrypt(unsigned char *encrypt_text,unsigned char *key_text,unsigned char *plain_text);
extern void DES3_encrypt(unsigned char *plain_text,const unsigned char *key_text,unsigned char *encrypt_text);
extern void CountMac1(unsigned char *Key,unsigned char *random,unsigned char *_data,unsigned int len ,unsigned char *Result);
//需要时间比较长
extern void CountMac(unsigned char *key,unsigned char *inittext,unsigned char *Source,unsigned int le ,unsigned char *destin);
