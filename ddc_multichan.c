/**************************************************************************
*
*   File: ddc_multichan.c
*
*   Description: This program demonstrates acquiring data all available
*                channels, routed through the digital down converter in
*                each channel.  Using program defaults, it captures a
*                continuous stream of DDC data for each channel.
*
*                Data source for the DDC in each channel is ADC 1, so
*                data from each channel can be compared to show all
*                channels are synchronized.  The DATA_SOURCE constant
*                is provided in ddc_multichan.h to change DDC input
*                source, if desired.
*
*                The number of channels used is set by using the maximum
*                number of channels determined by the board resource
*                structudmare initialization routine after a module has been
*                found and initialized.
*
*                The program always starts with DDC channel 1 and continues
*                using DDC channels in order until all channels are used.
*                If the use of the Signal Analyzer is specified in the
*                command line arguments, channel 1 will be selected as the
*                default data displaying channel.
*
*                Data packing mode is fixed to packed.
*
*   Program Usage:
*       ddc_multichan  (to use program defaults)    OR
*       ddc_multichan  -dev       ddc (must use 'ddc')
*                      -chan      <c>  c = Channel number (1 through 4)
*                                      Default = max available channels
*                      -xfersize  <s>  s = number of samples to transfer
*                                      Default = 524288 samples
*                      -loop      <l>  l = number loop counts to run (0 = run
*                                      forever)
*                      -tuneFreq  <t>  t = DDC tuning (NCO) frequency
*                                      Default = 20.0 MHz
*                      -decim     <d>  d = total DDC decimation value (2
*                                      to 65536)
*                                      Default = 30
*                      -datformat <f>  f = data format
*                                          "bin" for binary,
*                                          "asci" for ASCII
*                                      Default = bin
*                      -vport     <p>  p = Signal Analyzer port number
*                      -vhost     <h>  h = Signal Analyzer host address/name
*
*   Example:
*       ddc_multichan -chan 1 -xfersize 524288 -loop 10000 -tunefreq 20000000.0
*                     -decim 30 -datformat bin -vport 3223187469 -vhost localhost
*
*   $Revision: 26 $   $Date: 4/15/15 4:16p $
*
*   $History: ddc_multichan.c $
*
* *****************  Version 26  *****************
* User: Ccl          Date: 4/15/15    Time: 4:16p
* Updated in $/71620/BSP/win64/examples/ddc_multichan
* Allow data writing to a file even if sending data to the viewer fails.
*
* *****************  Version 25  *****************
* User: Ccl          Date: 4/08/15    Time: 3:45p
* Updated in $/71620/BSP/win32/examples/ddc_multichan
* Modify to free memory first berfore closing device in exit handler.
*
* *****************  Version 24  *****************
* User: Ccl          Date: 3/30/15    Time: 6:17p
* Updated in $/71620/BSP/win32/examples/ddc_multichan
* Correct program descriptions, and change program defaults.
*
* *****************  Version 23  *****************
* User: Ccl          Date: 3/27/15    Time: 5:13p
* Updated in $/71620/BSP/win32/examples/ddc_multichan
* Changed program design to allow continuous data acquisition based on
* user specified loop count.
* tm_mye
* *****************  Version 22  *****************
* User: Ccl          Date: 1/21/15    Time: 2:37p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* Changed the use of a debugging function back to a library function.
*
* *****************  Version 21  *****************
* User: Misir        Date: 1/14/15    Time: 4:25p
* Updated in $/71640/BSP/win64/examples/ddc_multichan
* Added viewer support.
*
* *****************  Version 20  *****************
* User: Ccl          Date: 11/14/14   Time: 4:57p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* Fix sampling rate for 7164x/7174x.
*
* *****************  Version 19  *****************
* User: Ccl          Date: 9/26/14    Time: 4:52p
* Updated in $/71640/BSP/win64/examples/ddc_multichan
* Added call to detect frequency for 7164x and 7174x
* Added support to 71741.
*
* *****************  Version 18  *****************
* User: Misir        Date: 7/11/13    Time: 12:44p
* Updated in $/71640/BSP/win32/examples/ddc_multichan
* The 71641 defaults to NON-DESMODE in this example, therefore the only
* valid decimations are 4, 8 and 16.  Modifications were made in the ddc
* core setup function to accommodate this.
*
* *****************  Version 17  *****************
* User: Frank        Date: 6/20/13    Time: 3:41p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* corrected Q-data selection
*
* *****************  Version 16  *****************
* User: Frank        Date: 3/06/13    Time: 4:28p
* Updated in $/71620/BSP/win64/examples/ddc_multichan
* fixed one output message; general cleanup
*
* *****************  Version 15  *****************
* User: Misir        Date: 3/05/13    Time: 4:10p
* Updated in $/71641/BSP/x86_64/examples
* Cleaned up code to support 71641 DDC Core.
*
* *****************  Version 13  *****************
* User: Frank        Date: 2/04/13    Time: 4:35p
* Updated in $/71620/BSP/win64/examples/ddc_multichan
* replaced local functions with Pentek Hi-Level lib functions
*
* *****************  Version 12  *****************
* User: Frank        Date: 12/07/12   Time: 9:09a
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* modified parseArgs() to output usage message
*
* *****************  Version 11  *****************
* User: Frank        Date: 12/03/12   Time: 1:49p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* corrected Win32 compiler warnings; changed unsigned int's to DWORD
*
* *****************  Version 10  *****************
* User: Frank        Date: 11/20/12   Time: 12:40p
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* added device type param to writeBufToFile()
*
* *****************  Version 9  *****************
* User: Frank        Date: 11/20/12   Time: 9:56a
* Updated in $/71660/BSP/win64/examples/ddc_multichan
* updated to scan for modules, added program header file, other changes
*
* *****************  Version 8  *****************
* User: Misir        Date: 8/01/12    Time: 12:02p
* Updated in $/71621/BSP/win32/examples/ddc_multichan
* Now using the MAX_CHANNELS define.
*
* *****************  Version 7  *****************
* User: Misir        Date: 3/09/12    Time: 2:41p
* Updated in $/71621/BSP/win32/examples/ddc_multichan
* Fixed bug concerning Register Dump.
*
* *****************  Version 6  *****************
* User: Ccl          Date: 1/20/12    Time: 1:41p
* Updated in $/71651/BSP/win32/examples/ddc_multichan
* Change default tuning frequency value.
*
* *****************  Version 5  *****************
* User: Misir        Date: 1/10/12    Time: 11:59a
* Updated in $/71621/BSP/x86_64/examples
* Added OSDEP.
*
* *****************  Version 2  *****************
* User: Misir        Date: 5/02/11    Time: 1:54p
* Updated in $/71621/BSP/x86/examples
* Ported new Windows examples to Linux.
*
* *****************  Version 2  *****************
* User: Frank        Date: 4/01/11    Time: 12:58p
* Updated in $/71621/BSP/win64/examples/ddc_multichan
* change command line param refClk to refFreq
*
* *****************  Version 1  *****************
* User: Frank        Date: 3/24/11    Time: 9:39a
* Created in $/71621/BSP/win64/examples/ddc_multichan
* initial
*
**************************************************************************/


#include "ddc_multichan.h"

#define EXT_TRIG 1
#define DAC 1
#define DANE_CHANGED_TRIG_SOURCE 1
#define ARB_WAVEFORM 1


#define RESET_REGS 0


void ddc_71641_core_setup (MODULE_RESRC       *modResrcBase,
                           P716x_CMDLINE_ARGS  progParams,
                           unsigned int        chanNum);

static unsigned int stopFlag = 0;
#if (DEBUG)
unsigned int intrCount = 0;
#endif

int Adc_delay;
volatile int SAMPLES_PER_PRI_GLOBAL;


/**************************************************************************
Parser setup START
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Redefines what the comment prefix should be. By default it is set to ";"
#define INI_INLINE_COMMENT_PREFIXES "%"
#include "ini.c"

// Parameters from header file that are necessary for this parser
typedef struct
{
    int NUM_TRANSFERS; // This is the number of transfers you are hoping to save.
    int WAVEFORM;
    int DAC_DELAY;
    int ADC_DELAY;
    int SAMPLES_PER_PRI;
    int NEXT_VARIABLE;

} configuration;

// Handler used by the parser to assign values to variables.
// Variables are declared in the configuration struct
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    //#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    //got rid of above line since sections are not necessary here. They are
    //necessary for the Rhino
    #define MATCH(n) strcmp(name, n) == 0

    if (MATCH("NUM_PRIS")) {
        pconfig->NUM_TRANSFERS = atoi(value);
	} else if (MATCH("WAVEFORM_INDEX")) {
		pconfig->WAVEFORM = atoi(value);
    } else if (MATCH("DAC_DELAY")) {
		pconfig->DAC_DELAY = atoi(value);
    } else if (MATCH("ADC_DELAY")) {
		pconfig->ADC_DELAY = atoi(value);
    } else if (MATCH("SAMPLES_PER_PRI")) {
		pconfig->SAMPLES_PER_PRI = atoi(value);
    } else if (MATCH("NEXT_VARIABLE")) {
        pconfig->NEXT_VARIABLE = atoi(value);
    }  else {
        return 0;  // unknown section/name, error
    }

    return 1;
}
/**************************************************************************
Parser setup END
**************************************************************************/









/**************************************************************************
 Function:    dmaIntHandler()

 Description: This routine serves as the User Mode interrupt handler for
              this example.

 Parameters:  hDev        - 716x Device Handle
              dmaChannel  - DMA channel generating the interrupt(0-3)
              pData       - Pointer to user defined data
              pIntResults - Pointer to the interrupt results structure

 Return:      none
**************************************************************************/
static void dmaIntHandler(PVOID               hDev,
                          LONG                dmaChannel,
                          PVOID               pData,
                          PTK716X_INT_RESULT *pIntResult)
{
    IFC_ARGS *ifcArgs = (IFC_ARGS *)pData;
#if DEBUG
    intrCount++;
#endif
    PTKIFC_SemaphorePost (ifcArgs, (4 + dmaChannel));
}


