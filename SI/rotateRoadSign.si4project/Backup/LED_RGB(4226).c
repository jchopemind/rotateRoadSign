
/************************************************
修改日期:2017/10/9
版本：V1.0
版权所有，盗版必究。
Copyright(C) 西南科技大学制造学院机电控制实验室
All rights reserved	
************************************************/
#include "LED_RGB.h"
#include "string.h"
//#include "usart.h"	
#include "delay.h"
#include "stdint.h"
#include "hc08.h"     
#include "cJSON.h"
#include "spi_flash.h"
//#include "uart4.h"
//#include "uart5.h"
#include "stdlib.h"
#include "malloc.h"
#include <includes.h>


extern OS_SEM led_sem;
u8 LORA_Send_Flag = 0;

TRoot* root=NULL;
PChapter chapter=NULL;
PRegion region=NULL;
PLeaf leaf=NULL;
PWindows wins=NULL;
DWORD BufferSeek=0;


BYTE CRCTableHi[256]={0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                      0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

BYTE CRCTableLo[256]={0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
                      0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
                      0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
                      0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
                      0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
                      0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
                      0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
                      0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
                      0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
                      0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
                      0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
                      0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
                      0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
                      0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
                      0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
                      0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};


//BYTE PC_Data[384];
//生成播放数据起始段  节目数据头部
void MakeRoot(BYTE* buffer, int colormode)
{
  DWORD size;
  root=(TRoot *)buffer;

  size=sizeof(TRoot);
  memset(root, 0, size);
  root->id=0x22; //掉电后数据丢失  0x22 掉电不丢失
  root->color=(WORD)colormode;
  root->count=0;
  root->survive=0xffffffff;
  root->Reserved=0;
  root->size=size;
  BufferSeek=size;
}

//添加节目
//time  节目播放时间（ms）
//wait  是否等待节目里的内容播完在播放下一节节目
void AddChapter(DWORD time, WORD wait)
{
  DWORD size;
  chapter=(PChapter)((DWORD)root+BufferSeek);

  size=sizeof(TChapter);
  memset((BYTE*)chapter, 0, size);
  chapter->id=0x3f;
  chapter->time=time;
  chapter->wait=wait;
  chapter->kind=0;
  chapter->size=size;
  root->count++;
  root->size+=size;
  BufferSeek+=size;
}



//指定显示区域
void AddRegion(long left, long top, long width, long height)
{
  DWORD size;
  region=(PRegion)((DWORD)root+BufferSeek); 

  size=sizeof(TRegion);
  memset((BYTE*)region, 0, size);
  region->id=0x3e;
  region->rect.left=left;
  region->rect.top=top;
  region->rect.right=left+width;
  region->rect.bottom=top+height;
  region->size=size;
  if (chapter) chapter->count++;
  if (chapter) chapter->size+=size;
  root->size+=size;
  BufferSeek+=size;
}


//添加页面
void AddLeaf(DWORD time, WORD wait)
{
  DWORD size;
  leaf=(PLeaf)((DWORD)root+BufferSeek);

  size=sizeof(TLeaf);
  memset((BYTE*)leaf, 0, size);
  leaf->id=0x30;
  leaf->time=time;
  leaf->wait=wait;
  leaf->size=size;
  if (region) region->count++;
  if (region) region->size+=size;
  if (chapter) chapter->size+=size;
  root->size+=size;
  BufferSeek+=size;
}

//添加文字显示
//  left,top,width,height	????????X??,???Y??,??,??
void AddStrings(long left, long top, long width, long height, long transparent)
{
  DWORD size;
  wins=(PWindows)((DWORD)root+BufferSeek);

  size=sizeof(TWindows);
  memset((BYTE*)wins, 0, size);
  wins->object.id=0x3c;
  wins->object.transparent=(BYTE)transparent;
  wins->object.rect.left=left;
  wins->object.rect.top=top;
  wins->object.rect.right=left+width;
  wins->object.rect.bottom=top+height;
  wins->object.size=size;
  if (leaf) leaf->count++;
  if (leaf) leaf->size+=size;
  if (region) region->size+=size;
  if (chapter) chapter->size+=size;
  root->size+=size;
  BufferSeek+=size;
}

//添加文字显示
//  str			显示的文字内容
//  fontset		显示的字体,=0为16点阵,=1为24点阵
//  color		文字的显示颜色
//  inmethod    引入方式
//  inspeed     引入速度
//  outmethod   引出方式
//  outspeed    引出速度
//  stopmethod  停留方式
//  stopspeed   停留速度
//  stoptime    停留时间（ms）
void AddChildString(char* string, long fontset, long color, long inmethod, long inspeed, long outmethod, long outspeed, long stopmethod, long stopspeed, long stoptime)
{
  DWORD size;
  PString str=(PString)((DWORD)root+BufferSeek);

  size=sizeof(TString)+strlen(string)+1;
  while ((size&0x3)>0) size++;

  memset((BYTE*)str, 0, size);
  str->id=0x33;
  str->inMethod=(BYTE)inmethod;
  str->inSpeed=(BYTE)inspeed;
  str->outMethod=(BYTE)outmethod;
  str->outSpeed=(BYTE)outspeed;
  str->stopMethod=(BYTE)stopmethod;
  str->stopSpeed=(BYTE)stopspeed;
  str->stoptime=stoptime;
  str->fontset=(WORD)fontset;
  str->color=color;
  strcpy((char*)((DWORD)str+sizeof(TString)), string);
  str->size=size;
  if (wins) wins->count++;
  if (wins) wins->object.size+=size;
  if (leaf) leaf->size+=size;
  if (region) region->size+=size;
  if (chapter) chapter->size+=size;
  root->size+=size;
  BufferSeek+=size;
}



WORD Pack(BYTE *ibuf,BYTE *obuf,WORD size)
{
  WORD i,c=0;
  BYTE r,CRCHi=0xFF,CRCLo=0xFF;
  obuf[0]=PKP_LEADER;
  for (i=0; i<size; i++)
  {
    switch (ibuf[i]){
      case PKP_LEADER:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_LEADER+1;
	    break;
      case PKP_SUFFIX:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_SUFFIX+1;
	    break;
      case PKP_FILLCH:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_FILLCH+1;
	    break;
      default:
	    obuf[++c]=ibuf[i];
	    break;
    }
    r=ibuf[i]^CRCHi;
    CRCHi=CRCLo^CRCTableHi[r];
    CRCLo=CRCTableLo[r];
  }

  switch(CRCLo)
  {
    case PKP_LEADER:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_LEADER+1;
      break;
    case PKP_SUFFIX:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_SUFFIX+1;
      break;
    case PKP_FILLCH:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_FILLCH+1;
      break;
    default:
      obuf[++c]=CRCLo;
      break;
  }

  switch(CRCHi)
  {
    case PKP_LEADER:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_LEADER+1;
      break;
    case PKP_SUFFIX:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_SUFFIX+1;
      break;
    case PKP_FILLCH:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_FILLCH+1;
      break;
    default:
      obuf[++c]=CRCHi;
      break;
  }

  obuf[++c]=PKP_SUFFIX;
  return c+1;
}

//生成起始包数据
DWORD DoBeginPacket(BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len=0;
	DWORD i;

	header.Command=PKC_BEGIN;
	header.dstAddr=dstAddr;
	header.SerialNo=0;
	header.srcAddr=0;
	
	
	
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));
    printf("Begin Packet:\n");
	for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");
    printf("\n");

	
	return len;
}
DWORD GetDataPacketCount(BYTE* ibuf)
{
    PRoot root=(PRoot)ibuf;
    DWORD K = root->size/PKP_LENGTH;
    if ((root->size%PKP_LENGTH) > 0) return K + 1;
    else return K;
}
WORD PackEx(BYTE *cbuf,WORD csize,BYTE *ibuf,BYTE *obuf,WORD size)
{
  WORD i,c=0;
  BYTE r,CRCHi=0xFF,CRCLo=0xFF;
  obuf[0]=PKP_LEADER;
  for (i=0; i<csize; i++)
  {
    switch (cbuf[i]){
      case PKP_LEADER:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_LEADER+1;
	    break;
      case PKP_SUFFIX:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_SUFFIX+1;
	    break;
      case PKP_FILLCH:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_FILLCH+1;
	    break;
      default:
	    obuf[++c]=cbuf[i];
	    break;
    }
    r=cbuf[i]^CRCHi;
    CRCHi=CRCLo^CRCTableHi[r];
    CRCLo=CRCTableLo[r];
  }

  for (i=0; i<size; i++)
  {
    switch (ibuf[i]){
      case PKP_LEADER:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_LEADER+1;
	    break;
      case PKP_SUFFIX:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_SUFFIX+1;
	    break;
      case PKP_FILLCH:
	    obuf[++c]=PKP_FILLCH;
	    obuf[++c]=PKP_FILLCH+1;
	    break;
      default:
	    obuf[++c]=ibuf[i];
	    break;
    }
    r=ibuf[i]^CRCHi;
    CRCHi=CRCLo^CRCTableHi[r];
    CRCLo=CRCTableLo[r];
  }

  switch(CRCLo)
  {
    case PKP_LEADER:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_LEADER+1;
      break;
    case PKP_SUFFIX:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_SUFFIX+1;
      break;
    case PKP_FILLCH:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_FILLCH+1;
      break;
    default:
      obuf[++c]=CRCLo;
      break;
  }

  switch(CRCHi)
  {
    case PKP_LEADER:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_LEADER+1;
      break;
    case PKP_SUFFIX:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_SUFFIX+1;
      break;
    case PKP_FILLCH:
      obuf[++c]=PKP_FILLCH;
      obuf[++c]=PKP_FILLCH+1;
      break;
    default:
      obuf[++c]=CRCHi;
      break;
  }

  obuf[++c]=PKP_SUFFIX;
  return c+1;
}
DWORD DoDataPacket(BYTE* ibuf, DWORD index, BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len, I;
	BYTE* p=obuf;
	PRoot root=(PRoot)ibuf;
	DWORD i;

	header.Command=PKC_DATA;
	header.dstAddr=dstAddr;
	header.srcAddr=0;

	I=PKP_LENGTH*(index-1);
	header.SerialNo=index;
	if (I<root->size)
	{
		if (root->size-I>PKP_LENGTH)
		{
			len=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, PKP_LENGTH);
		}
		else
		{
			len=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, (WORD)(root->size-I));
		}
	}

