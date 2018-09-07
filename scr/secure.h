/*================================================================= 
 * Function ID :  GenMacData 
 * Input       :  char *_data,char len 
 * Output      :  char *_data 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  返回_data的长度 
 * Description :  对需要计算MAC的数据进行处理 
 * Notice	   :   
 *					 
 *=================================================================*/ 
int	GenMacData(char *_data, char len); 
/*================================================================= 
 * Function ID :  genmac 
 * Input       :  unsigned char *s_key,unsigned char *random,unsigned int len, 
 *				  unsigned char *input_data 
 * Output      :  unsigned char *mac 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  用过程密钥和随机数计算出MAC1或MAC2(用单DES) 
 * Notice	   :  一般情况下初始random为8个字节的0x00 
 *					 
 *=================================================================*/ 
unsigned char* genmac(unsigned char *key,unsigned char *random,unsigned int len,unsigned char *input_data,unsigned char *out_data); 
/*================================================================= 
 * Function ID :  GenMac 
 * Input       :  unsigned char *s_key,unsigned int len, 
 *				  unsigned char *input_data 
 * Output      :  unsigned char *mac 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  用过程密钥计算出MAC1或MAC2(用单DES) 
 * Notice	   :   
 *					 
 *=================================================================*/ 
unsigned char* GenMac(unsigned char *Key,unsigned char *_data,unsigned char len ,unsigned char Result[8]); 
/*================================================================= 
 * Function ID :  genmac_3des 
 * Input       :  unsigned char *s_key,unsigned char *random,unsigned int len, 
 *				  unsigned char *input_data 
 * Output      :  unsigned char *mac 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  用过程密钥和随机数计算出MAC1或MAC2(用3DES) 
 * Notice	   :  一般情况下初始random为8个字节的0x00 
 *					 
 *=================================================================*/ 
unsigned char* genmac_3des(unsigned char *key,unsigned char *random,unsigned int len,unsigned char *input_data,unsigned char *out_data); 
/*================================================================= 
 * Function ID :  triple_des 
 * Input       :  unsigned char key[17],unsigned char in_data[9] 
 * Output      :  unsigned char out_data[9] 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  对in_data数据进行3DES运算 
 * Notice	   :   
 *					 
 *=================================================================*/ 
unsigned char* triple_des(unsigned char key[17],unsigned char in_data[9],unsigned char out_data[9]); 
/*================================================================= 
 * Function ID :  _triple_des 
 * Input       :  unsigned char key[17],unsigned char in_data[9] 
 * Output      :  unsigned char out_data[9] 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  对in_data数据进行3DES解密运算 
 * Notice	   :   
 *					 
 *=================================================================*/ 
unsigned char* _triple_des(unsigned char key[17],unsigned char in_data[9],unsigned char out_data[9]); 
 
/*================================================================= 
 * Function ID :  pboc_diver_key 
 * Input       :  unsigned char cardno[9], unsigned char key_in[17] 
 * Output      :  unsigned char key_out[17]  
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  对密钥进行分散求子密钥 
 * Notice	   :   
 *					 
 *=================================================================*/ 
unsigned char* pboc_diver_key ( unsigned char cardno[9], unsigned char key_in[17], unsigned char key_out[17] ); 
/*================================================================= 
 * Function ID :  pboc_session_key 
 * Input       :  uchar cardno[9],uchar M_key[17],uchar in_data[9] 
 * Output      :  uchar session_key[9](得到过程密钥) 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :   
 * Description :  计算过程密钥 
 * Notice	   :   
 *					 
 *=================================================================*/ 
unsigned char* pboc_session_key(unsigned char cardno[9],unsigned char M_key[17],unsigned char in_data[9],unsigned char session_key[9]); 
/*================================================================= 
 * Function ID :  SingleMACVB 
 * Input       :  unsigned char *s_key,unsigned char *random,unsigned int len 
 *				  unsigned char *input_data 
 * Output      :  unsigned char *mac 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  将用来算MAC的数据按8的倍数进行处理， 
 *				  再用过程密钥计算出MAC1或MAC2(用单DES) 
 * Notice	   :  一般情况下初始random为8个字节的0x00 
 *					 
 *=================================================================*/ 
unsigned char* SingleMACVB(unsigned char *s_key,unsigned char *random,unsigned int len,unsigned char *input_data,unsigned char *mac); 
/*================================================================= 
 * Function ID :  TripleMACVB 
 * Input       :  unsigned char *s_key,unsigned char *random,unsigned int len 
 *				  unsigned char *input_data 
 * Output      :  unsigned char *mac 
 * Author      :  DengJun 
 * Date        :  Dec  12  2000 
 * Return	   :  void 
 * Description :  将用来算MAC的数据按8的倍数进行处理， 
 *				  再用过程密钥计算出MAC1或MAC2(用3DES) 
 * Notice	   :  一般情况下初始random为8个字节的0x00 
 *					 
 *=================================================================*/ 
unsigned char* TripleMACVB(unsigned char *s_key,unsigned char *random,unsigned int len,unsigned char *input_data,unsigned char *mac); 
