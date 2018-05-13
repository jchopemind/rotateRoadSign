#ifndef __LED_RGB_H
#define __LED_RGB_H

#pragma pack (1)

//数据类型定义
typedef unsigned int       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef DWORD   			COLORREF;
//#define   NULL ((void *)0)


#define FONT_SET_16           0x0
#define FONT_SET_24           0x1


#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

typedef struct _RECT{
  int left;
  int top;
  int right;
  int bottom;
}TRect, *PRect;


typedef struct TIMESTAMP{
  int time;
  int date;
}TTimeStamp, *PTimeStamp;


//数据流头部
typedef struct ROOT{
  WORD  id;              //?????
  WORD  color;           //??????????
  DWORD size;            //????????(??????????????????)
  DWORD count;           //?????
  int  survive;         //??????
  WORD  flag;            //??
  WORD  Reserved;
}TRoot,*PRoot;



//章节
typedef struct CHAPTER{
  WORD  id;             //??,??????
  WORD  no;             //????(??????,???????)
  WORD  wait;           //????????(=1??; =0?????)
  WORD  repeat;         //??????
  WORD  count;          //??????
  WORD  Reserved;
  DWORD time; 
  DWORD size;           //????????(??????????????????)
  WORD  kind;           //??????0=????;1=?????????;3=????????????
  WORD  week;           //?????????
  TTimeStamp fromtime;  //开始时间
  TTimeStamp totime;    //借宿时间
}TChapter, *PChapter;


//区域
typedef struct REGION{
  WORD  id;             //??,??????
  WORD  no;             //??????(????????,???????)
  WORD  over;           //????????(?mode??????????,???????????????)
  WORD  count;          //????
  DWORD size;           //??????????(??????????????????)
  TRect rect;           //????????
  WORD  Reserved1;
  WORD  Reserved2;
}TRegion, *PRegion;

//页面数据结构体
typedef struct LEAF{
  WORD  id;             //????(??????,???????)
  WORD  no;
  WORD  wait;           //????????(=1??; =0?????)
  WORD  serial;         //????????
  WORD  count;          //????
  WORD  Reserved;
  DWORD time;           //??????
  DWORD size;           //????????(??????????????????)
  WORD  Reserved1;
  WORD  Reserved2;
}TLeaf, *PLeaf;

//对象通用属性结构体
typedef struct OBJECT{
  WORD  id;             //对象标识
  BYTE  transparent;    //是否透明
  BYTE  border;
  DWORD size;           //对象字节数
  TRect rect;           //对象的显示区域
}TObject, *PObject;

//多播放对象数据结构，后面跟多个TWindows对象或者多个TString对象
typedef struct WINDOWS{
  TObject     object;     //对象的一般定义
  BYTE        method;     //动作方式
  BYTE        speed;      //动作速度
  WORD        count;      //包含的WINDOWS数量
}TWindows;
typedef TWindows * PWindows;


//内码文字对象
typedef struct STRING{
  WORD        id;
  BYTE        inMethod;   //????
  BYTE        outMethod;  //????
  BYTE        stopMethod; //????
  BYTE        reserved;
  WORD        inSpeed;    //????
  WORD        outSpeed;   //????
  WORD        stopSpeed;  //????(??????,??????)
  DWORD       stoptime;   //????
  int        stopx;      //????
  DWORD       size;       //????
  DWORD       color;      //????
  WORD        fontset;    //???(???? 0=16?? 1=24??)
  WORD        Reserved;
}TString;

typedef TString * PString;
//数据发送
#define  PKC_BEGIN   0
#define  PKC_END     1
#define  PKC_DATA    2

#define  PKP_LEADER  0x55
#define  PKP_SUFFIX  0xAA
#define  PKP_FILLCH  0xBB
#define  PKP_LENGTH  512
#define  PKP_MAX_LENGTH PKP_LENGTH*2+sizeof(TPKG_Header)*2

typedef struct DEVICEINFO{
  WORD  dev_id;
  WORD  port;
  DWORD ip;
}TDeviceInfo, *PDeviceInfo;

typedef struct PKG_HEADER {
	WORD   Command;
  BYTE    srcAddr;
  BYTE    dstAddr;
  DWORD  SerialNo;
}TPKG_Header, *PPKG_Header;


#define PLAY_MODE_NORMAL      0x00  //严格按照时间切换
#define PLAY_MODE_WAIT        0x01  //等待自身播放完成
#define ID_WINDOWS            0x3B

//????,????????TPixels
typedef struct WINDOW{
  WORD        id;
  BYTE        inMethod;   //????
  BYTE        outMethod;  //????
  BYTE        stopMethod; //????
  BYTE        reserved;
  WORD        inSpeed;    //????
  WORD        outSpeed;   //????
  WORD        stopSpeed;  //????(??????,??????)
  DWORD       stoptime;   //????
  int        stopx;      //????
  DWORD       size;       //????
}TWindow;

typedef TWindow * PWindow;
#define ID_WINDOW             0x31

//?????
#define LED_MONO			1
#define LED_DOUBLE			2
#define LED_THREE			3
#define LED_FULL			4

typedef struct _RGB{
  BYTE r;
  BYTE g;
  BYTE b;
}TRGB;
typedef TRGB * PRGB;


typedef struct PIXELS{
  WORD   id;            
  WORD   bits;          
  int   width;         
  int   height;        
  DWORD  size;          
}TPixels;

typedef TPixels * PPixels;

#pragma pack ()

#define ID_PIXELS             0x36
#define BITMAP_RGB            3


void Picture_Show(WORD ID);
//void demo_string(u8 LED_ID);
//void Style_Switch(void);
WORD Pack(BYTE *ibuf,BYTE *obuf,WORD size);
DWORD DoBeginPacket(BYTE* obuf, BYTE dstAddr);
DWORD GetDataPacketCount(BYTE* ibuf);
DWORD DoDataPacket(BYTE* ibuf, DWORD index, BYTE* obuf, BYTE dstAddr);
DWORD DoEndPacket(BYTE* obuf, DWORD index, BYTE dstAddr);
int show_string(int fd, int sw, int text_color, int movSty1, int movSty2, int movSty3, int sectime);
void HCO5_1send(int fd, BYTE* data1);

#endif