//    printf("One Data Packet:\n");
//    for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
//    printf("\n");
//    printf("\n");

	return len;
}
//生成结束包数据
DWORD DoEndPacket(BYTE* obuf, DWORD index, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len=0;
	DWORD i;

	header.Command=PKC_END;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	

   
	header.SerialNo=index;
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));
	printf("End Packet:\n");
	for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");
    printf("\n");
	return len;
}

//发送数据


//void com_send(BYTE* stream)
//{
//  DWORD size;
//  DWORD i, K;
//  //TDeviceInfo dev;
////  uart_Initialize(com_port);

//  //发送起始包
////	USART_RX_STA=0;	
//  size=DoBeginPacket(buffer, 0);
////	UART2_PutStr_Num(buffer,size);
////		hc05send_size(buffer,size);
//	delay_ms(2000);
//	delay_ms(2000);
//	delay_ms(2000);	
//	//发送数据包

//	  K=GetDataPacketCount(stream);
//	 for (i=1; i<=K; i++)
//  {
//    size=DoDataPacket(stream, i, buffer, 0);
////    uart_Write(buffer, size, NULL);
////		USART_RX_STA=0;	
////		UART2_PutStr_Num(buffer,size);

//	  delay_ms(2000);
//		delay_ms(2000);
////    size=uart_Read(buffer, 6144, &dev);
////    if (size>0) printf("==>???%d????\n\n", i); else printf("==>???%d????\n\n", i);
//  }
//	
//	//发送结束包
////					USART_RX_STA=0;	
//	size=DoEndPacket(buffer, K+1, 0);
////  uart_Write(buffer, size, NULL);
////  Sleep(100);
////			UART2_PutStr_Num(buffer,size);

