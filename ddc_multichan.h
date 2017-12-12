/***********************************************************************
*
*   File: ddc_multichan.h
*
*   Description: header file for ddc_multichan.c.
*
*   $Revision: 9 $   $Date: 3/30/15 6:18p $
*
*   $History: ddc_multichan.h $
* 
* *****************  Version 9  *****************
* User: Ccl          Date: 3/30/15    Time: 6:18p
* Updated in $/71620/BSP/win32/examples/ddc_multichan
* Change program default.
* 
* *****************  Version 8  *****************
* User: Ccl          Date: 3/27/15    Time: 5:19p
* Updated in $/71620/BSP/win32/examples/ddc_multichan
* Update to change program design to allow continuous data acquisition
* based on user specified loop count.
* 
* *****************  Version 7  *****************
* User: Misir        Date: 1/16/15    Time: 11:46a
* Updated in $/71640/BSP/x86_64/examples
* Added Viewer Support.
* 
* *****************  Version 1  *****************
* User: Misir        Date: 3/01/13    Time: 2:39p
* Created in $/71641/BSP/x86_64/examples
* 
* *****************  Version 5  *****************
* User: Frank        Date: 2/04/13    Time: 4:35p
* Updated in $/71620/BSP/win64/examples/ddc_multichan
* replaced local functions with Pentek Hi-Level lib functions
* 
* *****************  Version 4  *****************
* User: Frank        Date: 12/07/12   Time: 9:09a
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* modified parseArgs() to output usage message
* 
* *****************  Version 3  *****************
* User: Frank        Date: 12/03/12   Time: 1:49p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* corrected Win32 compiler warnings; changed unsigned int's to DWORD
* 
* *****************  Version 2  *****************
* User: Frank        Date: 11/20/12   Time: 12:40p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* added device type param to writeBufToFile()
* 
* *****************  Version 1  *****************
* User: Frank        Date: 11/20/12   Time: 9:56a
* Created in $/71660/BSP/win64/examples/ddc_multichan
* initial
* 
************************************************************************/


#include "ptk_osdep.h"         /* include first */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

#include "716x.h"              /* 716x-family general header */
#include "ptk716x.h"           /* 716x driver header */
#include "716xddc.h"           /* DDC header file */
#include "716xcmdline.h"       /* command line argument processing */
#include "716xregdump.h"       /* debug standard module registers */
#include "716xview.h"          /* For signal Analyzer */
#include "716xddcregdump.h"    /* debug DDC IP core registers */


/* program defines and constants ------------------------------------------
 *
 * The main program constants and defines are found below.  The first three 
 * are used to set default program parameters and can be changed by the 
 * user using the command line interface.  If the command line interface is
 * not used, they can be changed by the user here, provided the program is 
 * recompiled.
 *
 * Defines and constants after the first three are not capable of being 
 * changed using the command line interface.  They can be modified but the
 * program must then be recompiled. 
 */

/* NUM_DMA_BUFS - number of circular DMA buffers used in this example */
#define NUM_DMA_BUFS     1


/* MAX_CHANNELS - Maximum number DMA channels used to adquire data for the
 * program.  It is used to define number of buffers, parameter tables, etc.
 * The default value is 4, the maximum number of ADC channels in Pentek 
 * products.  It can be changed by the user to any value from 1 to 4 if 
 * memory usage restriction is required.  
 */
#define MAX_CHANNELS     4

/* DDC_BYPASS - set to 1 to bypass DDC insertion into the channel.  Data
 * input to the channel in this case is the internal sine generator.
 * Default = 0.
 *
 */
#define DDC_BYPASS       0

/* TRIGGER - set to 0 to run the program in continous, auto trigger mode.  Set
 *  to 1 to allow main() to generate a trigger when the thread is ready.
 */
#define TRIGGER          0

/* DEBUG - set to 1 for debugging statements */
#define DEBUG            0


/* ACTIVE_CHANNEL - selects the DDC channel used for input.  Use defines:
 *     P716x_DDC1 (program default)
 *     P716x_DDC2
 *     P716x_DDC3
 *     P716x_DDC4
 * Although 4 selections are shown above, not all may be available for 
 * your module.  For example, if the program is to be run on a 71621,
 * there are only 3 channels.  
 *
 * defaultProgArgs = "-chan", "1"
 */
const DWORD ACTIVE_CHANNEL = P716x_DDC1;


/* SAMPLES_PER_BUFFER - specifies the number of data samples captured by
 * each channel.  A single data sample is 32-bits, consisting of a 16-bit 
 * I-Data component and a 16-bit Q-Data component.  
 *  NOTE: SAMPLES_PER_BUFFER has been repaled by XFER_WORD_SIZE
 * default: 524288 samples (2097152 bytes)
 */