/**************************************************************************
 Function:     main

 Description:  Multi-channel 716x DDC data acquition example.

               main() initializes the board and starts the channel threads.
               The threads set up the individual channel, arms the trigger
               and sets up the channel's DMA engine for receipt of the
               trigger signal.  Once all channels are ready, main()
               generates the trigger and waits for all threads to signal
               that their DMA transfer has completed.

 Parameters:   none
 Return:       0  - program completed successfully
               1  - Error: driver library init failed
               2  - Error: device failed to open
               3  - Error: invalid command line argument(s)
               4  - Error: invalid maximum number of channels
               5  - Error: buffer allocation failed
               6  - Error: requested channel not available
               7  - Error: filter table load failed
               8  - Error: semaphore creation failed
               9  - Error: thread start failed
               10 - internal thread failure
               11 - invalid Signal Analyzer parameter
               12 - cannot connect to Signal Analyzer
               13 - sending data to Signal Analyzer failed
               14 - Thread Error
               15 - Failure writing to file
               16 - Failure Enabling interrupt
               17 - DMA channel failed to open
               18 - DMA complete timeout

**************************************************************************/
int main (int argc, char *argv[])
{
    IFC_ARGS               ifcArgs;
    MODULE_RESRC          *moduleResrc = NULL;
    int                   *IDTable     = (int *)&P716xValidIdTable;
    DWORD                  numDevices;
    ADC12D1800_PARAMS      adc12d1800Params;

    DWORD                  dataSrc     = DATA_SOURCE;
    DWORD                  chan;
    DWORD                  bufSize;            /* in bytes */
    DWORD                  ddcBufSize;            /* in bytes */
    DWORD                  ducBufSize;            /* in bytes */
    DWORD                  numChans;
    double                 tuningFreq;
    DWORD                  decimation;

    DWORD                  libStat     = PTK716X_STATUS_UNDEFINED;
    DWORD                  intrStat    = PTK716X_STATUS_OK;


    DMA_THREAD_PARAMS      dmaThreadParams[MAX_CHANNELS];

    DWORD                  dwStatus;
    DWORD                  calcRet;
    DWORD                  actualDec;
    unsigned int           pulseCount = 0;
    int                    status;
    unsigned int           i;




#if DAC

	  P716x_DAC_DMA_DESCRIPT_CWORD_PARAMS  dacDmaCword;
    P716x_DAC_DMA_LLIST_DESCRIPTOR       dacDmaDescriptor;
    P716x_DAC_OCTRL_LLIST_DEFINITION     dacOutLlistDef;

	 DWORD                dacChan     = ACTIVE_CHANNEL;
    PTK716X_DMA_HANDLE  *dmaHandle   = NULL;
    PTK716X_DMA_BUFFER   dmaBuf;

    DWORD                bufStat     = PTK716X_STATUS_OK;

	   DWORD                memType;
    DWORD                acqType;




#endif





    /* Signal Analyzer variables and structures */
    DWORD                  useViewer[MAX_CHANNELS] = {0,0,0,0};  /* 0 = no; 1 = yes */
    int                    sockFd;
    int                    newSockFd;
    P716x_VIEW_CONTROL     viewCtrlParams;
    DWORD                  dispChannel;

    EXIT_HANDLE_RESRC      exitHdlResrc =
                           {
                               {0, 0, 0, 0, 0},
                               NULL,              /* modResrcBase pointer */
                               &libStat,
                               &intrStat,
                               {&(dmaThreadParams[0].dmaHandle),
                                &(dmaThreadParams[1].dmaHandle),
                                &(dmaThreadParams[2].dmaHandle),
                                &(dmaThreadParams[3].dmaHandle)},

                               /* Will be initialized later, based on the value of NUM_DMA_BUFS */
                               {&(dmaThreadParams[0].dmaBuf[0])},

                               {&(dmaThreadParams[0].dataBuf),
                                &(dmaThreadParams[1].dataBuf),
                                &(dmaThreadParams[2].dataBuf),
                                &(dmaThreadParams[3].dataBuf)},
                               &numChans,
                               &ifcArgs
                           };



    /* Program entry ------------------------------------------------------
     *
     * Initialize library access, identify board, allocate buffers, etc.
     */

    printf ("\n[%s] Entry\n", PROGRAM_ID);

    /* initialize OS-dependent resources */
    PTKIFC_Init(&ifcArgs);

    PTKIFC_MutexCreate (&ifcArgs, 0);

    /* initialize the PTK716X library */
    libStat = PTK716X_LibInit();
    if (libStat != PTK716X_STATUS_OK)
    {
        exitHdlResrc.exitCode[0] = 1;
        return (exitHandler (&exitHdlResrc));
    }

    printf ("                Scanning for modules\n");
    moduleResrc = PTKHLL_DeviceFindAndOpen(IDTable, &numDevices);
    if (numDevices == 0)       /* no module found */
    {
        (exitHdlResrc.exitCode[0]) = 2;
        return (exitHandler(&exitHdlResrc));
    }

    /* save module resource base address to exit handler */
    exitHdlResrc.modResrcBase = moduleResrc;

    /* more than one module found? */
    if (numDevices > 1)
        moduleResrc = PTKHLL_DeviceSelect(moduleResrc);

    PTKHLL_DisplayBarAddresses (PROGRAM_ID, moduleResrc);

    /* process command line arguments */
    status = PTKHLL_ParseArgs(&(argc), argv, &(moduleResrc->progParams),
                              &(moduleResrc->p716xBrdResrc));

	if (status != 0)
    {
        if (status == P716x_CMDLINE_BAD_ARG)
            exitHdlResrc.exitCode[0] = 3; /* invalid argument */
        else
            exitHdlResrc.exitCode[0] = 0; /* user usage query */
        return (exitHandler(&exitHdlResrc));
    }

    /* use command line param to set program parameters */
    bufSize     = moduleResrc->progParams.xferSize * 4;  /* size in bytes */
    //ddcBufSize     = DDC_XFER_WORD_SIZE * 4;  /* size in bytes */
    tuningFreq  = moduleResrc->progParams.tuneFreq;      /* same for all channels */
    decimation  = moduleResrc->progParams.decimation;    /* same for all channels */
    dispChannel = moduleResrc->progParams.channel - 1 ;
    numChans    = moduleResrc->p716xBrdResrc.numADC;

    /*  We need to match the Data source to its corresponding channel */
    if ((moduleResrc->moduleId != P71641_MODULE_ID) &&
        (moduleResrc->moduleId != P71741_MODULE_ID))
    {
        if (dispChannel == P716x_ADC1 )
        {
            dataSrc = DDC_DATA_SRC_ADC_1;
        }
        else if (dispChannel == P716x_ADC2)
        {
            dataSrc = DDC_DATA_SRC_ADC_2;
        }
        else if (dispChannel == P716x_ADC3)
        {
            dataSrc = DDC_DATA_SRC_ADC_3;
        }
        else if (dispChannel == P716x_ADC4)
        {
            dataSrc = DDC_DATA_SRC_ADC_4;
        }
    }

    /* verify the specified channel exists */
    if (dispChannel > moduleResrc->p716xBrdResrc.numDDC)
    {
        exitHdlResrc.exitCode[0] = 6;
        return (exitHandler(&exitHdlResrc));
    }

    /* set useViewer parameter based on command line parameters */
    if (((moduleResrc->progParams.viewPort) != P716x_CMDLINE_UNSUPPORTED_ARG) &&
        ((moduleResrc->progParams.viewPort) != P716x_CMDLINE_BAD_ARG)           )
    {
        useViewer[dispChannel] = 1;                 /* use viewer */
    }

// DP
#if 0
    /* Open a DMA Channel */
    status = PTK716X_DMAOpen(moduleResrc->hDev,  dacChan, &(dmaHandle));
    if (status != PTK716X_STATUS_OK)
    {
		printf("DMA Open problem\n");
                exit(0);
    }


    /* allocate DMA data buffer */
    bufStat = PTK716X_DMAAllocMem(dmaHandle, (moduleResrc->progParams.xferSize << 2),
                                  &(dmaBuf), TRUE);
    if (bufStat != PTK716X_STATUS_OK)
    {
       printf("DMA alloc problem\n");
                exit(0);
    }

    /* set buffer to a value, for debug only */
    memset (dmaBuf.usrBuf, 0x5a, (moduleResrc->progParams.xferSize) << 2);
#endif

// DP
#if DAC
    /* Open a DMA Channel */
    status = PTK716X_DMAOpen(moduleResrc->hDev,  dacChan, &(dmaHandle));
    if (status != PTK716X_STATUS_OK)
    {
		printf("DMA Open problem\n");
                exit(0);
    }


    /* allocate DMA data buffer */
    bufStat = PTK716X_DMAAllocMem(dmaHandle, (XFER_WORD_SIZE_DAC_DMA << 2),
                                  &(dmaBuf), TRUE);
    if (bufStat != PTK716X_STATUS_OK)
    {
       printf("DMA alloc problem\n");
                exit(0);
    }

    /* set buffer to a value, for debug only */
    memset (dmaBuf.usrBuf, 0x5a, XFER_WORD_SIZE_DAC_DMA << 2);
#endif


#if !ARB_WAVEFORM
    /* Generate a waveform and store in the dacData buffer */
    status = P716xDacWaveformGen(DATA_MODE, (unsigned int *)dmaBuf.usrBuf,
                                 moduleResrc->progParams.xferSize, 32);
    if (status != 0)
    {
       printf("DAC Waveform Gen problem\n");
                exit(0);
    }
#endif

#if ARB_WAVEFORM
{ // OPEN BLOCK
    int wdCount;
	int wdTemp;
	int *ptrTemp;
	int wdSize;
    char        sbBuffer[20+1];
    FILE        * ptrFile;

    // DP
    //wdSize = XFER_WORD_SIZE; // Long enough to cover the entire buffer.
    wdSize = XFER_WORD_SIZE_DAC_DMA; // Long enough to cover the entire buffer.

    printf("LOADING CUSTOM WAVEFORM \n");
    printf("funcLoadChirpWaveform\n wdSize = %i\n", wdSize);
    //ptrFile = fopen("C:\\Waveforms\\MultiWaveformVector.dat", "r");
	//ptrFile = fopen("C:\\Waveforms\\10MHzSine.dat", "r");
	//ptrFile = fopen("C:\\Waveforms\\5p625MHzSine.dat", "r");
	//ptrFile = fopen("./Waveforms/WaveformDataTable.dat", "r");
	//ptrFile = fopen("///smbtest/WaveformsRandy17Nov2017/WaveGenFiles/0p25usPulse.dat", "r");
        ptrFile = fopen("///smbtest/Waveforms/WaveformTable.dat", "r");

	//ptrFile = fopen("./Waveforms/B50LFMChirp.dat", "r");
    ptrTemp = (unsigned int *)dmaBuf.usrBuf;
    //*****************************

    if (ptrTemp == NULL || ptrFile == NULL)
    {

		if (ptrTemp == NULL) printf("ptrTemp == NULL");
		if (ptrFile == NULL) printf("ptrFile == NULL");
		printf("ARB Waveform Gen problem\n");
		/* Generate a waveform and store in the dacData buffer */
		/* // DP
		status = P716xDacWaveformGen(DATA_MODE, (unsigned int *)dmaBuf.usrBuf,
									 moduleResrc->progParams.xferSize, 32);
		*/
		status = P716xDacWaveformGen(DATA_MODE, (unsigned int *)dmaBuf.usrBuf,
									 XFER_WORD_SIZE_DAC_DMA, 32);

		if (status != 0)
		{
		   printf("DAC Waveform Gen problem\n");
					exit(0);
		}
    }
    for (wdCount=0; wdCount < wdSize; wdCount++)
    {
        //if((fgets(sbBuffer, 20, ptrFile))<=0)
        if((fgets(sbBuffer, 20, ptrFile))==NULL)
            break;

        wdTemp = atoi(sbBuffer);
        ptrTemp[wdCount] = wdTemp;
    }
    //******************************
    fclose(ptrFile);
} // CLOSE BLOCKolumns 1 through 6:

#endif

#if DAC
	/* Flush the CPU caches (see documentation of WDC_DMASyncCpu()) */
    PTK716X_DMASyncCpu(&(dmaBuf));

	    moduleResrc->p716xGlobalParams.sbusParams.gateBRecSource = \
            P716x_SBUS_CTRL2_GATEB_RCV_SRC_TRIG_IN;
//		moduleResrc->p716xGlobalParams.sbusParams.sbusGateBInputTapDelay = 0x1F;
		  /* DAC Data Control register */
    moduleResrc->p716xDacParams[dacChan].dataSource = \
        P716x_DAC_DATA_CTRL_DATA_SRC_DMA_RAM;

    /* DAC Rate Divider register */
    moduleResrc->p716xDacParams[dacChan].rateDivide = moduleResrc->progParams.rateDiv;

    /* DAC Gate/Trigger Control register */
    moduleResrc->p716xDacParams[dacChan].triggerMode = \
        P716x_DAC_GATE_TRIG_CTRL_TRIG_MODE_TRIG;
    moduleResrc->p716xDacParams[dacChan].gateTrigEnable = \
        P716x_DAC_GATE_TRIG_CTRL_GATE_TRIG_ENABLE;

    /* Output Control Linked List Start Pointer register */
    moduleResrc->p716xDacParams[dacChan].llistStartIndex = 0;



    moduleResrc->p716xDacParams[dacChan].syncOutEnable  = P716x_DAC_DATA_CTRL_SYNC_OUT_ENABLE;
       moduleResrc->p716xDacParams[dacChan].syncSelect     = P716x_DAC_DATA_CTRL_SYNC_SEL_SYNCB;
    moduleResrc->p716xDacParams[dacChan].outputGateDelay= 0x6f;


    /** Set up Dac5688 Parameters **/

    /* DAC5688 parameters (By now, the DAC5688 has been reset and enabled) */
    moduleResrc->dac5688Params.interpValue = \
        DAC5688ConvertInterp(moduleResrc->progParams.interpolation);


	moduleResrc->dac5688Params.interpValue = \
        DAC5688ConvertInterp(DAC_INTERPOLATION);

    if (DATA_MODE == P716x_DAC_WAVEFORM_16BIT_CHAN_PACKED)
        moduleResrc->dac5688Params.inselMode = \
            DAC5688_CONFIG1_INSELMODE_NORMAL;
    else  /* (DATA_MODE = P716x_DAC_WAVEFORM_16BIT_TIME_PACKED) */
        moduleResrc->dac5688Params.inselMode = \
            DAC5688_CONFIG1_INSELMODE_HALFRATE_DATA_AB;

    moduleResrc->dac5688Params.diffClkEna = \
        DAC5688_CONFIG2_DIFFCLK_DISABLE;
    moduleResrc->dac5688Params.clk1InEna = \
        DAC5688_CONFIG2_CLK1_IN_DISABLE;
    moduleResrc->dac5688Params.clk1cInEna = \
        DAC5688_CONFIG2_CLK1C_IN_DISABLE;
    moduleResrc->dac5688Params.fir4Ena = \
        DAC5688_CONFIG2_FIR4_DISABLE;
    moduleResrc->dac5688Params.mixerEna = \
        DAC5688_CONFIG2_MIXER_ENABLE;

    /* Set NCO Frequency to 10 Mhz */
    //moduleResrc->dac5688Params.ncoFrequency   = 20000000.00;
    moduleResrc->dac5688Params.ncoFrequency   = DAC_NCO_FREQ;


    /* use software sync for all functions */
    moduleResrc->dac5688Params.ncoSel = \
        DAC5688_CONFIG22_SYNC_SIF_SIG;
    moduleResrc->dac5688Params.ncoRegSel = \
        DAC5688_CONFIG22_SYNC_SIF_SIG;
    moduleResrc->dac5688Params.qmCorrRegSel = \
        DAC5688_CONFIG22_SYNC_SIF_SIG;
    moduleResrc->dac5688Params.qmOffsetRegSel = \
        DAC5688_CONFIG22_SYNC_SIF_SIG;

    moduleResrc->dac5688Params.fifoSel = \
        DAC5688_CONFIG23_SYNC_SIF_SIG; /* Software sync */

    moduleResrc->dac5688Params.pllEna = \
        DAC5688_CONFIG26_PLL_DISABLE;  /* Not using PLL */
    moduleResrc->dac5688Params.io1p83p3 = \
        DAC5688_CONFIG26_IO_1P8_3P3_SET;


	/* Config register 22 */
	moduleResrc->dac5688Params.ncoSel         = DAC5688_CONFIG22_SYNC_FROM_FIFO_OUTPUT;
    moduleResrc->dac5688Params.ncoRegSel      = DAC5688_CONFIG22_SYNC_FROM_FIFO_OUTPUT; // The FIFO_OUTPUT is driven by the DAC5688 PIN is driven by the SYNC B signal, which is driven by GATE B, which is driven by the LVTTL external trigger.
    moduleResrc->dac5688Params.qmCorrRegSel   = DAC5688_CONFIG22_SYNC_FROM_FIFO_OUTPUT;
   moduleResrc->dac5688Params.qmOffsetRegSel = DAC5688_CONFIG22_SYNC_FROM_FIFO_OUTPUT;

	/* Enable this for inverse sinc on DAC */
    moduleResrc->dac5688Params.fir4Ena    = DAC5688_CONFIG2_FIR4_ENABLE;

    /* Config registers 5 and 23 */
    moduleResrc->dac5688Params.clkDivSyncEna  = DAC5688_CONFIG5_CLK_SYNC_DIV_ENABLE;	printf("DAC5688_CONFIG5_CLK_SYNC_DIV_ENABLE %i \n",  DAC5688_CONFIG5_CLK_SYNC_DIV_ENABLE);
//    moduleResrc->dac5688Params.clkDivSyncSel  = DAC5688_CONFIG5_CLK_SYNC_DIV_SEL_CLEAR; printf("DAC5688_CONFIG5_CLK_SYNC_DIV_SEL_CLEAR %i \n",  DAC5688_CONFIG5_CLK_SYNC_DIV_SEL_CLEAR);
	moduleResrc->dac5688Params.clkDivSyncSel  = DAC5688_CONFIG5_CLK_SYNC_DIV_SEL_SET; // DPP


    moduleResrc->dac5688Params.fifoSel        = DAC5688_CONFIG23_SYNC_FROM_PIN; // The DAC5688 PIN is driven by the SYNC B signal, which is driven by GATE B, which is driven by the LVTTL external trigger.


	moduleResrc->p716xGlobalParams.sbusParams.syncBRecSource = \
            P716x_SBUS_CTRL2_SYNCB_RCV_SRC_POS_GATE;


#endif


    /* Initialize DMA handles and allocate data buffers ---------------- */

    /* set all DMA handles and DMA buffers to NULL */
    for (chan = P716x_ADC1; chan < MAX_CHANNELS; chan++)
    {
        *(exitHdlResrc.dmaHandlePtr[chan]) = NULL;

        for (i = 0; i < NUM_DMA_BUFS; i++)
        {
            (exitHdlResrc.dmaBufPtr[chan][i]) = &(dmaThreadParams[chan].dmaBuf[i]);
            (dmaThreadParams[chan].dmaBuf[i]).usrBuf = NULL;
        }
    }

    /* initialize the module using library routines -----------------------
     *
     * set parameters for this program; parameter defaults are set when
     * PTKHLL_DeviceFindAndOpen() is called.
     */

    puts ("[ddc_multichan] initialization");

    if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
        (moduleResrc->moduleId == P71741_MODULE_ID))
        ADC12D1800SetParamsDefaults(&adc12d1800Params);


    /* set parameters -------------------------------------------------- */

    /* Global Parameters - the program uses internal clock A and internal
     * gate A register for the gate signal but obtains them from the front
     * panel sync bus.  Therefore, the module is set up as bus master.
     */