//	 delay_ms(2000);
////  size=uart_Read(buffer, 6144, &dev);
////  if (size>0) printf("==>???????\n"); else printf("==>???????\n");
////	
//	
//}


//	BYTE buffer[1000];
//void HCO5_send(BYTE* data)
//{
//	

//  DWORD size;
//  DWORD i, K;
//  //TDeviceInfo dev;
////  uart_Initialize(com_port);

//  //发送起始包
////	USART_RX_STA=0;	
//  size=DoBeginPacket(buffer, 0);
//	
//	hc05send_size(buffer,size);
//	
//	
////	 while(1);
//	 delay_ms(100);
//	//delay_ms(2000);
//	
////	delay_ms(2000);	
////	delay_ms(1000);	
////		delay_ms(1000);	
////			delay_ms(1000);	
////				delay_ms(1000);	
////					delay_ms(1000);	
//	//发送数据包

//	K=GetDataPacketCount(data);
//	 for (i=1; i<=K; i++)
//  {
//    size=DoDataPacket(data, i, buffer, 0);
////    uart_Write(buffer, size, NULL);
////		USART_RX_STA=0;	
//		hc05send_size(buffer,size);
////	  delay_ms(200);
////			delay_ms(2000);	
//////		delay_ms(1000);
////			delay_ms(1000);
////			delay_ms(1000);
////			delay_ms(1000);
//		
//		//		delay_ms(2000);
////    size=uart_Read(buffer, 6144, &dev);
////    if (size>0) printf("==>???%d????\n\n", i); else printf("==>???%d????\n\n", i);
//  }
//		 delay_ms(100);
//	//发送结束包
////					USART_RX_STA=0;	
//	size=DoEndPacket(buffer, K+1, 0);
////  uart_Write(buffer, size, NULL);
////  Sleep(100);
//		hc05send_size(buffer,size);
////	 delay_ms(200);
////		delay_ms(1000);
////	delay_ms(1000);
////	delay_ms(1000);
////	delay_ms(1000);
////	delay_ms(1000);	
//////  size=uart_Read(buffer, 6144, &dev);
////  if (size>0) printf("==>???????\n"); else printf("==>???????\n");
////	
//	
//}