//const DWORD SAMPLES_PER_BUFFER = 32768;//1024; //524288; // To make this computer crash, change this number...


/* DATA_PACK_MODE - data from the selected ADC channel can be unpacked 
 * (16-bit data in 32 bits) or time packed (two 16-bit samples in each
 * 32-bit data word).  This define is used to select 
 * the packing mode that will be used.  Use defines:
 *     P716x_ADC_DATA_CTRL_PACK_MODE_I_DATA_PACK  (program default)
 *     P716x_ADC_DATA_CTRL_PACK_MODE_I_DATA_UNPACK
 */
const DWORD DATA_PACK_MODE = P716x_ADC_DATA_CTRL_PACK_MODE_I_DATA_PACK;

/* DATA_MODE - selects the data mode of data for the channel.
 * Select one of the following:
 *    P716x_DAC_WAVEFORM_16BIT_CHAN_PACKED (16 bit Channel packed data) 
 *    P716x_DAC_WAVEFORM_16BIT_TIME_PACKED (16 bit Time packed data) 
 */


const DWORD DATA_MODE = P716x_DAC_WAVEFORM_16BIT_CHAN_PACKED;
//const DWORD XFER_WORD_SIZE = 32768;    /* 32K of I/Q samples */
const DWORD XFER_WORD_SIZE = 4096;    /* Samples at 90MHz - for the DDC DMA - THIS IS THE NUMBER OF RANGE BINS */
//const DWORD XFER_WORD_SIZE = 4096;
//const DWORD XFER_WORD_SIZE = 8192;    /* Samples at 90MHz - for the DDC DMA - THIS IS THE NUMBER OF RANGE BINS */
//const DWORD XFER_WORD_SIZE = 8;    /* Samples at 90MHz - for the DDC DMA - THIS IS THE NUMBER OF RANGE BINS */
const DWORD XFER_WORD_SIZE_DAC_DMA = 32768;    /* for the DAC DMA */
//const DWORD DDC_XFER_WORD_SIZE = 6000;    /* 6E3 I/Q samples */
#define CONTINUOUS_TX 0 // For continuous pulses set this
const DWORD CLOCK_SOURCE = P716x_SBUS_CTRL1_CLK_SEL_VCXO_NO_REF;


/* DDC_DECIMATION - specifies the total DDC decimation desired.  This value
 * will be used to calculate the DDC decimation for both FIR stages and can
 * be a value from 2 to 65536.  Program default is set to 30, so both FIR 
 * stages are used.
 */
//const DWORD DDC_DECIMATION = 30;
const DWORD DDC_DECIMATION = 2;
const DWORD DAC_INTERPOLATION = 4;

#define BRDCLK 720e6

/* TUNING_FREQ and TEST_GEN_FREQ - TUNING_FREQ is tuning frequency of the 
 * DDC.  TEST_GEN_FREQ is the frequency of the internal test generator, if
 * used.  The default tuning frequency is 20.0 MHz.  The default test 
 * generator frequency is 20.01MHz.
 */

const double TUNING_FREQ    = 55.0e6 * 4; //60.625e6 * 4;// DDC tuning freq
const double DAC_NCO_FREQ   = 125.0e6;    //125.0e6;// 75000000.0; // DUC tuning freq


const double TEST_GEN_FREQ = 20000000.0;



/* DATA_SOURCE - selects the source of data for the DDC.  Use defines:
 *     DDC_DATA_SRC_DEFAULT_ADC
 *     DDC_DATA_SRC_ADC_1      (default)
 *     DDC_DATA_SRC_ADC_2
 *     DDC_DATA_SRC_ADC_3
 *     DDC_DATA_SRC_ADC_4
 *     DDC_DATA_SRC_TEST_GEN_REAL 
 *     DDC_DATA_SRC_TEST_GEN_CMPLX 
 */
#define DDC_DATA_SRC_DEFAULT_ADC    0x00
#define DDC_DATA_SRC_ADC_1          0x01
#define DDC_DATA_SRC_ADC_2          0x02
#define DDC_DATA_SRC_ADC_3          0x03
#define DDC_DATA_SRC_ADC_4          0x04
#define DDC_DATA_SRC_TEST_GEN_REAL  0x10
#define DDC_DATA_SRC_TEST_GEN_CMPLX 0x20
const DWORD DATA_SOURCE = DDC_DATA_SRC_ADC_1;


/* NUM_PROG_ARGS - number of command line arguments allowed for this 
 * program.  There are 6 arguments; devType must stay as 'ddc',
 * number of channels, number of samples to be captured, DDC tuning 
 * frequency, DDC decimation, and data file saving format. 
 */