#if !DANE_CHANGED_TRIG_SOURCE
    moduleResrc->p716xGlobalParams.sbusParams.clockMaster =
         P716x_SBUS_CTRL1_CLK_MASTER_ENABLE;
    moduleResrc->p716xGlobalParams.sbusParams.gateASyncAMaster =
         P716x_SBUS_CTRL1_GATEA_SYNCA_MASTER_ENABLE;
    moduleResrc->p716xGlobalParams.sbusParams.gateARecSource =
         P716x_SBUS_CTRL2_GATEA_RCV_SRC_GATE_REG;
    moduleResrc->p716xGlobalParams.sbusParams.syncARecSource =
         P716x_SBUS_CTRL2_SYNCA_RCV_SRC_SYNC_REG;
#endif

#if DANE_CHANGED_TRIG_SOURCE
	moduleResrc->p716xGlobalParams.sbusParams.clockMaster =
         P716x_SBUS_CTRL1_CLK_MASTER_ENABLE;
    moduleResrc->p716xGlobalParams.sbusParams.gateASyncAMaster =
         P716x_SBUS_CTRL1_GATEA_SYNCA_MASTER_ENABLE;
    moduleResrc->p716xGlobalParams.sbusParams.gateARecSource =
         P716x_SBUS_CTRL2_GATEA_RCV_SRC_GATE_REG;
    moduleResrc->p716xGlobalParams.sbusParams.syncARecSource =
         P716x_SBUS_CTRL2_SYNCA_RCV_SRC_SYNC_REG;
#endif

#if DAC
	moduleResrc->p716xGlobalParams.sbusParams.clockBSource = \
        P716x_CLK_CTRL_STAT_FPGA_CLKB_SRC_SEL_DAC;

	//moduleResrc->p716xGlobalParams.sbusParams.clockBSource = \
        //P716x_CLK_CTRL_STAT_FPGA_CLKB_SRC_SEL_CDC;
#endif

	moduleResrc->p716xGlobalParams.sbusParams.clockSelect =
		P716x_SBUS_CTRL1_CLK_SEL_VCXO_EXT_CLK_REF;
	//	P716x_SBUS_CTRL1_CLK_SEL_EXT_CLK;

#if EXT_TRIG


	moduleResrc->p716xGlobalParams.sbusParams.gateARecSource = \
            P716x_SBUS_CTRL2_GATEA_RCV_SRC_TRIG_IN;
	moduleResrc->p716xGlobalParams.sbusParams.syncARecSource = \
            P716x_SBUS_CTRL2_SYNCA_RCV_SRC_POS_GATE;

	moduleResrc->p716xGlobalParams.sbusParams.sbusGateADriveSource = \
            P716x_SBUS_CTRL1_SBUS_GATEA_SRC_FP_TRIG;
#endif



    /* Internal Test Generator (if in use) */
    if ((dataSrc == DDC_DATA_SRC_TEST_GEN_REAL) ||
        (dataSrc == DDC_DATA_SRC_TEST_GEN_CMPLX)  )
    {
        /* enable the generator */
        moduleResrc->p716xGlobalParams.testSigParams.testSigEnable =
            P716x_TEST_SIG_TS_CTRL_TEST_SIG_RUN;

        /* set the frequency to 1.0MHz */
        moduleResrc->p716xGlobalParams.testSigParams.testAFreq =
            TEST_GEN_FREQ;
    }

    if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
        (moduleResrc->moduleId == P71741_MODULE_ID))
    {
        if( (P71640DetectAdcClkFreq(&(moduleResrc->p716xRegs),
                               &(moduleResrc->progParams.clockFreq))) != 0 )
			puts ("                Clock not detected!\n");
    }


    /* set the ADC sampling clock frequency - This is actually the DAC sampling frequency. The ADC gets its clock from this. */
    moduleResrc->p716xGlobalParams.brdClkFreq = BRDCLK;	//moduleResrc->progParams.clockFreq;
    printf("[ddc_multichan] BRDCLK is now set to %.2f\n", BRDCLK);
	printf("numchans = %d\n",numChans);

    /* ADC Channel Parameters - for this program, the only non-default
     * channel parameter is the data source.
     */
    for (chan = P716x_ADC1; chan < numChans; chan++)
    {
        /* enable the gate signal input for the channel */
        moduleResrc->p716xAdcParams[chan].gateTrigEnable =
            P716x_ADC_GATE_TRIG_CTRL_GATE_TRIG_IN_ENABLE;

#if EXT_TRIG
	     /* ADC Gate/Trigger Control register */
   moduleResrc->p716xAdcParams[chan].triggerMode = \
        P716x_DAC_GATE_TRIG_CTRL_TRIG_MODE_TRIG;
#endif

        /* set ADC channel data packing mode */
        moduleResrc->p716xAdcParams[chan].dataPackMode =
            P716x_ADC_DATA_CTRL_PACK_MODE_IQ_DATA_PACK;

        /* set ADC rate divider to 1 */
        moduleResrc->p716xAdcParams[chan].rateDivide = 1;

        /* set ADC channel data source */
        moduleResrc->p716xAdcParams[chan].dataSource = dataSrc;

        /* verify availability of on-board RAM, enable it if found */
        status = PTKHLL_VerifyRamPath (chan, &(moduleResrc->p716xRegs),
                                       &(moduleResrc->p716xAdcParams[chan]));
        if (status == 0)
            puts ("                no onboard RAM available");
    }


    /* DDC Parameters - setup both Dolumns 1 through 6: DC and DDC stage parameters.  This
     * block of code also sets two ADC parameters that switch in and enable
     * the DDC core.  Only parameters necessary for this program are set.
     */
    for (chan = P716x_ADC1; chan < numChans; chan++)
    {
        /* insert DDC into the set ADC channel data path */
        moduleResrc->p716xAdcParams[chan].dataSelect =
            P716x_ADC_DATA_CTRL_USR_DATA_SEL_USER;
        moduleResrc->p716xAdcParams[chan].userDataValidEnable =
            P716x_ADC_GATE_TRIG_CTRL_USER_DVAL_ENABLE;

        /* set DDC general paramters */
        moduleResrc->p716xDdcParams[chan].tuningFreq =
            tuningFreq;
        moduleResrc->p716xDdcParams[chan].accSync =
            P716x_DDC_CH_CTRL1_ACC_SYNC_ENABLE;
        moduleResrc->p716xDdcParams[chan].fmtrSync =
            P716x_DDC_CH_CTRL2_FMTR_SYNC_ENABLE;
        moduleResrc->p716xDdcParams[chan].coreSync =
            P716x_DDC_CORE_CTRL_CORE_SYNC_ENABLE;
        moduleResrc->p716xDdcParams[chan].ddcOut =
            P716x_DDC_CH_CTRL2_DDC_OUT_ENABLE;

        if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
            (moduleResrc->moduleId == P71741_MODULE_ID))
        {
            moduleResrc->p716xDdcParams[chan].inverseSpectrum = \
                P716x_DDC_CH_CTRL2_INVRS_SPEC_ENABLE;
        }
        else
            moduleResrc->p716xDdcParams[chan].inverseSpectrum = \
                P716x_DDC_CH_CTRL2_INVRS_SPEC_DISABLE;


        /* Select DDC data input source (using local function) */
        selectDdcInput(chan, chan,
                       &moduleResrc->p716xDdcParams[chan].iDataInputSel,
                       &moduleResrc->p716xDdcParams[chan].qDataInputSel,
                       &moduleResrc->p716xDdcParams[chan].cmplxInput);

        if ((moduleResrc->moduleId != P71641_MODULE_ID) &&
            (moduleResrc->moduleId != P71741_MODULE_ID))
        {
            /* calculate stage decimation based on desired total decimation.
             * The library function calculates FIR stage decimation and
             * enables FIR Stage 2, if required.
             */
            calcRet = P716xCalcDdcStageDecimation(
                          decimation,
                          &moduleResrc->p716xDdcParams[chan].stage[P716x_DDC_STAGE1].decimation,
                          &moduleResrc->p716xDdcParams[chan].stage[P716x_DDC_STAGE2].decimation,
                          &actualDec,
                          &moduleResrc->p716xDdcParams[chan].firStage2,
                          &moduleResrc->p716xBrdResrc);
            if (calcRet == 1)
            {
                printf ("[ddc_multichan] ");
                printf ("Desired DDC decimation cannot be achieved\n");
                printf ("                ");
                printf ("Actual decimation = %d\n", actualDec);
            }

            /* enable FIR sync for both stages */
            moduleResrc->p716xDdcParams[chan].stage[P716x_DDC_STAGE1].firSync =
                P716x_DDC_CH_CTRL2_ST1_FIR_SYNC_ENABLE;
            moduleResrc->p716xDdcParams[chan].stage[P716x_DDC_STAGE2].firSync =
                P716x_DDC_CH_CTRL2_ST2_FIR_SYNC_ENABLE;
        }
    }

    /* apply parameter tables to registers --------------------------------
     *
     * P716xInitAdcRegs() returns a zero if the channel exists; if the
     * desired channel doesn't exist, exit program.
     */



	//moduleResrc->p716xGlobalParams.cdcParams.cdc7005Params.sbusClockBMuxSelect =
    //            CDC7005_WORD1_MUX2_SEL_DIV_BY_1;
	//moduleResrc->p716xGlobalParams.cdcParams.cdc7005Params.sbusClockBMuxSelect =
    //            CDC7005_WORD1_MUX3_SEL_DIV_BY_1;

    P716xInitGlobalRegs(&moduleResrc->p716xGlobalParams,
                        &moduleResrc->p716xRegs);

#if DAC

	 status = P716xInitDacRegs(&(moduleResrc->p716xDacParams[dacChan]),
                              &(moduleResrc->p716xRegs),
                              dacChan);
    if (status != PTK716X_STATUS_OK)
    {
        printf("DAC Init problem\n");
                exit(0);
    }



    DAC5688InitDac5688Regs(
        (unsigned int *)(moduleResrc->p716xRegs.dacRegs[dacChan].serialAddr),
        &(moduleResrc->dac5688Params),
        moduleResrc->p716xGlobalParams.brdClkFreq);


    /* Issue a DAC5688 software sync (default is using the Software sync)
     * Ie., DAC5688_CONFIG23_SYNC_SIF_SIG
     */
    DAC5688GenerateSifSync(
        (unsigned int *)(moduleResrc->p716xRegs.dacRegs[dacChan].serialAddr));


#if 0
    DAC5688RegDump(
        (unsigned int *)(moduleResrc->p716xRegs.dacRegs[dacChan].serialAddr),
        stdout);
    printf ("           After SI571SetOutputClock(%f) is called\n",
            progParams.clockFreq);
    SI571RegDump(moduleResrc->p716xRegs.twsiPort1ControlStatus,
                 P716x_SI571_VCXO_ADDR);
#endif  /* DEBUG */

#if 1
    /* Is Clock B detected? */
    puts ("           Verifying DAC clock is present");
    do
    {
        status = P716xGetClkCtrlStatCdcStatus(
                     moduleResrc->p716xRegs.clockControlStatus,
                     P716x_CDC_CLKB,
                     P716x_CDC_CLK_STAT_DETECT);
        BaseboardDelayuS(10);
    } while (status != P716x_CLK_CTRL_STAT_CDC_CLKB_DETECTED);


    /* verify FPGA Clock B is present by clearing and checking the flag */
    puts ("           Verify FPGA clock is present");
    do
    {
        P716xGetDacInterruptFlag(
            moduleResrc->p716xRegs.dacRegs[dacChan].interruptStatus,
            P716x_DAC_INTR_FPGA_CLKB_NOT_DETECT);
        BaseboardDelayuS(10);
        status = P716xGetDacInterruptFlag(
                     moduleResrc->p716xRegs.dacRegs[dacChan].interruptStatus,
                     P716x_DAC_INTR_FPGA_CLKB_NOT_DETECT);
    } while (status == P716x_DAC_INTR_FPGA_CLKB_NOT_DETECT);