//	BYTE buffer_1[1000];
//void HCO5_1send(BYTE* data1)
//{
//	

//  DWORD size;
//  DWORD i, K;

//  size=DoBeginPacket(buffer_1, 0);
//	
//	hc05send1_size(buffer_1,size);
////		 delay_ms(100);

//	K=GetDataPacketCount(data1);
//	 for (i=1; i<=K; i++)
//  {
//    size=DoDataPacket(data1, i, buffer_1, 0);

//		hc05send1_size(buffer_1,size);

//  }
////		 delay_ms(100);
//	size=DoEndPacket(buffer_1, K+1, 0);

//		hc05send1_size(buffer_1,size);
//	
//}


//BYTE buffer_2[1000];
//void HCO5_2send(BYTE* data2)
//{
//	

//  DWORD size;
//  DWORD i, K;

//  size=DoBeginPacket(buffer_2, 0);
//	
//	hc05send2_size(buffer_2,size);
////		 delay_ms(100);

//	K=GetDataPacketCount(data2);
//	 for (i=1; i<=K; i++)
//  {
//    size=DoDataPacket(data2, i, buffer_2, 0);

//		hc05send2_size(buffer_2,size);

//  }
////		 delay_ms(100);
//	size=DoEndPacket(buffer_2, K+1, 0);

//		hc05send2_size(buffer_2,size);
//	
//}

BYTE *stream;


//添加图片显示
//  left,top,width,height	????????X??,???Y??,??,??
void AddWindows(long left, long top, long width, long height, long transparent)
{
	DWORD size;
	wins=(PWindows)((DWORD)root+BufferSeek);

	size=sizeof(TWindows);
	memset((BYTE*)wins, 0, size);
	wins->object.id=ID_WINDOWS;
	wins->object.transparent=(BYTE)transparent;
	wins->object.rect.left=left;
	wins->object.rect.top=top;
	wins->object.rect.right=left+width;
	wins->object.rect.bottom=top+height;
	wins->object.size=size;
	if (leaf) leaf->count++;
	if (leaf) leaf->size+=size;
	if (region) region->size+=size;
	if (chapter) chapter->size+=size;
	root->size+=size;
	BufferSeek+=size;
}

//??????--?????
//  obuffer?????????
//  ibuffer??????,????????????(RGB)3???
//  cx,cy???????

