


extern void initFDpara_ALL(void);
extern void FengDuan_disp(void);
extern void FengDuan_BUS_card(void);
extern void FengDuan_main(void);

//---------------------------------
extern void FD_Check_devMode(void);
extern int FDParaCheck(void);

#define FDMODE_GPS 0xA0
#define FDMODE_NORMAL 0x5E
extern unsigned char gFDMode;	//��ʶ�ֶ��շ��е��豸���ͣ�0xA1Ϊǰ�Ż�������GPS��GPRS����0xA2Ϊ���Ż���ͨ�����GPSģ��ȷ�ϡ�


#define FDParChange_Line 0x01			//��·
#define FDParChange_Device (0x01<<1)	//�豸��
#define FDParChange_Piaojia (0x01<<2)	//Ʊ��
#define FDParChange_Program (0x01<<4)	//ǰ�ų���
extern void FDParChange_QM(unsigned char mode);	//�����Ƿ�仯��0�ޱ仯�������б仯

//�ֶγ��������õı�־�������־=0x5EΪ���Ż�����־Ϊ0xA0Ϊǰ�Ż�����Ҫ��־����Ч��ͨ��Ӳ�����жϡ�
//��CPUINFO�������豸�����͡������������ж�
extern void SETCpuFDMode(void);