#endif



    /* Clear Trigger */
    P716xSetDacGateTrigCtrlTrigClearState(
        moduleResrc->p716xRegs.dacRegs[dacChan].gateTrigControl,
        P716x_DAC_GATE_TRIG_CTRL_TRIG_CLR_ASSERT);

    /* Select trigger mode */
    P716xSetDacGateTrigCtrlTriggerMode(
        moduleResrc->p716xRegs.dacRegs[dacChan].gateTrigControl,
        P716x_DAC_GATE_TRIG_CTRL_TRIG_MODE_TRIG);


    /* Hold Output Control Linked List state machine in Reset */
    P716xSetDacGateTrigCtrlOutLListResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].gateTrigControl,
        P716x_DAC_GATE_TRIG_CTRL_OUT_CTRL_LLIST_RESET);




/**************************************************************************/
// TEST START
/**************************************************************************/
#if 1

// LMF standard
/*
int RAM_LENGTH_VEC[29] = {103,119,135,159,175,191,207,231,247,263,279,303,319,335,351,375,391,407,423,447,463,479,495,519,535,551,911,1359,1815};
int RAM_OFFSET_VEC[29] = {7,111,239,383,551,735,935,1151,1391,1647,1919,2207,2519,2847,3191,3551,3935,4335,4751,5183,5639,6111,6599,7103,7631,8175,8735,9655,11023};
float T_param_vec[29]  = {5e-07,6e-07,7e-07,8e-07,9e-07,1e-06,1.1e-06,1.2e-06,1.3e-06,1.4e-06,1.5e-06,1.6e-06,1.7e-06,1.8e-06,1.9e-06,2e-06,2.1e-06,2.2e-06,2.3e-06,2.4e-06,2.5e-06,2.6e-06,2.7e-06,2.8e-06,2.9e-06,3e-06,5e-06,7.5e-06,10.0e-06};
*/

/***************************************************************************
Build vectors from file input
int RAM_LENGTH_VEC[]
int RAM_OFFSET_VEC[]
double T_param_vec[]
****************************************************************************/

    printf("Generate arrays from file input\n");

    FILE *fp_ram_length_vec;
    FILE *fp_ram_offset_vec;
    FILE *fp_T_param_vec;

    fp_ram_length_vec = fopen("./RAMdataTable/RAM_LENGTH_VEC.txt", "r"); // might need to change the path in Linux
    fp_ram_offset_vec = fopen("./RAMdataTable/RAM_OFFSET_VEC.txt", "r"); // might need to change the path in Linux
    fp_T_param_vec = fopen("./RAMdataTable/T_paramVec.txt", "r"); // might need to change the path in Linux

    int ram_length_size,
        ram_offset_size,
        t_param_size;

    fscanf(fp_ram_length_vec ,"%d",&ram_length_size);
    fscanf(fp_ram_offset_vec ,"%d",&ram_offset_size);
    fscanf(fp_T_param_vec    ,"%d",&t_param_size);

    printf("ram_length size = %d \n", ram_length_size);
    printf("ram_offset size = %d \n", ram_offset_size);
    printf("t_param    size = %d \n", t_param_size   );

    int RAM_LENGTH_VEC[ram_length_size];
    int RAM_OFFSET_VEC[ram_offset_size];
    double T_param_vec[t_param_size];
/*
    int RAM_LENGTH_VEC[29] = {103,119,135,159,175,191,207,231,247,263,279,303,319,335,351,375,391,407,423,447,463,479,495,519,535,551,911,1359,1815};
    int RAM_OFFSET_VEC[29] = {7,111,239,383,551,735,935,1151,1391,1647,1919,2207,2519,2847,3191,3551,3935,4335,4751,5183,5639,6111,6599,7103,7631,8175,8735,9655,11023};
    float T_param_vec[29]  = {5e-07,6e-07,7e-07,8e-07,9e-07,1e-06,1.1e-06,1.2e-06,1.3e-06,1.4e-06,1.5e-06,1.6e-06,1.7e-06,1.8e-06,1.9e-06,2e-06,2.1e-06,2.2e-06,2.3e-06,2.4e-06,2.5e-06,2.6e-06,2.7e-06,2.8e-06,2.9e-06,3e-06,5e-06,7.5e-06,10.0e-06};
*/
    // read ram_length variables
    printf("Displaying ram_length parameters:\n");
    //int i = 0, val;
    char* junk;
    int val = 0;
    for(i = 0; i < ram_length_size; i++) {
        fscanf(fp_ram_length_vec,"%d",&RAM_LENGTH_VEC[i]);
	fscanf(fp_ram_length_vec,"%c", &junk);
        printf("%d ", RAM_LENGTH_VEC[i]);
    }
    printf("\n");

    // read ram_length variables
    printf("Displaying ram_offset parameters:\n");
    for(i = 0; i < ram_offset_size; i++) {
        fscanf(fp_ram_offset_vec,"%d",&RAM_OFFSET_VEC[i]);
	fscanf(fp_ram_offset_vec,"%c", &junk);
        printf("%d ", RAM_OFFSET_VEC[i]);
    }
    printf("\n");

    // read ram_length variables
    double tval;
    printf("Displaying T_param parameters:\n");
    for(i = 0; i < t_param_size; i++) {
        fscanf(fp_T_param_vec,"%lg",&T_param_vec[i]);
	fscanf(fp_T_param_vec,"%c",&junk);
        printf("%e ", T_param_vec[i]);
        //printf("%e ", t_param[i]);
    }
    printf("\n");

    fclose(fp_ram_length_vec);
    fclose(fp_ram_offset_vec);
    fclose(fp_T_param_vec);


/***************************************************************************

****************************************************************************/



    // Parser
	// closes program if file can't be found
    configuration config;

	//if (ini_parse("/smbtest/NeXtRAD_Header.txt", handler, &config) < 0) {
    if (ini_parse("///smbtest/NeXtRAD.ini", handler, &config) < 0) {
		printf("PARSER: Can't load file.'\n");
		return 1;
    }
	int PulseNum = config.WAVEFORM;
	int Dac_delay = config.DAC_DELAY;
	if(Dac_delay < 1) {
        printf("ERROR: DAC_DELAY must be greater than or equal to 1.");
        return 1;
	}

	Adc_delay = config.ADC_DELAY;
	SAMPLES_PER_PRI_GLOBAL = config.SAMPLES_PER_PRI;
	printf("SAMPLES_PER_PRI_GLOBAL = %d\n", SAMPLES_PER_PRI_GLOBAL);
	printf("PARSER:\nWAVEFORM INDEX = \t%i,\nDURATION = \t%0.1E s\n", PulseNum, T_param_vec[PulseNum-1]);

#endif

#if 1 // USE if not transmitting blanks
// Testing the active link
    /* Program Output Controller Linked List */
    dacOutLlistDef.delay     = 1+Dac_delay;      /* 8 clock cycles */
    dacOutLlistDef.length    = RAM_LENGTH_VEC[PulseNum-1];
    dacOutLlistDef.repeat    = 1;      /* Repeat = 1 round */

	//dacOutLlistDef.ramLength = (moduleResrc->progParams.xferSize) - 1;

	dacOutLlistDef.ramOffset = RAM_OFFSET_VEC[PulseNum-1]; // VALID VALUES MUST BE MULTIPLES OF 8 MINUS 1
	printf("RAM_OFFSET_VEC(%i)=%i \n",PulseNum-1,RAM_OFFSET_VEC[PulseNum-1]);
	dacOutLlistDef.ramLength = RAM_LENGTH_VEC[PulseNum-1]; // VALID VALUES MUST BE MULTIPLES OF 8 MINUS 1
	printf("RAM_LENGTH_VEC(%i)=%i \n",PulseNum-1,RAM_LENGTH_VEC[PulseNum-1]);

    /* Do not start immediately, wait for Trigger */
    dacOutLlistDef.continueImmed = \
        P716x_DAC_OCTRL_LLIST_CONTINUE_WAIT_FOR_TRIG;
    dacOutLlistDef.nextLinkDef   = 0;
    dacOutLlistDef.reserved      = 0;

    /* Initialize the Output Control Linked list */
    P716xInitDacOCtrlLList(&(dacOutLlistDef), &(moduleResrc->p716xRegs),
                           dacChan, 0);
#endif



/**************************************************************************/
// TEST END
/**************************************************************************/

#if 0 // USE 1 if transmitting 3 blanks
/**************************************************************************
 LINK 0
**************************************************************************/

    /* Program Output Controller Linked List */
    dacOutLlistDef.delay     = 0x0000BB80; // This will delay by X system clock cycles (X/360MSPS) // 0xBB80 = 2x6000x4 = twice the time the ADC is active, ie, the DAC is OFF for the entire recording.
    dacOutLlistDef.length    = 7;
    dacOutLlistDef.repeat    = 1;      /* Repeat = 1 round */
    dacOutLlistDef.ramOffset = 7;      /* Read data from beginning */
	dacOutLlistDef.ramLength = 7;

#if 0
    /* check memory type for the DAC (memory bank CD); adjust RAM length */
    memType = P716xGetDaughterBoardMemoryType(
        moduleResrc->p716xRegs.daughterBoardId, P716x_MEM_BANK_CD);
    if (memType == P716x_DAUGHTER_BOARD_ID_MEM_CD_TYPE_QDR_16MB) {
        dacOutLlistDef.ramLength = ((moduleResrc->progParams.xferSize) >> 1) - 1;  /* 64-bits word */
        printf("QDR RAM \n");
    } else {
        dacOutLlistDef.ramLength = (moduleResrc->progParams.xferSize) - 1;
        printf("DDR RAM \n");
	}
#endif

    /* Do not start immediately, wait for Trigger */
    dacOutLlistDef.continueImmed = \
        P716x_DAC_OCTRL_LLIST_CONTINUE_WAIT_FOR_TRIG;
    dacOutLlistDef.nextLinkDef   = 1;
    dacOutLlistDef.reserved      = 0;

    /* Initialize the Output Control Linked list */
    P716xInitDacOCtrlLList(&(dacOutLlistDef), &(moduleResrc->p716xRegs),
                           dacChan, 0);

/**************************************************************************
 LINK 1
**************************************************************************/

    /* Program Output Controller Linked List */
    dacOutLlistDef.delay     = 0x0000BB80;
    dacOutLlistDef.length    = 7;//(moduleResrc->progParams.xferSize) - 1;  /* 32K words */
    dacOutLlistDef.repeat    = 1;      /* Repeat = 1 round */
    dacOutLlistDef.ramOffset = 7;      /* Read data from beginning */
	dacOutLlistDef.ramLength = 7;
  /* Do not start immediately, wait for Trigger */
    dacOutLlistDef.continueImmed = \
        P716x_DAC_OCTRL_LLIST_CONTINUE_WAIT_FOR_TRIG;
    dacOutLlistDef.nextLinkDef   = 2;
    dacOutLlistDef.reserved      = 0;

    /* Initialize the Output Control Linked list */
    P716xInitDacOCtrlLList(&(dacOutLlistDef), &(moduleResrc->p716xRegs),
                           dacChan, 1);
/**************************************************************************
 LINK 2
**************************************************************************/

    /* Program Output Controller Linked List */
    dacOutLlistDef.delay     = 0x0000BB80;
    dacOutLlistDef.length    = 7;//(moduleResrc->progParams.xferSize) - 1;  /* 32K words */
    dacOutLlistDef.repeat    = 1;      /* Repeat = 1 round */
    dacOutLlistDef.ramOffset = 7;      /* Read data from beginning */
	dacOutLlistDef.ramLength = 7;
  /* Do not start immediately, wait for Trigger */
    dacOutLlistDef.continueImmed = \
        P716x_DAC_OCTRL_LLIST_CONTINUE_WAIT_FOR_TRIG;
    dacOutLlistDef.nextLinkDef   = 3;
    dacOutLlistDef.reserved      = 0;

    /* Initialize the Output Control Linked list */
    P716xInitDacOCtrlLList(&(dacOutLlistDef), &(moduleResrc->p716xRegs),
                           dacChan, 2);

/**************************************************************************
 LINK 3
**************************************************************************/

/* Program Output Controller Linked List */
    dacOutLlistDef.delay     = 1+Dac_delay;      /* 8 clock cycles */
    dacOutLlistDef.length    = RAM_LENGTH_VEC[PulseNum-1];
    dacOutLlistDef.repeat    = 1;      /* Repeat = 1 round */

    //dacOutLlistDef.ramOffset = (BLOCKSIZE*(PulseNum-1));      /* Start reading at waveform of interest */
	//dacOutLlistDef.ramLength = (moduleResrc->progParams.xferSize) - 1;

	dacOutLlistDef.ramOffset = RAM_OFFSET_VEC[PulseNum-1]; // VALID VALUES MUST BE MULTIPLES OF 8 MINUS 1
	printf("RAM_OFFSET_VEC(%i)=%i \n",PulseNum-1,RAM_OFFSET_VEC[PulseNum-1]);
	dacOutLlistDef.ramLength = RAM_LENGTH_VEC[PulseNum-1]; // VALID VALUES MUST BE MULTIPLES OF 8 MINUS 1
	printf("RAM_LENGTH_VEC(%i)=%i \n",PulseNum-1,RAM_LENGTH_VEC[PulseNum-1]);

    /* Do not start immediately, wait for Trigger */
    dacOutLlistDef.continueImmed = \
        P716x_DAC_OCTRL_LLIST_CONTINUE_WAIT_FOR_TRIG;
    dacOutLlistDef.nextLinkDef   = 3;
    dacOutLlistDef.reserved      = 0;

    /* Initialize the Output Control Linked list */
    P716xInitDacOCtrlLList(&(dacOutLlistDef), &(moduleResrc->p716xRegs),
                           dacChan, 3);