#define NUM_PROG_ARGS 13//10

/* NUM_TRANSFERS - specifies the number of DMA transfers the program will
 * allow before stopping.  Only data from the last two transfers will be
 * saved in the data buffers.  The default is 10000.
 */
const DWORD NUM_TRANSFERS = 100; 


/* REG_DUMP - set to 1 to dump selected registers after initialization to
 * a file.  Set to 0 (default), if not desired.  File name will be 
 * ddc_multichan.txt, using the PROGRAM_ID text string below.  This text 
 * string is also used for naming files when data saving is invoked.
 */
const DWORD  REG_DUMP   = 1;
char        *PROGRAM_ID = "ddc_multichan";


/* FILE_SAVE - if set to 1 (default), captured data is saved to a file. 
 * Filenames will be ddc_multichan_ddc1.dat, ddc_multichan_ddc2.dat, 
 * ddc_multichan_ddc3.dat, ddc_multichan_ddc4.dat, where the number in the 
 * filename corresponds to the DDC channel number.
 *
 * FILE_MODE - provided to specify how data is saved to the file, either
 * ASCII text or binary data.  Use defines:
 *     P716x_CMDLINE_FILE_FORMAT_BIN (default) for binary,
 *     P716x_CMDLINE_FILE_FORMAT_ASCII FOR ascii TEXT.
 */
const DWORD FILE_SAVE = 1;
const DWORD FILE_MODE = P716x_CMDLINE_FILE_FORMAT_BIN;


/* General definitions */


/* MODULE_RESOURCES - module resources structure, values set by 
 * localDeviceFindAndOpen(), contains module ID, BAR addresses, 
 * etc., as follows:
 *     moduleId          = actual module ID
 *     slot              = module slot 
 *     hDev              = device handle
 *     BAR0Base          = BAR 0 base address
 *     BAR2Base          = BAR 2 base address  
 *     BAR4Base          = BAR 4 base address
 *     p716xRegs         = module address table pointer
 *     p716xDdcRegs      = DDC core address table pointer
 *     p716xBrdResrc     = board resource table pointer
 *     p716xGlobalParams = global parameter table pointer
 *     p716xAdcParams    = ADC channel parameter table pointer
 *     p716xDdcParams    = DDC parameter table pointer
 *     progParams        = pointer to P716x_CMDLINE_ARGS, program arguments structure
 *     nextDev           = pointer to next module resource table
 *                         (linked list)
 */
typedef struct MODULE_RESRC
        {
            DWORD                 moduleId;
            DWORD                 slot;
            PVOID                 hDev;
            BAR_ADDR              BAR0Base;
            BAR_ADDR              BAR2Base;  
            BAR_ADDR              BAR4Base;
            P716x_REG_ADDR        p716xRegs;
            P716x_DDC_REG_ADDR    p716xDdcRegs;
            P716x_BOARD_RESOURCE  p716xBrdResrc;
            P716x_GLOBAL_PARAMS   p716xGlobalParams;
            P716x_DAC_CHAN_PARAMS p716xDacParams[P716x_MAX_DAC_CHANS];
            DAC5688_PARAMS        dac5688Params;
            P716x_ADC_CHAN_PARAMS p716xAdcParams[MAX_CHANNELS];
            P716x_DDC_CHAN_PARAMS p716xDdcParams[MAX_CHANNELS];
            P716x_CMDLINE_ARGS    progParams;
            void                 *nextDev;
        } MODULE_RESRC;


/* DMA_THREAD_PARAMS - DMA thread parameter structure
 *     moduleResrc    = Pointer to MODULE_RESRC, module resources structure
 *     chanNum        = ADC channel number
 *     adcParams      = ADC channel parameter table pointer
 *     ifcArgs        = OS-depandant structure pointer
 *     bufSize        = buffer size in bytes
 *     dmaHandle      = Pointer to PTK716X_DMA_HANDLE structure
 *     dmaBuf         = Array of PTK716X_DMA_BUFFER structure
 *     dataBuf        = data buffer pointer
 *     useViewer      = parameter to indicate whether Viewer is used for the thread
 *     sockFd         = Pointer to Socket File descriptor for the viewer
 *     newSockFd      = Pointer to Socket File descriptor for the viewer
 *     viewCtrlParams = Pointer to Viewer Control parameter structure
 *     exitCodePtr    = Pointer to thread's exit code
 */
