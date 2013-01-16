%
%/* VXI-11 RPCL definitions. Taken from appendix C of the VXI-11 specfication.
% * See http://www.vxi.org 
% */

%#include <strings.h>
%#include <stdlib.h>
%
%/* VXI-11 flags */
%#define VXI11_FLAG_WAITLOCK         0x01
%#define VXI11_FLAG_ENDW             0x08
%#define VXI11_FLAG_TERMCHRSET       0x80
%
%/* VXI-11 device_docmd 'cmd' values */
%#define VXI11_DOCMD_SEND_COMMAND    0x020000
%#define VXI11_DOCMD_BUS_STATUS      0x020001
%#define VXI11_DOCMD_ATN_CONTROL     0x020002
%#define VXI11_DOCMD_REN_CONTROL     0x020003
%#define VXI11_DOCMD_PASS_CONTROL    0x020004
%#define VXI11_DOCMD_BUS_ADDRESS     0x02000A
%#define VXI11_DOCMD_IFC_CONTROL     0x020010
%
%/* VXI-11 device_docmd Bus Status values */
%#define VXI11_DOCMD_STAT_REMOTE     1
%#define VXI11_DOCMD_STAT_SRQ        2
%#define VXI11_DOCMD_STAT_NDAC       3
%#define VXI11_DOCMD_STAT_SYS_CTRLR  4
%#define VXI11_DOCMD_STAT_CTRLR_CHRG 5
%#define VXI11_DOCMD_STAT_TALKER     6
%#define VXI11_DOCMD_STAT_LISTENER   7
%#define VXI11_DOCMD_STAT_BUSADDR    8
%
%/* VXI-11 device_read 'reason' bits */
%#define VXI11_REASON_REQCNT 1   /* requested number of bytes read */
%#define VXI11_REASON_CHR    2   /* read terminated by eos character */
%#define VXI11_REASON_END    4   /* read terminated by EOI */
%
%/* VXI-11 errors */
%#define VXI11_ERR_SUCCESS   0
%#define VXI11_ERR_SYNTAX    1
%#define VXI11_ERR_NODEVICE  3
%#define VXI11_ERR_LINKINVAL 4
%#define VXI11_ERR_PARAMETER 5
%#define VXI11_ERR_NOCHAN    6
%#define VXI11_ERR_NOTSUPP   8
%#define VXI11_ERR_RESOURCES 9
%#define VXI11_ERR_LOCKED    11
%#define VXI11_ERR_NOLOCK    12
%#define VXI11_ERR_IOTIMEOUT 15
%#define VXI11_ERR_IOERROR   17
%#define VXI11_ERR_ADDRINVAL 21
%#define VXI11_ERR_ABORT     23
%#define VXI11_ERR_CHANEST   29

/* Types */
typedef long Device_Link;
enum Device_AddrFamily { /* used by interrupts */
   DEVICE_TCP,
   DEVICE_UDP
};
typedef long Device_Flags;

/* Error types */
typedef long Device_ErrorCode;
struct Device_Error {
   Device_ErrorCode error;
};

struct Create_LinkParms {
   long clientId; /* implementation specific value */
   bool lockDevice; /* attempt to lock the device */
   unsigned long lock_timeout; /* time to wait on a lock */
   string device<>; /* name of device */
};

struct Create_LinkResp {
   Device_ErrorCode error;
   Device_Link lid;
   unsigned short abortPort; /* for the abort RPC */
   unsigned long maxRecvSize; /* specifies max data size in bytes device will accept on a write */ 
};

struct Device_WriteParms { 
   Device_Link lid; /* link id from create_link */
   unsigned long io_timeout; /* time to wait for I/O */
   unsigned long lock_timeout; /* time to wait for lock */
   Device_Flags flags; opaque data<>; /* the data length and the data itself */
};

struct Device_WriteResp {
   Device_ErrorCode error;
   unsigned long size; /* Number of bytes written */ 
};

struct Device_ReadParms { 
   Device_Link lid; /* link id from create_link */
   unsigned long requestSize; /* Bytes requested */
   unsigned long io_timeout; /* time to wait for I/O */
   unsigned long lock_timeout; /* time to wait for lock */
   Device_Flags flags; 
   char termChar; /* valid if flags & termchrset */
};

struct Device_ReadResp {
   Device_ErrorCode error;
   long reason; /* Reason(s) read completed */
   opaque data<>; /* data.len and data.val */
};

struct Device_ReadStbResp {
   Device_ErrorCode error; /* error code */
   unsigned char stb; /* the returned status byte */
};

struct Device_GenericParms {
   Device_Link lid; /* Device_Link id from connect call */
   Device_Flags flags; /* flags with options */
   unsigned long lock_timeout; /* time to wait for lock */
   unsigned long io_timeout; /* time to wait for I/O */
};

struct Device_RemoteFunc {
   unsigned long hostAddr; /* Host servicing Interrupt */
   unsigned short hostPort; /* valid port # on client */
   unsigned long progNum; /* DEVICE_INTR */
   unsigned long progVers; /* DEVICE_INTR_VERSION */
   Device_AddrFamily progFamily; /* DEVICE_UDP | DEVICE_TCP */
};

struct Device_EnableSrqParms {
   Device_Link lid;
   bool enable; /* Enable or disable interrupts */
   opaque handle<40>; /* Host specific data */
};

struct Device_LockParms {
   Device_Link lid; /* link id from create_link */
   Device_Flags flags; /* Contains the waitlock flag */
   unsigned long lock_timeout; /* time to wait to acquire lock */
};

struct Device_DocmdParms {
   Device_Link lid; /* link id from create_link */
   Device_Flags flags; /* flags specifying various options */
   unsigned long io_timeout; /* time to wait for I/O to complete */
   unsigned long lock_timeout; /* time to wait on a lock */
   long cmd; /* which command to execute */
   bool network_order; /* client's byte order */
   long datasize; /* size of individual data elements */
   opaque data_in<>; /* docmd data parameters */
};

struct Device_DocmdResp {
   Device_ErrorCode error; /* returned status */
   opaque data_out<>; /* returned data parameter */
};

program DEVICE_ASYNC{
   version DEVICE_ASYNC_VERSION {
      Device_Error device_abort (Device_Link) = 1;
   } = 1;
} = 0x0607B0;

program DEVICE_CORE {
   version DEVICE_CORE_VERSION {
      Create_LinkResp create_link (Create_LinkParms) = 10;
      Device_WriteResp device_write (Device_WriteParms) = 11;
      Device_ReadResp device_read (Device_ReadParms) = 12;
      Device_ReadStbResp device_readstb (Device_GenericParms) = 13;
      Device_Error device_trigger (Device_GenericParms) = 14;
      Device_Error device_clear (Device_GenericParms) = 15;
      Device_Error device_remote (Device_GenericParms) = 16;
      Device_Error device_local (Device_GenericParms) = 17;
      Device_Error device_lock (Device_LockParms) = 18;
      Device_Error device_unlock (Device_Link) = 19;
      Device_Error device_enable_srq (Device_EnableSrqParms) = 20;
      Device_DocmdResp device_docmd (Device_DocmdParms) = 22;
      Device_Error destroy_link (Device_Link) = 23;
      Device_Error create_intr_chan (Device_RemoteFunc) = 25;
      Device_Error destroy_intr_chan (void) = 26;
   } = 1;
} = 0x0607AF;