DWORD MakePixelsRGB(BYTE* obuffer, BYTE* ibuffer, long cx, long cy)
{
  long x, y;
  PRGB rgb;
  DWORD shift;
  PPixels pixels=(PPixels)obuffer;
	
	BYTE* pic=(obuffer+sizeof(TPixels));
  pixels->id=ID_PIXELS;
  pixels->bits=BITMAP_RGB;
  pixels->width=cx;
  pixels->height=((cy+0x1f)&(~0x1f));
  pixels->size=sizeof(TPixels)+pixels->width*(pixels->height>>3)*3;	
	memcpy(pic, ibuffer, pixels->width*(pixels->height>>3)*3);
  return pixels->size;
}
DWORD MakePixels(BYTE* obuffer, BYTE* ibuffer, long cx, long cy)
{
  switch(root->color){
    case LED_THREE:
      return MakePixelsRGB(obuffer, ibuffer, cx, cy);
    default:
      return 0;
  }
}

//添加图片对象
//  inmethod    引入方式
//  inspeed     引入速度
//  outmethod  引出方式
//  outspeed    引出速度
//  stopmethod  停留方式
//  stopspeed  停留速度
//  stoptime   停留时间（毫秒）
void AddChildWindow(BYTE* dibbuffer, long cx, long cy, long inmethod, long inspeed, long outmethod, long outspeed, long stopmethod, long stopspeed, long stoptime)
{
  PWindow win=(PWindow)((DWORD)root+BufferSeek);

  memset((BYTE*)win, 0, sizeof(TWindow));
  win->id=ID_WINDOW;
  win->inMethod=(BYTE)inmethod;
  win->inSpeed=(BYTE)inspeed;
  win->outMethod=(BYTE)outmethod;
  win->outSpeed=(BYTE)outspeed;
  win->stopMethod=(BYTE)stopmethod;
  win->stopSpeed=(BYTE)stopspeed;
  win->stoptime=stoptime;
  win->size=sizeof(TWindow);

  win->size+=MakePixels((BYTE*)((DWORD)win+sizeof(TWindow)), dibbuffer, cx, cy);

  if (wins) wins->count++;
  if (wins) wins->object.size+=win->size;
  if (leaf) leaf->size+=win->size;
  if (region) region->size+=win->size;
  if (chapter) chapter->size+=win->size;
  root->size+=win->size;
  BufferSeek+=win->size;
}



//BYTE PC_Data[384];
//BYTE PC_Data2[384];

long movestyle_1=0;
long movestyle_2=0;
long movestyle_3=0;
long text_color=RGB(255,255,255);

u8 Pic_Head_Read[3], b[]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x01,0xC0,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x01,0x90,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0x3F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0x7F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x1E,0x70,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x07,0xE0,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xE7,0xCF,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x98,0xF3,0xDF,0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0xF9,0x9F,0x7B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCE,0x79,0xBC,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x3C,0x38,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x1C,0x78,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x3C,0x38,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCE,0x79,0xBC,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9C,0xF3,0x9D,0x71,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9C,0xF3,0xDF,0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xE7,0xCE,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x07,0xEF,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x7E,0x77,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0x73,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0x3B,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC1,0xB9,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE3,0x9D,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xEF,0xDE,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0xCE,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xE7,0x00};
u16 Pic_Data_num_Read=0; //图片数据大小
//u8 Pic_Id_Num_Read=0; //图片ID号
u8 *pic_p_Read;

