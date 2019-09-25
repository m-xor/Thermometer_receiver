#include "qpc.h"
#include "bsp.h"

#include "BSP_inits.h"
#include "device_config.h"
#include "project.h"
#include "timer.h"
#include "rerror.h"
#include "err.h"
#include "gpio.h"
#include "iwdg.h"
#include "VirtualWire.h"


#include "stm32f0xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelAwareISRs {
    VW_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    SYSTICK_PRIO,
    ADC_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

void SysTick_Handler(void);
void assert_failed(uint8_t * file, uint32_t line);

/* Local-scope defines -----------------------------------------------------*/

static uint32_t timestamp;	/* timestamp counter */

#ifdef Q_SPY
    QSTimeCtr QS_tickTime_;
    QSTimeCtr QS_tickPeriod_;

    /* event-source identifiers used for tracing */
    static uint8_t const l_bsp_obj = 0U;

#endif

/* ISRs used in the application ==========================================*/
void SysTick_Handler(void) {   /* system clock tick ISR */

	/* state of the button debouncing, see below */
  static struct ButtonsDebouncing {
      uint16_t depressed;
      uint16_t previous;
  } buttons = { UINT16_MAX, UINT16_MAX };
  uint16_t current;
  uint16_t tmp;

#ifdef Q_SPY
    {
        tmp = SysTick->CTRL; /* clear CTRL_COUNTFLAG */
        QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
    }
#endif

    QF_TICK_X(HARD_TICKRATE_1, &l_bsp_obj); /* perform the QF clock tick processing */

    //perform software clock tick
    static uint16_t soft_timer_div = BSP_TICKS_PER_SEC/BSP_SOFT_TIMER_TICKS_PER_SEC;
    if(!--soft_timer_div)
    {
    		QACTIVE_POST(the_Ticker0, 0, 0);
    		soft_timer_div = BSP_TICKS_PER_SEC/BSP_SOFT_TIMER_TICKS_PER_SEC;
    }

    /* get state of the user button */
    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = (uint16_t)~GPIO(BUTTON_GPIO)->IDR; /* read Port C with the state of Button B1 */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & (1<<BUTTON_PIN)) != 0U) {  /* debounced B1 state changed? */
        if ((buttons.depressed & (1<<BUTTON_PIN)) != 0U) { /* is B1 depressed? */
            QF_PUBLISH(Q_NEW(QEvt, BTN_PRSS_SIG), &l_bsp_obj);
        }
        else {            /* the button is released */
            QF_PUBLISH(Q_NEW(QEvt, BTN_REL_SIG), &l_bsp_obj);
        }
    }

    static uint8_t timestamp_div = BSP_TICKS_PER_SEC/BSP_TIMESTAMP_TICKS_PER_SEC;

    if(!--timestamp_div)
    {
 		   timestamp++;
 		   timestamp_div = BSP_TICKS_PER_SEC/BSP_TIMESTAMP_TICKS_PER_SEC;
    }
}
/*..........................................................................*/
static inline uint16_t compute_adc(uint16_t *tab, uint8_t cnt)
{
	unsigned int res = 0;

	for(uint8_t i=0; i<cnt; i++)
		res += tab[i];

	return (uint16_t)(res/cnt);
}

void ADC1_IRQHandler(void)
{
	static int8_t read_idx = -1;
	static uint16_t readout[ADC_OVS];
	static uint16_t const brightness_lut[] = {
#include "adc_ldr_comp_64.txt"
	};

	if((ADC1->ISR & ADC_ISR_EOC))
	{
		read_idx++;
		read_idx %= ADC_OVS;
		readout[read_idx] = (uint16_t)ADC1->DR;
		//6-bit resolution (64 steps)
		TIM3->CCR1 = brightness_lut[ compute_adc(readout,ADC_OVS) ];

	}
}
/*..........................................................................*/

//obsługa VW
void TIM14_IRQHandler(void)
{
	if (TIM14->SR & TIM_SR_UIF)
	{
				TIM14->SR = ~TIM_SR_UIF;

				//odczytaj stan pinu VW
				uint16_t pin = GPIO_Get(GPIO(VW_GPIO),VW_PIN);
				//wywołaj vw_pll
				if(vw_isr(pin))
						QF_PUBLISH(Q_NEW(QEvt, VW_SIG), &l_bsp_obj);

	}
}


/* BSP functions ===========================================================*/
void BSP_init(void) {

	//Start Watchdog. Reset timeout ~2s
	iwdg_enable(IWDG_32, 2499, 0);

	BSP_resetClock();
//	button
	BSP_buttonInit();
//	LDR
	BSP_adcInit();
//	spi
	BSP_spiInit();
//	adc trigger
	BSP_adcTriggerInit();
//	oe
	BSP_pwmTimerInit();

//VW driver start
	vw_rx_start();
	BSP_vwTimerInit();



    /* initialize the QS software tracing... */
    if (QS_INIT((void *)0) == 0U) {
        Q_ERROR();
    }

//    QS_OBJ_DICTIONARY(&l_bsp_obj); /* must be called *after* QF_init() */
//    QS_USR_DICTIONARY(APP_STAT);
//    QS_USR_DICTIONARY(COMMAND_STAT);
//    QS_USR_DICTIONARY(LOG_STAT);
//    QS_USR_DICTIONARY(SYSTICK_STAT);
//    QS_USR_DICTIONARY(VW_STAT);
//    QS_USR_DICTIONARY(DUP_STAT);
//    QS_USR_DICTIONARY(VALERR_STAT);
//    QS_USR_DICTIONARY(HALT_STAT);

//    /* setup the QS filters... */
////    QS_FILTER_ON(QS_ALL_RECORDS);
////    QS_FILTER_OFF(QS_QF_TICK);
//
//    QS_FILTER_ON(QS_SM_RECORDS); // state machine records
//    QS_FILTER_ON(QS_UA_RECORDS); // all usedr records
}
/*..........................................................................*/
uint32_t BSP_getTimestamp(void)
{
	return timestamp;
}
/*..........................................................................*/