/**************************************************************************
 DAC OUTPUT LLIST DEFINITIONS END
**************************************************************************/
#endif


    /* Set Output Controller Linked List Start address */
    P716xSetDacOutCtrlLListStart(
        moduleResrc->p716xRegs.dacRegs[dacChan].outCtrllerLListStart, 0);

    /* Reset & Release DMA controller & the DMA FIFO  */
    P716xSetDacDmaCtrlDmaResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].dmaControl,
        P716x_DAC_DMA_CTRL_DMA_RESET);
    P716xSetDacDmaCtrlDmaResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].dmaControl,
        P716x_DAC_DMA_CTRL_DMA_RUN);

    /* Abort any existing transfers */
    P716xSetDacDmaCtrlDmaAdvanceState(
        moduleResrc->p716xRegs.dacRegs[dacChan].dmaControl,
        P716x_DAC_DMA_CTRL_DMA_ADV_WAIT);

    /* Set up DMA descriptor */
    dacDmaCword.readReqSize     = \
        P716x_DAC_DMA_CWORD_READ_REQ_SIZE_128;

    dacDmaCword.readReqSizeMode = \
        P716x_DAC_DMA_CWORD_READ_REQ_SIZE_MODE_AUTO;

    dacDmaCword.nextLinkIndx = 0;
    dacDmaCword.startMode = \
        P716x_DAC_DMA_CWORD_START_MODE_AUTO;
    dacDmaCword.linkEndIntr = \
        P716x_DAC_DMA_CWORD_LINK_END_INTR_DISABLE;
    dacDmaCword.chainEndIntr = \
        P716x_DAC_DMA_CWORD_CHAIN_END_INTR_ENABLE;
    dacDmaCword.chainEnd = \
        P716x_DAC_DMA_CWORD_END_OF_CHAIN_ENABLE;

    dacDmaDescriptor.linkCtrlWord = \
        P716xBuildDacDmaLListDescriptCword(&(dacDmaCword));

    // DP
    //dacDmaDescriptor.xferLength = \
    //    (moduleResrc->progParams.xferSize << 2);   /* In bytes */
	dacDmaDescriptor.xferLength = \
        (XFER_WORD_SIZE_DAC_DMA << 2);   /* In bytes */

    P716xSetDacDmaLListDescriptorAddress ((dmaBuf).kernBuf,
                                          &(dacDmaDescriptor.mswAddress),
                                          &(dacDmaDescriptor.lswAddress));

    /* Program the DMA descriptor Ram */
    P716xInitDacDmaLListDescriptor(&(dacDmaDescriptor),
                                   &(moduleResrc->p716xRegs),
                                   dacChan, 0);

    /* Reset & Release Capture Memory */
    P716xSetDacRamCtrlRamResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
        P716x_DAC_RAM_CTRL_RAM_RESET);
    P716xSetDacRamCtrlRamResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
        P716x_DAC_RAM_CTRL_RAM_RUN);

    /* Enable Ram Path */
    P716xSetDacRamCtrlRamPathEnable(
        moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
        P716x_DAC_RAM_CTRL_RAM_PATH_ENABLE);

    /* Enable Ram Read */
    P716xSetDacRamCtrlRamReadEnable(
        moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
        P716x_DAC_RAM_CTRL_RAM_READ_ENABLE);

    /* Do this for tm_myeDDR memory only */
    acqType = P716xGetDacChanStatPwrMgmntAcquireType(
                  moduleResrc->p716xRegs.dacRegs[dacChan].chanStatPowerMgmnt);
    if (acqType == P716x_DAC_CHAN_STAT_PWR_MGMNT_ACQ_TYPE_DDR)
    {
        /* Set the direction to 'WRITE' so data can be placed onto the
         * memory
         */
        P716xSetDacRamCtrlDDRDataDirection(
            moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
            P716x_DAC_RAM_CTRL_DDR_DATA_DIR_WRITE);

        /* This is the delay following a trigger issued by the user.
         * By default, the DAC Output Gate delay is set to 0x30, which
         * works fine with QDR memory.  But for DDR memory, this value
         * needs to be higher
         */
        P716xSetDacOutGateDelayValue(
            moduleResrc->p716xRegs.dacRegs[dacChan].outGateDelay,
            0x7F);
    }


    /* Clear all DAC interrupt flags; verify that Chain-End interrupt flag
     * is not set
     */
    P716xClearDacInterruptFlag(
        moduleResrc->p716xRegs.dacRegs[dacChan].interruptFlag,
        P716x_DAC_INTR_ALL);
    status = P716xGetDacInterruptFlag(
                 moduleResrc->p716xRegs.dacRegs[dacChan].interruptFlag,
                 P716x_DAC_INTR_CHAIN_END);
    if (status == P716x_DAC_INTR_CHAIN_END)
    {
        printf("Can't clear int flag\n");
                exit(0);
    }

    /* Start the DAC DMA; wait for Chain-End interrupt flag to be set */
    puts ("           Starting data transfer to RAM");
    P716xDacDmaStart(&(moduleResrc->p716xRegs), dacChan);
    do
    {
        status = P716xGetDacInterruptFlag(
                     moduleResrc->p716xRegs.dacRegs[dacChan].interruptFlag,
                     P716x_DAC_INTR_CHAIN_END);
        BaseboardDelayuS(10);
    } while (status != P716x_DAC_INTR_CHAIN_END);

    /* Confirm that All the Data is received */
    puts ("           Confirming data transfer");
    do
    {
        status = P716xGetDacDmaStatus(
                     moduleResrc->p716xRegs.dacRegs[dacChan].dmaStatus,
                     P716x_DAC_DMA_STAT_ALL_DATA_RCV);
        BaseboardDelayuS(10);
    } while (status != P716x_DAC_DMA_STAT_ALL_DATA_RCV);


    /* Clear Chain End interrupt flag (not required; for debug only) */
    P716xClearDacInterruptFlag(
        moduleResrc->p716xRegs.dacRegs[dacChan].interruptFlag,
        P716x_DAC_INTR_CHAIN_END);

#if 0
    /* Connect and Enable Interrupt */
    intrStat = PTK716X_intEnable(moduleResrc->hDev,
                                 (PTK716X_PCIE_INTR_DAC_ACQ_MOD1 << dacChan),
                                 P716x_DAC_INTR_TRIG_CMPLT,
                                 (PVOID)dacChan,
                                 trigIntHandler);
    if (intrStat != PTK716X_STATUS_OK)
    {
        exitHdlResrc.exitCode = 6;
        return (exitHandler(&(exitHdlResrc)));
    }
    puts("           Trigger Complete Interrupt is enabled");

    /* Create a semaphore for the Trigger Complete interrupt */
    if ((PTKIFC_SemaphoreCreate(&(ifcCurrent), dacChan)) < 0)
    {
        exitHdlResrc.exitCode = 7;
        return (exitHandler(&(exitHdlResrc)));
    }
#endif

    /* Reset & Release Out FIFO */
    P716xDacFifoFlush(&(moduleResrc->p716xRegs), dacChan);


    /* Release Output Control Linked List state machine
     *
     * Note: this loads the Output Control Linked-List parameters to the
     *       state machine
     */
    P716xSetDacGateTrigCtrlOutLListResetState(
        moduleResrc->p716xRegs.dacRegs[dacChan].gateTrigControl,
        P716x_DAC_GATE_TRIG_CTRL_OUT_CTRL_LLIST_RUN);

    /* set data direction to 'READ', needed for DDR memory only */
    if (acqType == P716x_DAC_CHAN_STAT_PWR_MGMNT_ACQ_TYPE_DDR)
    {
        P716xSetDacRamCtrlDDRDataDirection(
            moduleResrc->p716xRegs.dacRegs[dacChan].ramControl,
            P716x_DAC_RAM_CTRL_DDR_DATA_DIR_READ);
    }


#endif


    if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
        (moduleResrc->moduleId == P71741_MODULE_ID))
    {
        for (chan = P716x_ADC1; chan < numChans; chan++)
        {
            moduleResrc->p716xAdcParams[chan].dualEdgeSampling = \
                P716x_ADC_DATA_CTRL_DES_NONDES;
        }

        /*  Set up ADC12D1800 Parameters */
        adc12d1800Params.dualEdgeSampleMode   = \
            ADC12D1800_CONFIG_DES_DUALEDGE_DISABLE;
        adc12d1800Params.outputVoltageSelect  = \
            ADC12D1800_CONFIG_OVS_LVDS_HIGHERLVL;
        adc12d1800Params.fsrMagnitudeI        = \
            ADC12D1800_ICHANNEL_FULLSCALE_RANGE_800MV;
        adc12d1800Params.caliSequenceSelect   = \
            ADC12D1800_CALIBRATION_ADJUST_CSS_RESET;
        adc12d1800Params.maxPwrAdjust         = \
            ADC12D1800_BIAS_ADJUST_FULL_RATE;
        adc12d1800Params.fsrMagnitudeQ        = \
            ADC12D1800_QCHANNEL_FULLSCALE_RANGE_800MV;
        adc12d1800Params.selectCoarseAdjust   = \
            ADC12D1800_APERTUREDLY_COURSE_ADJUST_STA_DISABLE;
        adc12d1800Params.coarseAperAdjustMag  = \
            ADC12D1800_APERTUREDLY_COURSE_ADJUST_CAM_CLEAR;
        adc12d1800Params.dutyCycleCorrect     = \
            ADC12D1800_APERTUREDLY_COURSE_ADJUST_DCC_ENABLE;
        adc12d1800Params.selectFineAdjust     = 0;
        adc12d1800Params.fineAperAdjustMag    = 0;
        adc12d1800Params.disableReset         = \
            ADC12D1800_AUTOSYNC_DISABLE_RESET_DISABLE;
        adc12d1800Params.disableOutputRefClks = \
            ADC12D1800_AUTOSYNC_OUTPUTREF_CLK_DISABLE;
        adc12d1800Params.enableSlave          = \
            ADC12D1800_AUTOSYNC_MASTER_MODE_ENABLE;
        adc12d1800Params.selectPhase          = \
            ADC12D1800_AUTOSYNC_SELECT_PHASE_0;
        adc12d1800Params.dlyRefClk            = 0;
    }


    for (chan = P716x_ADC1; chan < numChans; chan++)
    {

        if (P716xInitAdcRegs(&moduleResrc->p716xAdcParams[chan],
                             &moduleResrc->p716xRegs, chan))
        {
            exitHdlResrc.exitCode[0] = 6;
            return (exitHandler(&exitHdlResrc));
        }

        if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
            (moduleResrc->moduleId == P71741_MODULE_ID))
        {
            if(chan == 0)      /* do this only once */
            {
                /* Initialize ADC12D1800 Chip Registers */
                ADC12D1800InitAdc12d1800Regs(
                    (unsigned int *)moduleResrc->p716xRegs.adcRegs[chan].serialAddr,
                    &(adc12d1800Params));
            }
        }

        P716xInitDdcRegs(&moduleResrc->p716xDdcParams[chan],
                         &moduleResrc->p716xDdcRegs,
                         chan,
                         moduleResrc->p716xGlobalParams.brdClkFreq);
    }

    if ((moduleResrc->moduleId != P71641_MODULE_ID) &&
        (moduleResrc->moduleId != P71741_MODULE_ID))
    {
        /* load FIR coefficient tables */
        for (chan = P716x_ADC1; chan < numChans; chan++)
        {
            /* FIR Stage 1 */
            status = PTKHLL_DdcLoadFilter(moduleResrc->moduleId,
                                          chan, P716x_DDC_STAGE1,
                                          &(moduleResrc->p716xDdcRegs),
                                          &(moduleResrc->p716xDdcParams[chan]));
            if (status != 0)
            {
                exitHdlResrc.exitCode[0] = 7;
                return (exitHandler(&exitHdlResrc));
            }

            /* FIR Stage 2, if in use */
            if (moduleResrc->p716xDdcParams[chan].firStage2 ==
                    P716x_DDC_CH_CTRL1_ST2_FIR_ENABLE)
            {
                status = PTKHLL_DdcLoadFilter(moduleResrc->moduleId,
                                              chan, P716x_DDC_STAGE2,
                                              &(moduleResrc->p716xDdcRegs),
                                              &(moduleResrc->p716xDdcParams[chan]));
                if (status != 0)
                {
                    exitHdlResrc.exitCode[0] = 7;
                    return (exitHandler(&exitHdlResrc));
                }
            }
        }
    }

    /* additional setup for 71641 core */
    if ((moduleResrc->moduleId == P71641_MODULE_ID) ||
        (moduleResrc->moduleId == P71741_MODULE_ID))
    {
        for (chan = P716x_ADC1; chan < numChans; chan++)
            ddc_71641_core_setup (moduleResrc, moduleResrc->progParams, chan);
    }

    /* toggle sync to synchronize all DDC channels */
    P716xPulseGenerate(moduleResrc->p716xRegs.syncAGenerate);


#if DAC
 /* Arm DAC trigger */
    P716xSetDacGateTrigCtrlTrigClearState(
        moduleResrc->p716xRegs.dacRegs[dacChan].gateTrigControl,
        P716x_DAC_GATE_TRIG_CTRL_TRIG_CLR_DEASSERT);
#endif



    /* dump register data to a file, if desired */
    if (REG_DUMP)
    {
        for (chan = P716x_ADC1; chan < numChans; chan++)
            regDump (moduleResrc, PROGRAM_ID, chan, tuningFreq,
                     decimation);
    }


		//*(unsigned int *)(moduleResrc->BAR0Base + 0x4034) = 0xf0000;

#if CONTINUOUS_TX
	while(1); //DP -- decomment for continuous TX