typedef struct DMA_THREAD_PARAMS
        {
            MODULE_RESRC          *moduleResrc;
            int                    chanNum;
            IFC_ARGS              *ifcArgs;
            DWORD                  bufSize;
            PTK716X_DMA_HANDLE    *dmaHandle;
            PTK716X_DMA_BUFFER     dmaBuf[NUM_DMA_BUFS];
            int                   *dataBuf;
            DWORD                  useViewer;
            int                   *sockFd;
            int                   *newSockFd;
            P716x_VIEW_CONTROL    *viewCtrlParams;  
            int                   *exitCodePtr;
        } DMA_THREAD_PARAMS;


/* EXIT_HANDLE_RESRC - exit handler resources structure where:
 *     exitCode[5]   = exit codes for main and threads
 *     modResrcBase  - module resource table base address
 *     libStatPtr    = library status pointer
 *     intrStatPtr   = interrupt status pointer
 *     dmaHandlePtr  = pointer to DMA Handle
 *     dmaBufPtr     = pointer to DMA buffer
 *     dataBufPtr    = pointer to a data buffer where DMA'ed data is copied to
 *     numChans      = number of channels
 *     ifcArgs       = OS-dependent resource structure pointer
 */
typedef struct EXIT_HANDLE_RESRC
        {
            int             exitCode[MAX_CHANNELS+1];
            MODULE_RESRC   *modResrcBase;
            DWORD          *libStatPtr;
            DWORD          *intrStatPtr;
            PTK716X_DMA_HANDLE   **dmaHandlePtr[MAX_CHANNELS];
            PTK716X_DMA_BUFFER   *dmaBufPtr[MAX_CHANNELS][NUM_DMA_BUFS];
            int           **dataBufPtr[MAX_CHANNELS];
            DWORD          *numChans;
            IFC_ARGS       *ifcArgs;
        } EXIT_HANDLE_RESRC;



/* exit message table - this is a list of exit messages and their exit 
 * codes, used by the exitHandler() routine.  
 */
static char *exitMsg[] = 
{
    /* exit message table; message 9, for thread internal errors,
     * is empty.  Thread internal error messages are output from
     * threadErrMsg().
     */
    "program done",                                	 /* 0 */
    "Error: driver library init failed",           	 /* 1 */
    "Error: device failed to open",                	 /* 2 */
    "Error: invalid command line argument(s)",     	 /* 3 */
    "Error: invalid maximum number of channels",   	 /* 4 */
    "Error: buffer allocation failed",             	 /* 5 */
    "Error: requested channel not available",      	 /* 6 */
    "Error: filter table load failed",             	 /* 7 */ 
    "Error: semaphore creation failed",            	 /* 8 */
    "Error: thread start failed",                  	 /* 9 */
    "Error: invalid Signal Analyzer parameter",		 /* 10 */
    "Error: cannot connect to Signal Analyzer",		 /* 11 */
    "Error: sending data to Signal Analyzer failed",  /* 12 */
    "Error: Thread Error",                            /* 13 */
    "Error: Failure writing to file",                 /* 14 */
    "Error: Failure Enabling interrupt",              /* 15 */
    "Error: DMA channel failed to open",              /* 16 */
    "Error: DMA complete timeout",                    /* 17 */
    "Error: undefined error",
    NULL
};

/* VIEW_BLK_SIZE - viewer block size in bytes.  The default is 4096. */
const DWORD VIEW_BLK_SIZE = 4096;


/* function prototypes */
static void dmaIntHandler(PVOID               hDev, 
                          LONG                dmaChannel, 
                          PVOID               pData,
                          PTK716X_INT_RESULT *pIntResult);
int         main (int   argc, 
                  char *argv[]);
static int  PTKHLL_DeviceInit (MODULE_RESRC *moduleResrc);
static void PTKHLL_SetProgramOptions(P716x_CMDLINE_ARGS   *argParams,
                                     P716x_BOARD_RESOURCE *brdResrc);
static void selectDdcInput(DWORD  dataSrc, 
                           DWORD  chan,
                           unsigned int *iDataInputSel,
                           unsigned int *qDataInputSel,
                           unsigned int *cmplxInput);
//static void dmaThread(PVOID pParams, int Adc_delay);
static void dmaThread(PVOID pParams);
static int  exitHandler (EXIT_HANDLE_RESRC *ehResrc);
static int  regDump (MODULE_RESRC *moduleResrc, 
                     char         *progId,
                     DWORD         numChans,
                     double        tuningFreq,
                     DWORD         decimation);
static void PTKHLL_ViewerSetControlWord(DWORD               moduleId,
                                        P716x_VIEW_CONTROL *viewCtrlWord,
                                        P716x_CMDLINE_ARGS *progParams);

/* include Pentek High-Level Library routines */
#include "ptkhll.c"