//u8 test_icon[]={0x00,0xF0,0x0F,0x00,0x00,0xF0,0x0F,0x00,0x00,0xF0,0x0F,0x00,0x00,0xFE,0x7F,0x00,0x00,0xFE,0x7F,0x00,0x00,0xFE,0x7F,0x00,0x80,0x07,0xE0,0x01,0x80,0x07,0xE0,0x01,0x80,0x07,0xE0,0x01,0xC0,0x01,0x80,0x03,0xC0,0x01,0x80,0x03,0xC0,0x01,0x80,0x03,0x60,0x00,0x00,0x06,0x60,0x00,0x00,0x06,0x60,0x00,0x00,0x06,0x30,0x0E,0x00,0x0C,0x30,0x0E,0x00,0x0C,0x30,0x0E,0x00,0x0C,0x18,0x1F,0x00,0x18,0x18,0x1F,0x00,0x18,0x18,0x1F,0x00,0x18,0x0C,0x1F,0x00,0x30,0x0C,0x1F,0x00,0x30,0x0C,0x1F,0x00,0x30,0x0C,0x1F,0x00,0x30,0x0C,0x1F,0x00,0x30,0x0C,0x1F,0x00,0x30,0x06,0x1F,0x00,0x60,0x06,0x1F,0x00,0x60,0x06,0x1F,0x00,0x60,0x06,0x1F,0x00,0x60,0x06,0x1F,0x00,0x60,0x06,0x1F,0x00,0x60,0x02,0x1F,0x00,0x40,0x02,0x1F,0x00,0x40,0x02,0x1F,0x00,0x40,0x03,0x3E,0x00,0xC0,0x03,0x3E,0x00,0xC0,0x03,0x3E,0x00,0xC0,0x03,0x7C,0x00,0xC0,0x03,0x7C,0x00,0xC0,0x03,0x7C,0x00,0xC0,0x03,0xF8,0xCF,0xC1,0x03,0xF8,0xCF,0xC1,0x03,0xF8,0xCF,0xC1,0x03,0xF0,0xEF,0xC1,0x03,0xF0,0xEF,0xC1,0x03,0xF0,0xEF,0xC1,0x03,0xE0,0xEF,0xC1,0x03,0xE0,0xEF,0xC1,0x03,0xE0,0xEF,0xC1,0x03,0xE0,0x87,0xC0,0x03,0xE0,0x87,0xC0,0x03,0xE0,0x87,0xC0,0x03,0xC0,0x07,0xC0,0x03,0xC0,0x07,0xC0,0x03,0xC0,0x07,0xC0,0x03,0x80,0x0F,0xC0,0x03,0x80,0x0F,0xC0,0x03,0x80,0x0F,0xC0,0x02,0x00,0x1F,0x40,0x02,0x00,0x1F,0x40,0x02,0x00,0x1F,0x40,0x06,0x00,0x3E,0x60,0x06,0x00,0x3E,0x60,0x06,0x00,0x3E,0x60,0x06,0x00,0x3E,0x60,0x06,0x00,0x3E,0x60,0x06,0x00,0x3E,0x60,0x0C,0x00,0x3C,0x30,0x0C,0x00,0x3C,0x30,0x0C,0x00,0x3C,0x30,0x0C,0x00,0x3C,0x30,0x0C,0x00,0x3C,0x30,0x0C,0x00,0x3C,0x30,0x18,0x00,0x3C,0x18,0x18,0x00,0x3C,0x18,0x18,0x00,0x3C,0x18,0x30,0x00,0x3C,0x0C,0x30,0x00,0x3C,0x0C,0x30,0x00,0x3C,0x0C,0x60,0x00,0x1C,0x06,0x60,0x00,0x1C,0x06,0x60,0x00,0x1C,0x06,0xC0,0x01,0x80,0x03,0xC0,0x01,0x80,0x03,0xC0,0x01,0x80,0x03,0x80,0x07,0xE0,0x01,0x80,0x07,0xE0,0x01,0x80,0x07,0xE0,0x01,0x00,0xFE,0x7F,0x00,0x00,0xFE,0x7F,0x00,0x00,0xF0,0x0F,0x00,0x00,0xF0,0x0F,0x00,0x00,0xF0,0x0F,0x00};
 void demo_string(u8 LED_ID)
{
	u8 word1_len=0,word2_len=0,word3_len=0;
	u32 i;
	OS_ERR err;
	
	Style_Switch();
	
	stream = (BYTE *)mymalloc(SRAMEX, 7000);
	pic_p_Read = mymalloc(SRAMEX, 3500);
	while(! stream || !pic_p_Read){
		printf("LED_RGB -> stream申请内存失败\r\n");
	}
	
	MakeRoot(stream, 3);    
	AddChapter(0x7fffffff, 0x01);  //等待节目播放完成
	AddRegion(0, 0,256, 64);
	AddLeaf(5000, 0x01);
	
	word1_len=strlen(sys_pra.word1);
	word2_len=strlen(sys_pra.word2);
	word3_len=strlen(sys_pra.word3);
	
//	sys_pra.time=5;				//停留时间test

	if(sys_pra.flag==0 | sys_pra.flag==2){
		switch(sys_pra.showstyle){
		case 0 :
				 AddStrings((176-6*word3_len), 0, 256, 32, 1); 	 //第一行文字（距离左边，距离上边，显示宽度，显示高度）
				 AddChildString(sys_pra.word3, FONT_SET_24, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 AddStrings((176-6*word3_len), 32, 256, 32, 1);  //第二行文字
				 AddChildString(sys_pra.word3, FONT_SET_24, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 break;
		case 1 :	
				 AddStrings((176-4*word3_len), 0, 256, 16, 1); 	 //第一行文字（距离左边，距离上边，显示宽度，显示高度）
				 AddChildString(sys_pra.word3, FONT_SET_16, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 AddStrings((176-4*word1_len), 16, 256, 16, 1);  //第二行文字
				 AddChildString(sys_pra.word1, FONT_SET_16, text_color, movestyle_1, 0, 1, 0, 0, 0, sys_pra.time*1000); 
				 AddStrings((176-4*word3_len), 32, 256, 16, 1);  //第三行文字
				 AddChildString(sys_pra.word3, FONT_SET_16, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 AddStrings((176-4*word2_len), 48, 256, 16, 1);  //第四行文字
				 AddChildString(sys_pra.word2, FONT_SET_16, text_color, movestyle_2, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 break;
		case 2 :
				 AddStrings((176-6*word3_len), 0, 256, 32, 1); 	 //第一行文字（距离左边，距离上边，显示宽度，显示高度）
				 AddChildString(sys_pra.word3, FONT_SET_24, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 AddStrings((176-4*word1_len), 32, 256, 16, 1);  //第二行文字
				 AddChildString(sys_pra.word1, FONT_SET_16, text_color, movestyle_1, 0, 1, 0, 0, 0, sys_pra.time*1000); 
				 AddStrings((176-4*word2_len), 48, 256, 16, 1);  //第三行文字
				 AddChildString(sys_pra.word2, FONT_SET_16, text_color, movestyle_2, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 break;
		case 3 :
				 AddStrings((176-4*word1_len), 0, 256, 16, 1); 	 //第一行文字（距离左边，距离上边，显示宽度，显示高度）
				 AddChildString(sys_pra.word1, FONT_SET_16, text_color, movestyle_1, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 AddStrings((176-4*word2_len), 16, 256, 16, 1);  //第二行文字
				 AddChildString(sys_pra.word2, FONT_SET_16, text_color, movestyle_2, 0, 1, 0, 0, 0, sys_pra.time*1000); 
				 AddStrings((176-6*word3_len), 32, 256, 32, 1);  //第三行文字
				 AddChildString(sys_pra.word3, FONT_SET_24, text_color, movestyle_3, 0, 1, 0, 0, 0, sys_pra.time*1000);
				 break;
		}
	}

	if(sys_pra.flag==2 || sys_pra.flag==1){
		if(sys_pra.img1id!=0){
			sys_pra.img2id=sys_pra.img1id+10;
		}		
		
		sys_pra.img2id=1;						//test_flash_img_icon_display
//***********第一幅图************
		  W25QXX_Read(Pic_Head_Read,sys_pra.img2id*Flash_Signal_Size,3);   //根据ID号 从Falsh中读取 放入到图片数组中
		for(i = 0; i < 3; i ++)
		{
			printf("%02x", Pic_Head_Read[i]);			
		}
			Pic_Data_num_Read=Pic_Head_Read[2]*256+Pic_Head_Read[1];
//		  pic_p_Read=(u8 *)malloc(Pic_Data_len * sizeof(u8));			
		  W25QXX_Read(pic_p_Read,sys_pra.img2id*Flash_Signal_Size+3,Pic_Data_num_Read);
		
		for(i = 0; i < Pic_Data_num_Read; i ++)
		{
			printf("%02x", pic_p_Read[i]);			
		}
   if(Pic_Data_num_Read<400)   //判断图片格式是32*32  还是192*32
	 {
		 	AddWindows(64, 0, 32, 32, 1);
			AddChildWindow(pic_p_Read, 32, 32, 1, 0, 1, 0, 0, 0, 0);//	 
	 }
	 else  
	 {
			AddWindows(64, 0, 192, 32, 1);
			AddChildWindow(pic_p_Read, 192, 32, 1, 0, 1, 0, 0, 0, 0);//	 
	 }
 
				memset(Pic_Head_Read,'\0',sizeof(Pic_Head_Read));
//				ex_free(pic_p_Read);
//				pic_p_Read = NULL;		
	 
//***********第二幅图*********** 
			W25QXX_Read(Pic_Head_Read,sys_pra.img2id*Flash_Signal_Size,3);   //根据ID号 从Falsh中读取 放入到图片数组中
			Pic_Data_num_Read=Pic_Head_Read[2]*256+Pic_Head_Read[1];
//			pic_p_Read=(u8 *)malloc(Pic_Data_num_Read * sizeof(u8));			
		  W25QXX_Read(pic_p_Read,sys_pra.img2id*Flash_Signal_Size+3,Pic_Data_num_Read);											
			 
   if(Pic_Data_num_Read<400)   //判断图片格式是32*32  还是192*32
	 {	
		  AddWindows(64, 32, 32, 32, 1);
			AddChildWindow(pic_p_Read, 32, 32, 1, 0, 1, 0, 0, 0, 0);//
	 }
	 else  
	 {		 
		 		AddWindows(64, 32,192, 32, 1);
				AddChildWindow(pic_p_Read, 192, 32, 1, 0, 1, 0, 0, 0, 0);//
	 }

				memset(Pic_Head_Read,'\0',sizeof(Pic_Head_Read));
				ex_free(pic_p_Read);
				pic_p_Read = NULL;
 }	 
	 macPC_Usart ( "Data_Len:%d\r\n",(u16)BufferSeek);  
//	com_send(stream);
		printf("-----\r\n");
	
 
	
 	 switch(LED_ID){
		 case 1:			 
		 case 2:		 
		 case 3:	
//			 OSSemPost (&led_sem, OS_OPT_POST_ALL, &err);
//					if(OS_ERR_NONE  != err) {
//						printf(" lora_task => OSSemPost err!\r\n");
//		
//					}
				LORA_Send_Flag = 1;
				printf("%d", LORA_Send_Flag);
		 break; 
	 }
	 
}


//协议显示内容选择函数
void Style_Switch(void)
{
				
 	switch(sys_pra.movestyle1)
	{
		case 0:		movestyle_1=1;break;
		case 1:		movestyle_1=2;break;
		case 2:		movestyle_1=4;break;
		case 3:		movestyle_1=11;break;
		case 4:		movestyle_1=14;break;
		case 5:		movestyle_1=15;break;
		case 6:		movestyle_1=21;break;
		case 7:		movestyle_1=23;break;
		case 8:		movestyle_1=24;break;		
	}
	
		switch(sys_pra.movestyle2)
	{
		case 0:		movestyle_2=1;break;
		case 1:		movestyle_2=2;break;
		case 2:		movestyle_2=4;break;
		case 3:		movestyle_2=11;break;
		case 4:		movestyle_2=14;break;
		case 5:		movestyle_2=15;break;
		case 6:		movestyle_2=21;break;
		case 7:		movestyle_2=23;break;
		case 8:		movestyle_2=24;break;		
	}
	
		switch(sys_pra.movestyle3)
	{
		case 0:		movestyle_3=1;break;
		case 1:		movestyle_3=2;break;
		case 2:		movestyle_3=4;break;
		case 3:		movestyle_3=11;break;
		case 4:		movestyle_3=14;break;
		case 5:		movestyle_3=15;break;
		case 6:		movestyle_3=21;break;
		case 7:		movestyle_3=23;break;
		case 8:		movestyle_3=24;break;		
	}
	
			switch(sys_pra.textcolor)
	{
		case 0:		text_color=RGB(0,0,0);break;
		case 1:		text_color=RGB(0,0,255);break;
		case 2:		text_color=RGB(0,255,0);break;
		case 3:		text_color=RGB(0,255,255);break;
		case 4:		text_color=RGB(255,0,0);break;
		case 5:		text_color=RGB(255,0,255);break;
		case 6:		text_color=RGB(255,255,0);break;
		case 7:		text_color=RGB(255,255,255);break;		
	}
	
}