#endif

    puts ("                initialization done");



    /* Application section --------------------------------------------- */

    /* signal analyzer setup ------------------------------------------- */

    /* initialize the viewer if in use */
    if (useViewer[dispChannel])
    {
        puts("[ddc_multichan] Initializing Signal Analyzer TCP/IP socket");
        status = PTKHLL_ViewerInitIface(moduleResrc->moduleId, &sockFd,
                                        &newSockFd, &viewCtrlParams,
                                        &moduleResrc->progParams);
        if (status == 1)       /* invalid viewer params */
        {
            exitHdlResrc.exitCode[0] = 10;
            return (exitHandler (&exitHdlResrc));
        }
        if (status == 2)       /* connection to viewer not established */
        {
            exitHdlResrc.exitCode[0] = 11;
            return (exitHandler (&exitHdlResrc));
        }
    }

#if 1

    puts ("[ddc_multichan] data capture");

    /* start threads */
    puts ("                starting channel threads");
    for (chan = P716x_ADC1; chan < numChans; chan++)
    {
        /* create ready semaphore for this channel */
        if (PTKIFC_SemaphoreCreate(&ifcArgs, chan) < 0)
        {
            exitHdlResrc.exitCode[0] = 8;
            return (exitHandler (&exitHdlResrc));
        }

        /* set DMA thread parameters */
        dmaThreadParams[chan].moduleResrc  = moduleResrc;
        dmaThreadParams[chan].chanNum      = chan;
        dmaThreadParams[chan].dmaHandle    = NULL;
        dmaThreadParams[chan].dataBuf      = NULL;
        dmaThreadParams[chan].bufSize      = bufSize;
        //dmaThreadParams[chan].bufSize      = ddcBufSize;
        printf("Buffer size (in bytes):\t\t %d\n", dmaThreadParams[chan].bufSize);
	printf("Bytes to record per range line:\t %d\n", SAMPLES_PER_PRI_GLOBAL<<2);

	if (SAMPLES_PER_PRI_GLOBAL > XFER_WORD_SIZE) {
	    printf("ERROR: SAMPLES_PER_PRI in header file exceeds ADC buffer size.\n");
	    return 1;
	}

        dmaThreadParams[chan].ifcArgs      = &ifcArgs;

        dmaThreadParams[chan].useViewer    = useViewer[chan];
        if( (useViewer[chan]) == 1 )
        {
            dmaThreadParams[chan].sockFd       = &sockFd;
            dmaThreadParams[chan].newSockFd    = &newSockFd;
            dmaThreadParams[chan].viewCtrlParams = &viewCtrlParams;
        }
        else
        {
            dmaThreadParams[chan].sockFd       = NULL;
            dmaThreadParams[chan].newSockFd    = NULL;
            dmaThreadParams[chan].viewCtrlParams = NULL;
        }
        dmaThreadParams[chan].exitCodePtr = &(exitHdlResrc.exitCode[chan+1]);

	printf("Adc_delay = %d\n", Adc_delay);
        /* start thread */
        dwStatus = PTKIFC_ThreadCreate(&ifcArgs, chan,
                                       (void *)dmaThread,
                                       &(dmaThreadParams[chan]));
        if (dwStatus != 0)
        {
            exitHdlResrc.exitCode[0] = 9;
            return (exitHandler (&exitHdlResrc));
        }

    }

#if DEBUG
    if( (dwStatus = P716xReadAdcInterruptFlag(moduleResrc->p716xRegs.adcRegs[0].interruptFlag,
                                   P716x_ADC_INTR_BAD_TRIG_ACTIVE)) != 0 )
    {
        printf("Before trigger: Bad trigger! dwStatus is %x, pulseCount is %d, intrCount is %d\n",
               dwStatus, pulseCount, intrCount);
    }
#endif

    puts ("                waiting for ready signal from all threads");
    /* wait until all threads are ready */
    for (chan = P716x_ADC1; chan < numChans; chan++)
        PTKIFC_SemaphoreWait(&ifcArgs, chan, IFC_WAIT_STATE_FOREVER);
#endif


#if 0

#if (!TRIGGER)
    /* start data capture by toggling Gate A */
    P716xPulseGenerate(moduleResrc->p716xRegs.gateAGenerate);
    pulseCount++;
#endif

printf("stopflag = %d\n",stopFlag);

    while( (!PTKIFC_Kbhit()) && (!stopFlag) )
    {
#if (TRIGGER)
        if( (!stopFlag) && (pulseCount<=intrCount) )
        {
            /* start data capture by toggling Gate A */
            P716xPulseGenerate(moduleResrc->p716xRegs.gateAGenerate);
            pulseCount++;
        }
        else if( (dwStatus = P716xReadAdcInterruptFlag(moduleResrc->p716xRegs.adcRegs[0].interruptFlag,
                                       P716x_ADC_INTR_BAD_TRIG_ACTIVE)) != 0 )
        {
            printf("Bad trigger! dwStatus is %x, pulseCount is %d, intrCount is %d\n",
                   dwStatus, pulseCount, intrCount);
        }
#endif
        BaseboardDelayuS(10);

#if (TRIGGER)
        /* wait until all threads are ready */
        for (chan = P716x_ADC1; chan < numChans; chan++)
            PTKIFC_SemaphoreWait(&ifcArgs, chan,
                                 IFC_WAIT_STATE_MILSEC(NUM_DMA_BUFS*5000));
#endif
    } //end of while loop


    stopFlag |= 0xF0;

#if DEBUG
    printf("[ddc_multichan] pulse Count is %d\n", pulseCount);
    printf("[ddc_multichan] Interrupt Count is %d\n", intrCount );
    printf("[ddc_multichan] stopFlag is %x\n", stopFlag);
#endif
printf("File write complete.\n");

#endif
    for (chan = P716x_ADC1; chan < numChans; chan++)
        PTKIFC_ThreadWaitFinish(&ifcArgs, chan);

	/* clean up and exit */
    exitHdlResrc.exitCode[0] = 0;
    return (exitHandler (&exitHdlResrc));
}


/**************************************************************************
 Function:     PTKHLL_DeviceInit()

 Description:  This routine initializes the module address tables, resets
               the module and sets all module parameters to default values.

 Inputs:       pointer to module resource table
 Return:       0 = success
               1 = module does not support example program
**************************************************************************/
static int PTKHLL_DeviceInit (MODULE_RESRC *moduleResrc)
{
    DWORD chan;
    DWORD numChans;

	 DWORD count;


    /* get actual module ID */
    moduleResrc->moduleId = P716xGetFPGACodeTypeFPGAModuleId(\
        (volatile unsigned int *)(moduleResrc->BAR0Base +
                                  P716x_FPGA_CODE_TYPE));

    /* set BAR4 to NULL, for 78110 or Onyx modules */
    if ((moduleResrc->moduleId == P78110_MODULE_ID)      ||
        ((moduleResrc->moduleId & 0x000FFF00) == 0x71700)  )
    {
        moduleResrc->BAR4Base = (BAR_ADDR) NULL;
    }

    /* initialize register address and board resource tables */
    P716xInitRegAddr (moduleResrc->BAR0Base,
                      &moduleResrc->p716xRegs,
                      &moduleResrc->p716xBrdResrc,
                      moduleResrc->moduleId);

    /* as this is an DDC example program, verify this module has
     * a DDC core; if not, don't allow it in the list
     */
    if (moduleResrc->p716xBrdResrc.numDDC == 0)
    {
        return (1);
    }

	/* initialize IP core address table */
    P716xInitDdcRegAddr (moduleResrc->BAR2Base,
                         &moduleResrc->p716xDdcRegs);

#if RESET_REGS

	//NOTE: when using the 10MHz external reference, we get 5ns jitter between runs.
	// If the 200MHz external clock is used then commenting out P715xResetRegs will take away the run to run jitter. But between system power cycles there will still be 5ns of uncertainty.
    /* reset all registers to default values */
    P716xResetRegs (&moduleResrc->p716xRegs);
#endif

    /* load parameter tables with default values */
    P716xSetGlobalDefaults(&moduleResrc->p716xBrdResrc,
                           &moduleResrc->p716xGlobalParams);
    numChans = moduleResrc->p716xBrdResrc.numADC;
    for (chan = 0; chan < numChans; chan++)
    {
        P716xSetAdcDefaults(&moduleResrc->p716xBrdResrc,
                            &moduleResrc->p716xAdcParams[chan]);
        P716xSetDdcDefaults (&moduleResrc->p716xDdcParams[chan]);
    }

#if DAC
	  /* load DAC parameter tables with default values */
    for (count = 0; count < P716x_MAX_DAC_CHANS; count++)
        P716xSetDacDefaults(&(moduleResrc->p716xBrdResrc),
                            &(moduleResrc->p716xDacParams[count]));

    /* load DAC5688 parameter tables with default values */
    DAC5688SetParamsDefaults(&(moduleResrc->dac5688Params));
#endif

    return (0);
}


