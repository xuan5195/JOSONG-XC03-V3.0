#include "bsp_stmflash.h"
#include "bsp_crc8.h"
#include "bsp.h"

#define FLASH_SAVE_ADD		0x0800FC00		//Flash�洢��ʼ��ַ ΪFlash���һҳ����СΪ1K
 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

/*
static void STMFLASH_Write(u32 WriteAddr,u8 *pBuffer,u16 NumToWrite)	
{
 	u16 i,DateTemp=0;    
	FLASH_Unlock();		//����
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(FLASH_SAVE_ADD);	
	for(i=0;i<NumToWrite;(i=i+2))
	{
		DateTemp = ((u16)pBuffer[i+1])<<8 ;
		DateTemp = DateTemp|pBuffer[i];
		FLASH_ProgramHalfWord(WriteAddr,DateTemp);
	    WriteAddr+=2;//��ַ����2.
	}  
	FLASH_Lock();//����
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u8 *pBuffer,u16 NumToRead)   	
{
	u16 i,DateTemp=0;
	for(i=0;i<NumToRead;(i=i+2))
	{
		DateTemp = STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		pBuffer[i]	=(u8)(DateTemp);
		pBuffer[i+1]=(u8)(DateTemp>>8);
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}*/

//����д
static void STMFLASH_WriteHword(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
 	u16 i;    
	FLASH_Unlock();		//����
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(FLASH_SAVE_ADD);	
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
	FLASH_Lock();//����
}

//���ֶ�
static void STMFLASH_ReadHword(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]= STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.	
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


void Read_Flash_Dat(void)
{
	u16 datatemp[20]={0};
ReReadFlash:
	STMFLASH_ReadHword(FLASH_SAVE_ADD,datatemp,20);
	printf("STMFLASH_Read: %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X; %04X%04X %04X%04X;\r\n",\
	datatemp[ 0],datatemp[ 1],datatemp[ 2],datatemp[ 3],datatemp[ 4],datatemp[ 5],datatemp[ 6],datatemp[ 7],datatemp[ 8],datatemp[ 9],\
	datatemp[10],datatemp[11],datatemp[12],datatemp[12],datatemp[14],datatemp[15],datatemp[16],datatemp[17],datatemp[18],datatemp[19]);
	if(datatemp[18] != 0xAA)	//�°���δ�ܵ����
	{
		printf("New Board...\r\n");
		Write_Flash_Dat();
		goto ReReadFlash;
	}
	else	//ʹ�ù��ģ�ֱ�Ӷ�ȡSN���ݣ���У������
	{
		gParamDat[Menu_Ua] = datatemp[ 0];//��ѹ����
		gParamDat[Menu_Ub] = datatemp[ 1];//��ѹ����
		gParamDat[Menu_A ] = datatemp[ 2];//����ֵ
		gParamDat[Menu_Aa] = datatemp[ 3];//��������
		gParamDat[Menu_Ab] = datatemp[ 4];//��������
		gParamDat[Menu_Ac] = datatemp[ 5];//�������
		gParamDat[Menu_Ca] = datatemp[ 6];//�¶�����
		gParamDat[Menu_Cb] = datatemp[ 7];//�¶�����
		gParamDat[Menu_Ha] = datatemp[ 8];//ʪ������
		gParamDat[Menu_Hb] = datatemp[ 9];//ʪ������
		gParamDat[Menu_Pa] = datatemp[10];//ѹ������
		gParamDat[Menu_Pb] = datatemp[11];//ѹ������
		gParamDat[Menu_Fa] = datatemp[12];//��������
		gParamDat[Menu_Fb] = datatemp[13];//��������
        printf("��ѹ����:%2d.��ѹ����:%2d.\r\n",gParamDat[Menu_Ua],gParamDat[Menu_Ub]);
        printf("��������:%2d.��������:%2d.��������:%3d.�������:%2d.\r\n",gParamDat[Menu_Aa],gParamDat[Menu_Ab],gParamDat[Menu_A],gParamDat[Menu_Ac]);
        printf("�¶�����:%2d.�¶�����:%2d.ʪ������:%2d.ʪ������:%2d.\r\n",gParamDat[Menu_Ca],gParamDat[Menu_Cb],gParamDat[Menu_Ha],gParamDat[Menu_Hb]);
        printf("ѹ������:%2d.ѹ������:%2d.��������:%2d.��������:%2d.\r\n",gParamDat[Menu_Pa],gParamDat[Menu_Pb],gParamDat[Menu_Fa],gParamDat[Menu_Fb]);
	}
}

void Write_Flash_Dat(void)
{
	u16 datatemp[20]={0};
	datatemp[ 0] = gParamDat[Menu_Ua];//��ѹ����
	datatemp[ 1] = gParamDat[Menu_Ub];//��ѹ����
	datatemp[ 2] = gParamDat[Menu_A ];//����ֵ
	datatemp[ 3] = gParamDat[Menu_Aa];//��������
	datatemp[ 4] = gParamDat[Menu_Ab];//��������
	datatemp[ 5] = gParamDat[Menu_Ac];//�������
	datatemp[ 6] = gParamDat[Menu_Ca];//�¶�����
	datatemp[ 7] = gParamDat[Menu_Cb];//�¶�����
	datatemp[ 8] = gParamDat[Menu_Ha];//ʪ������
	datatemp[ 9] = gParamDat[Menu_Hb];//ʪ������
	datatemp[10] = gParamDat[Menu_Pa];//ѹ������
	datatemp[11] = gParamDat[Menu_Pb];//ѹ������
	datatemp[12] = gParamDat[Menu_Fa];//��������
	datatemp[13] = gParamDat[Menu_Fb];//��������
	datatemp[18] = 0xAA;	//ʹ�ñ�־
	STMFLASH_WriteHword(FLASH_SAVE_ADD,(u16 *)datatemp,20);
}