/*..........................................................................*/

/*..........................................................................*/

/*..........................................................................*/

/*..........................................................................*/

/*..........................................................................*/

#if !defined(DEBUG)
void __reset_hardware(void);
#endif

void assert_failed(uint8_t * file, uint32_t line)
{
	/* Save error to examine */
	rerr_setErr(file,line);

#if !defined(DEBUG)
  __reset_hardware();
#endif

  for(;;)
  {
	  __BKPT();
  }

}
/*..........................................................................*/


/* QF callbacks ============================================================*/
void QF_onStartup(void) {

	/* Reset watchdog */
	iwdg_reset();

	/* Check there has been software reset due to error */
  uint8_t err = errCheckNGetCode();
  if(err != ERR_NOERR)
  {
			ErrEvt *pe = Q_NEW(ErrEvt, ERR_SIG);
			pe->errCode = err;
			QF_PUBLISH((QEvt*)pe, &l_bsp_obj);
			//Reset miał być ręczny po sprawdzeniu pliku i lokacji przez konsolę
			//UART, ale ponieważ UART nie zotał wdrożony reset większości błędów
			//następuje w HSM SerDispErr. Reset nie jest wykonywany jedynie dla
			//ERR_ASSERT_FAILED (err.h) by można sobie było wleźć przez debugger i
			//odczytać moduł i lokację błędów ze zmiennej rerror (rerror.c) w sekcji
			//.noinit oraz zresetować błąd.
			//rerr_reset();	//clear it after examination
  }

    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    //NVIC_SetPriority(EXTI0_1_IRQn,   EXTI0_1_PRIO);
    /* ... */

    /* enable IRQs... */
    BSP_adcStart(ADC_PRIO);
    BSP_vwTimerStart(VW_PRIO);

}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QV_onIdle(void) {  /* called with interrupts disabled, see NOTE01 */


#ifdef Q_SPY
    QF_INT_ENABLE();
    if ((USART1->ISR & 0x0080U) != 0) {  /* is TXE empty? */
        uint16_t b;

        QF_INT_DISABLE();
        b = QS_getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  /* not End-Of-Data? */
            USART1->TDR  = (b & 0xFFU);  /* put into the DR register */
        }
    }
#elif defined NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    /* !!!CAUTION!!!
    * The QV_CPU_SLEEP() contains the WFI instruction, which stops the CPU
    * clock, which unfortunately disables the JTAG port, so the ST-Link
    * debugger can no longer connect to the board. For that reason, the call
    * to QV_CPU_SLEEP() has to be used with CAUTION.
    */
    /* NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    * reset the board, then connect with ST-Link Utilities and erase the part.
    * The trick with BOOT(0) is it gets the part to run the System Loader
    * instead of your broken code. When done disconnect BOOT0, and start over.
    */
    //QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
    QF_INT_ENABLE(); /* for now, just enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const *module, int loc) {

    QS_ASSERTION(module, loc, (uint32_t)10000U); /* report assertion to QS */

    assert_failed((uint8_t*)module, (uint32_t)loc);

}

/* QS callbacks ============================================================*/
#ifdef Q_SPY
/*..........................................................................*/

/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {
    static uint8_t qsBuf[2*1024]; /* buffer for Quantum Spy */

    (void)arg; /* avoid the "unused parameter" compiler warning */
    QS_initBuf(qsBuf, sizeof(qsBuf));

    /* enable USART1 */
    BSP_uartInit();

    QS_tickPeriod_ = SystemCoreClock / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_; /* to start the timestamp at zero */

    /* setup the QS filters... */
//    QS_FILTER_ON(QS_SM_RECORDS);
//    QS_FILTER_ON(QS_UA_RECORDS);
    //    QS_FILTER_ON(QS_ALL_RECORDS);
    //    QS_FILTER_OFF(QS_QF_TICK);

    return (uint8_t)1; /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) { /* NOTE: invoked with interrupts DISABLED */
    if ((SysTick->CTRL & 0x00010000) == 0) {  /* COUNT no set? */
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else { /* the rollover occured, but the SysTick_ISR did not run yet */
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }
}
/*..........................................................................*/
void QS_onFlush(void) {
    uint16_t b;

    QF_INT_DISABLE();
    while ((b = QS_getByte()) != QS_EOD) {    /* while not End-Of-Data... */
        QF_INT_ENABLE();
        while ((USART1->ISR & 0x0080U) == 0U) { /* while TXE not empty */
        }
        USART1->TDR  = (b & 0xFFU);  /* put into the DR register */
        QF_INT_DISABLE();
    }
    QF_INT_ENABLE();
}

#endif /* Q_SPY */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The QV_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QV_onIdle() must internally enable interrupts, ideally
* atomically with putting the CPU to the power-saving mode.
*
*/