/**************************************************************************
 Function: selectDdcInput()

 Description: This routine sets the data source for both I and Q inputs
              and enables complex data input if required.  It uses input
              defines at the top of the program.

 Inputs:      chan    - the DDC channel being set.  Use defines:
                            P716x_ADC1 or P716x_DDC1
                            P716x_ADC2 or P716x_DDC2
                            P716x_ADC3 or P716x_DDC3
                            P716x_ADC4 or P716x_DDC4
              dataSrc - data source for this channel.  Use defines:
                            DDC_DATA_SRC_DEFAULT_ADC
                            DDC_DATA_SRC_ADC_1
                            DDC_DATA_SRC_ADC_2
                            DDC_DATA_SRC_ADC_3
                            DDC_DATA_SRC_ADC_4
                            DDC_DATA_SRC_TEST_GEN_REAL
                            DDC_DATA_SRC_TEST_GEN_CMPLX
              ddcParams - pointer to the DDC parameter table

 Returns:     none
**************************************************************************/
static void selectDdcInput(DWORD  dataSrc,
                           DWORD  chan,
                           unsigned int *iDataInputSel,
                           unsigned int *qDataInputSel,
                           unsigned int *cmplxInput)
{
    /* Select DDC data input source */
    if (dataSrc == DDC_DATA_SRC_DEFAULT_ADC)
    {
        /* select the "default" ADC for DDC data input */
        switch (chan)
        {
            case P716x_ADC1:
                *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC1;
            break;

            case P716x_ADC2:
                *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC2;
            break;

            case P716x_ADC3:
                *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC3;
            break;

            case P716x_ADC4:
                *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC4;
            break;
        }

        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else if (dataSrc == DDC_DATA_SRC_ADC_1)
    {

        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC2;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else if (dataSrc == DDC_DATA_SRC_ADC_2)
    {

        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC3;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else if (dataSrc == DDC_DATA_SRC_ADC_3)
    {

        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC4;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else if (dataSrc == DDC_DATA_SRC_ADC_4)
    {
        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_ADC4;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else if (dataSrc == DDC_DATA_SRC_TEST_GEN_REAL)
    {
        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_TEST_COS;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_ZERO;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_DISABLE;
    }
    else /* (dataSrc == DDC_DATA_SRC_TEST_GEN_CMPLX) */
    {
        *iDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_IDATA_SEL_TEST_COS;
        *qDataInputSel = P716x_DDC_INP_MUX_CTRL_DDC_QDATA_SEL_TEST_SIN;
        *cmplxInput    = P716x_DDC_INP_MUX_CTRL_CMPLX_INP_ENABLE;
    }
}


/**************************************************************************
 Function:    dmaThread()

 Description: This thread is created to handle a specified DMA channel.
              It initializes and acquires data on a single DMA channel.
 Parameters:  pParams - Address of
[transceiversystem@localhost examples]$
 the DMA_THREAD_PARAMS data structure
 Returns:     none
 Notes:       on error, an exit code and a stop flag are set before
              the control is return to main().
************************************************************************/
//static void dmaThread (PVOID pParams, int Adc_delay)
static void dmaThread (PVOID pParams)
{
    DMA_THREAD_PARAMS     *dmaParams    = (DMA_THREAD_PARAMS *)pParams;
    int                    chanNum      = dmaParams->chanNum;
    P716x_REG_ADDR        *p716xRegs    = &(dmaParams->moduleResrc->p716xRegs);
    P716x_GLOBAL_PARAMS   *globalParams = &(dmaParams->moduleResrc->p716xGlobalParams);
    PVOID                  hDev         = dmaParams->moduleResrc->hDev;
    DWORD                  bufSize      = dmaParams->bufSize;
    IFC_ARGS              *ifcArgs      = dmaParams->ifcArgs;
    unsigned int           loopCount    = dmaParams->moduleResrc->progParams.loop;
    //unsigned int           loopCount    = 5;

    P716x_ADC_TRIG_CTRL_LLIST_DEFINITION  trigLlistDef;
    P716x_ADC_DMA_LLIST_DESCRIPTOR        dmaDescriptor[NUM_DMA_BUFS];
    DWORD                  dwStatus;

    DWORD                  operand;
    int                    status;
    unsigned int           i;
	FILE                  *outfile;
	char                   outfileName[40];



	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);


//	sprintf (outfileName, "%d_%d_%d_%d_%d_%d_adc%ddata.dat",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,chanNum);
	//sprintf (outfileName, "./data/%d_%d_%d_%d_%d_%d_adc%ddata.dat",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,chanNum);
	//sprintf (outfileName, "/smbtest/%d_%d_%d_%d_%d_%d_adc%ddata.dat",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,chanNum);
	sprintf (outfileName, "///smbtest/adc%d.dat",chanNum);
//	sprintf (outfileName, "%d_%d_%d_%d_%d_%d_adc%ddata.dat",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,chanNum);
	outfile = fopen(outfileName, "wb"); //DP Change directory
	//system(sprintf("cp Nextradheader.txt ThisExperiment%s ", outfilename);


    /* Trigger Setup --------------------------------------------------- */

    /* set trigger clear bit in the Gate Trigger Control register */
    P716xSetAdcGateTrigCtrlTriggerClearState(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_CLR_RESET);

    /* enable trigger mode in the Gate Trigger Control register */
    P716xSetAdcGateTrigCtrlTriggerMode(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_MODE_TRIG);


    /* put trigger linked list state machine in reset in the Gate Trigger
     * Control register
     */
    P716xSetAdcGateTrigCtrlTrigLinkListState(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_LLIST_RESET);

    /* program the trigger linked list definitions; the linkCtrl parameter
     * consists a next link index (zero here) and a next link control.
     * They are OR'ed as shown in the code below.
     *
     * Note that the delay is set to 4, the minimum delay for the 71650
     * module.  It may be set lower for other modules.
     */
    if ((dmaParams->moduleResrc->moduleId == P71640_MODULE_ID) ||
        (dmaParams->moduleResrc->moduleId == 0x71641)          ||
        (dmaParams->moduleResrc->moduleId == 0x71740)          ||
        (dmaParams->moduleResrc->moduleId == 0x71741))
    {
        trigLlistDef.delay    = 100;
        trigLlistDef.length   = (bufSize * 2) - 1;
    }
    else
    {

	if (Adc_delay < 10) {
		printf("Minimum ADC_DELAY is 10 (at 90MSPS)\n");
		trigLlistDef.delay = 10;
	}
	else {
		trigLlistDef.delay    = Adc_delay;
	}
        trigLlistDef.length   = ((NUM_DMA_BUFS*bufSize)>>2) - 1; //NUM_DMA_BUFS*bufSize - 1; FMP
    }


#if (TRIGGER)
    trigLlistDef.repeat   = 1;
#else
    trigLlistDef.repeat   = 0;
	trigLlistDef.repeat   = 1; // DPP
#endif

    trigLlistDef.linkCtrl = 0 |
                            P716x_ADC_ICTRL_LLIST_NEXT_LINK_CTRL_TRIG;

    /* apply link definition to link memory */
    P716xInitAdcTrigCtrlLListLink(&trigLlistDef, p716xRegs, chanNum, 0);

    /* set the linked list start index */
    P716xSetAdcTrigLinkedListStart(
        p716xRegs->adcRegs[chanNum].trigCtrlLListStart, 0);

    /* return trigger clear bit to zero */
    P716xSetAdcGateTrigCtrlTriggerClearState(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_CLR_RUN);

    /* release trigger linked list state machine from reset */
    P716xSetAdcGateTrigCtrlTrigLinkListState(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_LLIST_RUN);


    /* DMA setup ------------------------------------------------------- */

    /* open a DMA channel */
    dwStatus = PTK716X_DMAOpen(hDev, chanNum, &(dmaParams->dmaHandle));
    if (dwStatus != PTK716X_STATUS_OK)
    {
        printf("[dmaThread %d] DMA channel open error \n", chanNum+1);
        *(dmaParams->exitCodePtr) = 16;
  //      stopFlag = 1<<chanNum;
        return;
    }

    for (i = 0; i < NUM_DMA_BUFS; i++)
    {
        /* allocate a DMA buffer */
        dwStatus = PTK716X_DMAAllocMem(dmaParams->dmaHandle, bufSize, &(dmaParams->dmaBuf[i]), TRUE);
        if (dwStatus != PTK716X_STATUS_OK)
        {
            printf("[dmaThread %d] DMA Buffer allocation error \n", chanNum+1);
            *(dmaParams->exitCodePtr) = 5;
    //        stopFlag = 1<<chanNum;
            return;
        }
        memset (dmaParams->dmaBuf[i].usrBuf, 0x5a, bufSize);
    }

    dmaParams->dataBuf = (int *)malloc(NUM_DMA_BUFS*bufSize);
    if (dmaParams->dataBuf == NULL)
    {
        printf("[dmaThread %d] memory allocation error\n", chanNum+1);
        *(dmaParams->exitCodePtr) = 5;
   //     stopFlag = 1<<chanNum;
        return;
    }


    /* reset the DMA linked list engine and FIFO */
    P716xAdcDmaReset(p716xRegs, chanNum);


    for (i = 0; i < NUM_DMA_BUFS; i++)
    {
        if( i == 0 )   /* The first descriptor */
        {
            if( NUM_DMA_BUFS == 1)
            {
                /* the thread uses a single descriptor */
                dmaDescriptor[i].linkCtrlWord =
                      ((0 << P716x_ADC_DMA_CWORD_NEXT_LINK_ADDR_OFFSET) |
                       P716x_ADC_DMA_CWORD_START_MODE_AUTO              |
                       P716x_ADC_DMA_CWORD_LINK_END_INTR_ENABLE         |
                       P716x_ADC_DMA_CWORD_END_OF_CHAIN_DISABLE);
            }
            else
            {
                /* the thread uses multiple descriptors */
                dmaDescriptor[i].linkCtrlWord =
                      (((i+1) << P716x_ADC_DMA_CWORD_NEXT_LINK_ADDR_OFFSET) |
                       P716x_ADC_DMA_CWORD_START_MODE_AUTO                  |
                       P716x_ADC_DMA_CWORD_LINK_END_INTR_ENABLE);
            }

        }
        else if( i != (NUM_DMA_BUFS-1) )
        {
            /* the thread uses a single descriptor */
            dmaDescriptor[i].linkCtrlWord =
                  (((i+1) << P716x_ADC_DMA_CWORD_NEXT_LINK_ADDR_OFFSET) |
                   P716x_ADC_DMA_CWORD_START_MODE_AUTO                  |
                   P716x_ADC_DMA_CWORD_LINK_END_INTR_ENABLE);
        }
        else  /* Last descriptor */
        {
            /* the thread uses a single descriptor */
            dmaDescriptor[i].linkCtrlWord =
                  ((0 << P716x_ADC_DMA_CWORD_NEXT_LINK_ADDR_OFFSET) |
                   P716x_ADC_DMA_CWORD_START_MODE_AUTO              |
                   P716x_ADC_DMA_CWORD_LINK_END_INTR_ENABLE         |
                   P716x_ADC_DMA_CWORD_END_OF_CHAIN_DISABLE);
        }

        dmaDescriptor[i].xferLength   = bufSize;
        P716xSetAdcDmaLListDescriptorAddress (dmaParams->dmaBuf[i].kernBuf,
                                          &dmaDescriptor[i].mswAddress,
                                          &dmaDescriptor[i].lswAddress);

        /* Apply descriptor parameters to descriptor memory link */
        P716xInitAdcDmaLListDescriptor(&dmaDescriptor[i], p716xRegs, chanNum, i);
    }

    /* reset the DMA linked list engine */
    P716xAdcDmaReset(p716xRegs, chanNum);

    /* Put Input DMA FIFO in reset */
    P716xSetAdcDmaCtrlDmaInFifoResetState(
         p716xRegs->adcRegs[chanNum].dmaControl,
         P716x_ADC_DMA_CTRL_DMA_INPUT_FIFO_RESET);


    /* enable the DMA interrupt */
    PTKIFC_MutexLock(ifcArgs, 0, (unsigned long)IFC_WAIT_STATE_FOREVER);
    dwStatus = PTK716X_intEnable(
                   hDev,
                   (PTK716X_PCIE_INTR_ADC_ACQ_MOD1 << chanNum),
                   P716x_ADC_INTR_LINK_END,
                   (PVOID)(ifcArgs),
                   dmaIntHandler);
    PTKIFC_MutexUnlock(ifcArgs, 0);
    if (dwStatus != PTK716X_STATUS_OK)
    {
        printf("[dmaThread %d] Interrupt Enabling error\n", chanNum+1);
        *(dmaParams->exitCodePtr) = 15;
 //       stopFlag = 1<<chanNum;
        return;
    }

    /* Create a DMA Complete semaphore for this DMA channel */
    if (PTKIFC_SemaphoreCreate (ifcArgs, 4 + chanNum) < 0)
    {
        printf("[dmaThread %d] Semaphore Creation error\n", chanNum+1);
        *(dmaParams->exitCodePtr) = 8;
 //       stopFlag = 1<<chanNum;
        return;
    }


    /* dump register data to a file, if desired */
    if ( REG_DUMP )
    {
        char   fileName[40];

        sprintf (fileName, "%s_dmaThread%d_beforeDMAStart", PROGRAM_ID, chanNum);
        regDump (dmaParams->moduleResrc, fileName, chanNum,
                 dmaParams->moduleResrc->progParams.tuneFreq,
                 dmaParams->moduleResrc->progParams.decimation);
    }

    /* Flush the CPU caches (see documentation of WDC_DMASyncCpu()) */
    PTK716X_DMASyncCpu(dmaParams->dmaBuf);

    /* Flush the ADC Input FIFOs */
    P716xAdcFifoFlush(p716xRegs, chanNum);

    /* Clear all interrupt flags */
    P716xClearAdcInterruptFlag(p716xRegs->adcRegs[chanNum].interruptFlag,
                               P716x_ADC_INTR_REG_MASK);

    /* Release Input DMA FIFO Reset */
    P716xSetAdcDmaCtrlDmaInFifoResetState(
         p716xRegs->adcRegs[chanNum].dmaControl,
         P716x_ADC_DMA_CTRL_DMA_INPUT_FIFO_RUN);


    /* Flush the CPU caches */
    PTK716X_DMASyncCpu(dmaParams->dmaBuf);

    printf("[dmaThread %d] will run ", chanNum+1);
    if (loopCount != 0)
    {
        operand = 1;
        printf("for %d loops, or until a key is hit...\n\n",
               loopCount);
    }
    else
    {
        loopCount = 1;
        operand   = 0;
        puts("forever, until a key is hit...\n");
    }


    /* start DMA */
    P716xAdcDmaStart(p716xRegs, chanNum);


    /* dump register data to a file, if desired */
    if ( REG_DUMP )
    {
        char   fileName[40];
        sprintf (fileName, "%s_dmaThread%d_afterDMAStart", PROGRAM_ID, chanNum);
        regDump (dmaParams->moduleResrc, fileName, chanNum,
                 dmaParams->moduleResrc->progParams.tuneFreq,
                 dmaParams->moduleResrc->progParams.decimation);
    }

    /* release semaphore to indicate "ready" to main() */
    PTKIFC_SemaphorePost(ifcArgs, chanNum);

/* like this: fopen(const char *filename, const char *mode) */
// FILE *outfile1[3]; // three file pointers
// outfile[0] = fopen("ADC1.dat", "wb");
// outfile[1] = fopen("ADC2.dat", "wb");
// outfile[2] = fopen("ADC3.dat", "wb");

    // Display loopCount
    //if(chanNum == 1) printf("PULSES GENERATED:  \n");
    //int totalPulses = loopCount;
//    while( (loopCount) && (!stopFlag) )
    while( (loopCount)  )

    {
        // Display loopCount
	//if(chanNum == 0 && loopCount%100 == 0) printf("%*i",10, loopCount);
	//if(chanNum == 1) printf("\r%*i  %*.1f %%",10 , totalPulses-loopCount,10, (float)(totalPulses-loopCount)/(float)(totalPulses)*100.0);

	for (i = 0; i < NUM_DMA_BUFS; i++)
        {
            /* Wait for DMA to Complete (interrupt signal) */
            status = PTKIFC_SemaphoreWait (ifcArgs, (4 + chanNum),
                                           IFC_WAIT_STATE_MILSEC(1000000)); //DP

	    //if(chanNum == 1 && i == 0) printf("\r%*i  %*.1f %%",10 , totalPulses-loopCount,10, (float)(totalPulses-loopCount)/(float)(totalPulses)*100.0);
	    if (status != PTK716X_STATUS_OK)
            {
                /* semaphore timeout */
                printf("[dmaThread %d] Semaphore timeout \n", chanNum+1);
                *(dmaParams->exitCodePtr) = 17;
    //            stopFlag = 1<<chanNum;
                return;
            }

            /* Flush the I/O caches */
            PTK716X_DMASyncIo(&dmaParams->dmaBuf[i]);

            /* copy captured data to ADC data buffer */
 //           memcpy (dmaParams->dataBuf+((i*bufSize)>>2), dmaParams->dmaBuf[i].usrBuf, bufSize);

			//fwrite(dmaParams->dmaBuf[i].usrBuf, 1, bufSize, outfile);
			// OR
			//bufSize = SAMPLES_PER_PRI_GLOBAL*4;
			// OR
			//fwrite(dmaParams->dmaBuf[i].usrBuf, 1, bufSize, outfile);
			// OR
			fwrite(dmaParams->dmaBuf[i].usrBuf, 1, SAMPLES_PER_PRI_GLOBAL*4, outfile);

#if (TRIGGER)
            /* release semaphore to indicate "ready" to main() */
            PTKIFC_SemaphorePost(ifcArgs, chanNum);
#endif

            /* if Signal Analyzer is enabled display the first DMA buffer */
            if ( (dmaParams->useViewer) )

            {
                /* dispChannel is an isolated variable specific to the viewer */
                status = PTKHLL_ViewerSendData(*(dmaParams->newSockFd),
                                               dmaParams->dataBuf,
                                               VIEW_BLK_SIZE,
                                               dmaParams->viewCtrlParams);
                if (status != 0)
                {
                    printf("[dmaThread %d] Failure to send data to viewer\n", chanNum+1);
                    *(dmaParams->exitCodePtr) = 12;
    //                stopFlag = 1<<chanNum;
                    break;
                }
            }
        }  /* end for() */

        loopCount = loopCount - operand;
	// Display loopCount
	//if(chanNum == 0 && loopCount%10 == 0) printf("\r");
	//if(chanNum == 0 && loopCount%100 == 0) printf("\r");

    }  /* end while() */

    // Display loopCount
    // printf("\n");

    fclose(outfile);

    /* Clear Trigger */
    P716xSetAdcGateTrigCtrlTriggerClearState(
        p716xRegs->adcRegs[chanNum].gateTriggerControl,
        P716x_ADC_GATE_TRIG_CTRL_TRIG_CLR_RESET);

    P716xAdcDmaAbort(p716xRegs, chanNum);

    /* Disable DMA Interrupt */
    PTKIFC_MutexLock(ifcArgs, 0, (unsigned long)IFC_WAIT_STATE_FOREVER);
    PTK716X_intDisable(hDev,
                       (PTK716X_PCIE_INTR_ADC_ACQ_MOD1 << chanNum),
                       P716x_ADC_INTR_LINK_END);
    PTKIFC_MutexUnlock(ifcArgs, 0);


    /* close viewer socket connections if in use */
    if ( dmaParams->useViewer )
    {
        P716xViewCloseSock(dmaParams->sockFd, dmaParams->newSockFd);
        printf("[dmaThread %d] Please close signal viewer program manually\n", chanNum+1);
    }

    printf("[dmaThread %d] Interrupt Disabled\n\n", chanNum+1);


 //   if (FILE_SAVE)
		    if (0)

    {
        printf("[dmaThread %d] saving captured data buffer(s)...\n", chanNum+1);

        status = 0;

        status |= PTKHLL_WriteBufToFile (dmaParams->dataBuf, bufSize*NUM_DMA_BUFS,
                                         dmaParams->moduleResrc->moduleId,
                                         PROGRAM_ID, chanNum,
                                         dmaParams->moduleResrc->progParams.devType,
                                         dmaParams->moduleResrc->progParams.datFormat);
        if (status == 0)
        {
            PTKHLL_ScriptUsage(bufSize, PROGRAM_ID,
                               dmaParams->moduleResrc->progParams.devType,
                               chanNum, dmaParams->moduleResrc->progParams.datFormat);
        }
        else
        {
            printf ("[dmaThread %d]   Warning: write to save file failed\n", chanNum+1);
            *(dmaParams->exitCodePtr) = 14;
      //      stopFlag = 1<<chanNum;
            return;
        }
    }

 //   stopFlag = 1<<chanNum;

    /* Exit Thread */
    PTKIFC_ThreadExit(ifcArgs);

    return;
}

/**************************************************************************
 Function: exitHandler

 Description:  This routine outputs an appropriate program exit message and
               cleans up before program exit.

 Inputs:       exitCode - exit code, used to select message

 Return:       exit code
**************************************************************************/
static int exitHandler (EXIT_HANDLE_RESRC *ehResrc)
{
    DWORD cntr;
    int   msgCnt;
    int   index;

    /* count messages in exitMsg */
    msgCnt = 0;
    while (exitMsg[msgCnt] != NULL)
        msgCnt++;
    msgCnt--;

    /* display message */
    for( cntr=0; cntr<((*(ehResrc->numChans))+1); cntr++ )
    {
        index = ehResrc->exitCode[cntr];
        if (index >= msgCnt)
            index = msgCnt;
        if (strlen(exitMsg[index]) > 0)
        {
            if( cntr == 0)
                printf("[exitHandler main()] ");
            else
                printf("[exitHandler thread(%d)] ", cntr);

            printf ("%s\n", exitMsg[index]);
        }
    }


    /* free buffers */
    for (cntr = 0; cntr < (*ehResrc->numChans); cntr++)
    {
        if( (*(ehResrc->dataBufPtr[cntr])) != NULL )
            free(*(ehResrc->dataBufPtr[cntr]));
    }

    for (cntr = 0; cntr < (*ehResrc->numChans); cntr++)
    {
        for (index = 0; index < NUM_DMA_BUFS; index++)
        {
            if( ((*(ehResrc->dmaBufPtr[cntr][index])).usrBuf) != NULL )

                /* Free DMA Buffer */
                PTK716X_DMAFreeMem(*(ehResrc->dmaHandlePtr[cntr]),
                                   (ehResrc->dmaBufPtr[cntr][index]));
        }
        /* Close DMA Channel */
        if( (*(ehResrc->dmaHandlePtr[cntr])) != NULL )
            PTK716X_DMAClose((ehResrc->modResrcBase->hDev),
                             *(ehResrc->dmaHandlePtr[cntr]));
    }

    /* check if library was initialized */
    if (*(ehResrc->libStatPtr) == PTK716X_STATUS_OK)
    {
         /* close all open Pentek devices */
 //       PTKHLL_DeviceClose(ehResrc->modResrcBase);

        /* library was initialized, uninit now */
 //       PTK716X_LibUninit();

        /* Release all OS-dependent resources */
        PTKIFC_UnInit((ehResrc->ifcArgs));
    }

#ifdef _DEBUG
    puts ("                press ENTER to exit...\n");
    getchar();
#endif

    return ((ehResrc->exitCode[0]));
}


/**************************************************************************
 Function: regDump()

 Description: This routine write major program parameters and register
              settings to ddc_multichan_regs.txt in the ReadyFlow data
              directory.

 Inputs:      moduleResrc - pointer to MODULE_RESRC, a module resources structure.

 Returns:     0 - successful
              1 - output file failed to open
**************************************************************************/
static int regDump (MODULE_RESRC *moduleResrc,
                    char         *progId,
                    DWORD         channel,
                    double        tuningFreq,
                    DWORD         decimation)
{
    char   outfile[180];
    FILE  *regdumpfile;


    /* get path to data directory */
    PTKIFC_GetInstallPath(&outfile[0], moduleResrc->moduleId);
    strcat (outfile, IFC_DATA_PATH);
    strcat (outfile, progId);
    strcat (outfile, "_regs.txt");

    /* open file */
    regdumpfile = fopen (outfile, "w");
    if (regdumpfile == NULL)
        return (1);            /* file failed to open */

    /* dump program conditions */
    fprintf (regdumpfile, "[%s] Debug Register Dump\n", progId);
    fprintf (regdumpfile, "    Initial conditions:\n");
    fprintf (regdumpfile, "        Tuning Frequency = %f\n",
             tuningFreq);
    fprintf (regdumpfile, "        Total Decimation = %d\n",
             decimation);

    /* dump register contents */
    P716xGlobalRegDump(&moduleResrc->p716xRegs, regdumpfile);
    P716xPcieRegDump(&moduleResrc->p716xRegs, regdumpfile);
    P716xBoardIdRegDump(&moduleResrc->p716xRegs, regdumpfile);

    P716xAdcRegDump(&moduleResrc->p716xRegs, channel, regdumpfile);
    P716xAdcDmaLListDescriptorDump(&(moduleResrc->p716xRegs), channel, 0,
                                   NUM_DMA_BUFS-1, regdumpfile);
    P716xAdcTrigCtrlLListDump(&(moduleResrc->p716xRegs), channel,
                              0, 0, regdumpfile);

    P716xDdcChanRegDump(&moduleResrc->p716xDdcRegs,
                        &moduleResrc->p716xBrdResrc,
                        channel, regdumpfile);

    fclose (regdumpfile);

    return (0);
}


/**************************************************************************
 Function: ddc_71641_core_setup()

 Description: This routine configures specific DDC setup on the 71641/71741.

 Inputs:      modResrcBase - pointer to the MODULE_RESRC structure
              progParams   - pointer to the P716x_CMDLINE_ARGS structure
              chanNum - Channel number

 Returns:     None

**************************************************************************/
void ddc_71641_core_setup (MODULE_RESRC       *modResrcBase,
                           P716x_CMDLINE_ARGS  progParams,
                           unsigned int        chanNum)
{
    unsigned int tuning_freq = 0;

    if (progParams.clockFreq == 0xc01df00d)
    {
        progParams.clockFreq = 200000000.00;
    }

    tuning_freq = (unsigned int) (4294967296.0 *
                                  (progParams.tuneFreq /
                                  (progParams.clockFreq)));

	switch(progParams.decimation){
		case(4):
			modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].decimation=P716x_71641_DDC_CH_DEC_4_OR_8;
			break;

		case(8):
			modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].decimation=P716x_71641_DDC_CH_DEC_8_OR_16;
			break;

		case(16):
			modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].decimation=P716x_71641_DDC_CH_DEC_16_OR_32;
			break;

		default:
			modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].decimation=P716x_71641_DDC_CH_DEC_4_OR_8;
			printf("ddc_multichan defaults to NON-DESMODE.  Valid decimation values in this mode are 4, 8 and 16\n");

	}//end of switch statement

    P716xSet71641DdcChanDecimation(
        modResrcBase->p716xDdcRegs.channel[chanNum].chanDecimation,
        modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].decimation);

    P71641_Set_Ddc_St1_Gain(
        modResrcBase->p716xDdcRegs.channel[chanNum].stage1FirGain,
        modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE1].st1_gain);

    P71641_Set_Ddc_St2_Gain(
        modResrcBase->p716xDdcRegs.channel[chanNum].stage2FirGain,
        modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE2].st2_gain);

    P71641_Set_Ddc_St3_Gain(
        modResrcBase->p716xDdcRegs.channel[chanNum].stage3FirGain,
        modResrcBase->p716xDdcParams[chanNum].stage[P716x_DDC_STAGE3].st3_gain);

    P716xSetDdcTuningFreqWord(
        modResrcBase->p716xDdcRegs.channel[chanNum].tuningFreq,
        tuning_freq);

    P716xSetDdcCoreCtrlCoreResetState(
        modResrcBase->p716xDdcRegs.channel[chanNum].coreCtrl,
        P716x_DDC_CORE_CTRL_CORE_RESET);

    P716xSetDdcCoreCtrlCoreResetState(
        modResrcBase->p716xDdcRegs.channel[chanNum].coreCtrl,
        P716x_DDC_CORE_CTRL_CORE_RUN);

    printf("\n\n");

    return;
}

/**************************************************************************
 Function: PTKHLL_SetProgramOptions()

 Description: This routine sets the Command Line Argument structure to
              program defaults.  It contains all possible parameters.
              Unused parameters are set to P716x_CMDLINE_UNSUPPORTED_ARG.

 Inputs:      argParams - pointer to P716x_CMDLINE_ARGS structure.

 Returns:     none

 Example:
              716x_CMDLINE_ARGS argParams;

              setProgramOptions(&argParams);
**************************************************************************/
static void PTKHLL_SetProgramOptions (P716x_CMDLINE_ARGS   *argParams,
                                      P716x_BOARD_RESOURCE *BrdResrc)
{
    argParams->devType       = P716x_DDC;
    argParams->channel       = ACTIVE_CHANNEL+1;
    argParams->dataSrc       = P716x_CMDLINE_UNSUPPORTED_ARG;


    //argParams->loop          = NUM_TRANSFERS; // get this from the parser


    // Parser
	// closes program if file can't be found
    configuration config;

	//if (ini_parse("///smbtest/NeXtRAD_Header.txt", handler, &config) < 0) {
	if (ini_parse("///smbtest/NeXtRAD.ini", handler, &config) < 0) {
		printf("Can't load in file.'\n");
		return 1;
    }
	argParams->loop          = config.NUM_TRANSFERS;
	    printf("PARSER: NUM_PRIS = %i\n", argParams->loop);

    argParams->clockFreq     = BrdResrc->adcDefaultFreq;
    argParams->refFreq       = P716x_CMDLINE_UNSUPPORTED_ARG;
    argParams->viewPort      = P716x_CMDLINE_BAD_ARG;   /* invalid port# */
    strcpy(&(argParams->viewServAddr[0]), "localhost");
    argParams->decimation    = DDC_DECIMATION;
    argParams->tuneFreq      = TUNING_FREQ;
    argParams->datFormat     = P716x_CMDLINE_FILE_FORMAT_BIN;

#if DAC
//	argParams->devType       = P716x_DAC;
//    argParams->channel       = ACTIVE_CHANNEL + 1; /* start from Chan. 1 */
    argParams->xferSize      = XFER_WORD_SIZE;
    argParams->rateDiv       = 1;
    argParams->interpolation = 2;
    argParams->clockSel      = CLOCK_SOURCE;
    argParams->clockFreq     = BrdResrc->dacDefaultFreq;
#endif

}







/**************************************************************************
 Function: setViewerControlWord()

 Description: This routine sets the selected elemenets in the Viewer Control
              Argument structure with program default values.

              Viewer Control parameters are set as follows:
                  board         - set to board ID in hex, ie: 0x71620
                  clock         - sample clock, a float, in Mhz
                  decimation    - channel divide rate, or DDC decimation
                  packingMode   - 2 = 16-bit time-packed
                  channelType   - 0 = ADC, 1 = DDC
                  centerFreq    - a float, in Mhz
                  voltageLevel  - 0 = 4dBm, 1 = 10dBm, 2 = 8dBm
                  blockSize     - number bytes to be sent to viewer
                  realComplex   - 0 = complex
                  adcResolution - number of bits

 Inputs:      boardId    - module ID
              viewCtrl   - pointer to the P716x_VIEW_CONTROL signal viewer
                           control structure.
              progParams - pointer to the command line structure.

 Returns:     none

 Example:
              P716x_CMDLINE_ARGS argParams;
              P716x_VIEW_CONTROL viewCtrlWord;

              setViewerControlWord(0x71620, &viewCtrlWord, &argParams);
**************************************************************************/
static void PTKHLL_ViewerSetControlWord (DWORD               boardId,
                                         P716x_VIEW_CONTROL *viewCtrl,
                                         P716x_CMDLINE_ARGS *progParams)
{
    viewCtrl->board          = boardId;
    viewCtrl->clock          = (float)((progParams->clockFreq) / 1.0e6);
    viewCtrl->packingMode    = 2;      /* 2 = 16-bit time packed */
    if (DDC_BYPASS == 0)               /* using theDDC core */
    {
        viewCtrl->decimation   = progParams->decimation;
        viewCtrl->channelType  = progParams->devType;
        viewCtrl->centerFreq   = (float)(progParams->tuneFreq / 1000000.0);
        viewCtrl->realComplex  = 0;
    }
    else                               /* bypassing the DDC */
    {
        viewCtrl->decimation   = 1;
        viewCtrl->channelType  = P716x_ADC;
        viewCtrl->centerFreq   = (float)(0.0);
        viewCtrl->realComplex  = 0;
    }
    viewCtrl->voltageLevel = 2;        /* 0 = 4dBm, 1 = 10dBm, 2 = 8dBm */
    viewCtrl->blockSize      = VIEW_BLK_SIZE;
    viewCtrl->adcResolution  = 16;
}

