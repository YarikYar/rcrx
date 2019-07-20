#include "stm32f0xx.h"                  // Device header
#include "defines.h"
#include "delay.h"
#include "ssd1306.h"
#include "i2c.h"
#include "spi.h"
#include "nrf24l01.h"

// 3 csn 4 ce
uint8_t tx_addr[5] = {0x34,0x43,0x10,0x10,0x01};
uint8_t rx_addr[5] = {0x05,0xAC,0x40,0xBC,0xa7};
uint8_t buf[10], rbuf[10],temp[10],ack_buf[7];
int16_t trim, epl, epr, dr,rev,c_lost,err;
uint16_t wdg_counter;

int main(void)
{
	system_clock();
	init_delay();
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	RCC->APB1ENR |=   RCC_APB1ENR_TIM3EN;
	
	GPIOA_MODER |= GPIO_MODER_11_OUT | GPIO_MODER_12_OUT | GPIO_MODER_2_OUT | GPIO_MODER_10_IN;
	GPIOA_MODER |= GPIO_MODER_5_AF | GPIO_MODER_6_AF | GPIO_MODER_7_AF | GPIO_MODER_3_OUT | GPIO_MODER_4_OUT | GPIO_MODER_0_IN;
	GPIOA_MODER |= GPIO_MODER_8_AF | GPIO_MODER_9_AF;
	GPIOA_AFRH |= GPIO_AFRH_8_A2 | GPIO_AFRH_9_A2;
	GPIOA_PUPDR |= GPIO_PUPDR_0_UP;
	
	GPIOA_AFRL |= GPIO_AFRL_5_A0 | GPIO_AFRL_6_A0 | GPIO_AFRL_7_A0;
	GPIOB_MODER |= GPIO_MODER_6_OUT | GPIO_MODER_0_AF | GPIO_MODER_1_AF;
	GPIOB_AFRL |= GPIO_AFRL_0_A1 | GPIO_AFRL_1_A1;
	GPIOA_OTYPER |= GPIO_OTYPER_11_OD | GPIO_OTYPER_12_OD; 
	GPIOA_OSPEEDR |= GPIO_OSPEEDR_11_FAS | GPIO_OSPEEDR_12_FAS | GPIO_OSPEEDR_0_FAS;
	GPIOA_PUPDR |= GPIO_PUPDR_10_DN | GPIO_PUPDR_0_UP | GPIO_PUPDR_12_UP | GPIO_PUPDR_11_UP;
	
	TIM1->ARR=19999;
	TIM1->PSC=47;
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1| TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2M_1| TIM_CCMR1_OC2M_2 ;
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM1->CCR1 = 1500;
	TIM1->CCR2 = 1500;
	
	TIM3->ARR=19999;
	TIM3->PSC=47;
  TIM3->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE;
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_1| TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC4M_1| TIM_CCMR2_OC4M_2 ;
	TIM3->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E;
	TIM3->BDTR |= TIM_BDTR_MOE;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM3->CCR3 = 1500;
	TIM3->CCR4 = 1500;
	
		spi_init(SPI_CR1_MODE_0, SPI_CR1_PSC_16, SPI_CR1_DIR_2FD);

	GPIOA_BSRR |= GPIO_BSRR_4_R;

		GPIOA_BSRR |= GPIO_BSRR_3_R;
		spi_send_recv(FLUSH_RX);
		GPIOA_BSRR |= GPIO_BSRR_3_S;
		nrf_writereg(STATUS,0x70);
	nrf_writebuf(TX_ADDR | W_REGISTER,tx_addr,5);
	nrf_writebuf(RX_ADDR_P0 | W_REGISTER,tx_addr,5);
//	nrf_writereg(RX_PW_P0,0x07);
	nrf_writereg(EN_RXADDR,0x01);
	nrf_writereg(EN_AA,0x01);
	nrf_writereg(RF_CH,0x28);
	nrf_writereg(RF_SETUP,0x06);
		nrf_writereg(FEATURE,(1<<1)|(1<<2));
	nrf_writereg(DYNPD,(1<<0));
	nrf_writereg(CONFIG,0x0B);
			delay_ms(12);
		GPIOA_BSRR |= GPIO_BSRR_4_S;
	
	ee_rblock(0x00,0x00,buf,10);
	ssd1306_init();
	ssd1306_flood(0x00);
	ssd1306_string(0,0,"nRF24l01 RC rec v0.1");
	delay_ms(10);
	
	i2c_start();
	i2c_wbyte(0x40);
	i2c_wbyte(0x0c);
	i2c_start();
	i2c_wbyte(0x41);
	uint8_t u = i2c_rbyte(0);
	i2c_stop();
	
	i2c_start();
	i2c_wbyte(0x40);
	i2c_wbyte(0x13);
	i2c_wbyte(0x02);
	i2c_stop();
	
	i2c_start();
	i2c_wbyte(0x40);
	i2c_wbyte(0x12);
	i2c_start();
	i2c_wbyte(0x41);
  u = i2c_rbyte(0);
	i2c_stop();
	
	
	
	ssd1306_clr();
	ssd1306_string(0,0,"Trim value:");
	ssd1306_string(0,9,"L ep value:");
	ssd1306_string(0,18,"R ep value:");
	ssd1306_string(0,27,"DR value:");
	ssd1306_string(0,36,"REV value:");
	trim = buf[0]<<8|buf[1];
	epl = buf[2]<<8|buf[3];
	epr = buf[4]<<8|buf[5];
	dr = buf[6]<<8|buf[7];
	rev = buf[8]<<8|buf[9];
	ssd1306_num(80,0,trim);
	ssd1306_num(80,9,epl);
	ssd1306_num(80,18,epr);
	ssd1306_num(80,27,dr);
	ssd1306_num(80,36,rev);

	while(1)
	{
		
		while((GPIOA_IDR & (1<<0)))
		{
			wdg_counter++;
			if(wdg_counter>60000)
			{
				  wdg_counter=0;
					GPIOA_BSRR |= GPIO_BSRR_4_R;
					delay_ms(5);
					GPIOA_BSRR |= GPIO_BSRR_4_S;
					delay_ms(2);
				c_lost++;
				if(c_lost>3)
				{
					c_lost=0;
          GPIOB->BSRR |= GPIO_BSRR_6_S;
					err=1;
						GPIOA_BSRR |= GPIO_BSRR_4_R;

		GPIOA_BSRR |= GPIO_BSRR_3_R;
		spi_send_recv(FLUSH_RX);
		GPIOA_BSRR |= GPIO_BSRR_3_S;
	
		GPIOA_BSRR |= GPIO_BSRR_4_S;
					
					TIM1->CCR1=1500;  // failsafe
					TIM1->CCR2=1500;
					TIM3->CCR4=1500;
				}
			}
		}
		ack_buf[0]=u;
		ack_buf[1]=0xAD;
		ack_buf[2]=0x12;
		ack_buf[3]=0x12;
		ack_buf[4]=0x12;
		nrf_writebuf(W_ACK_PAYLOAD,ack_buf,7);
		if(err)
		{
			err=0;
			GPIOB->BSRR |= GPIO_BSRR_6_R;
		}
		
		GPIOA_BSRR |= GPIO_BSRR_4_R;
			nrf_readbuf(R_RX_PAYLOAD,rbuf,7);
		if(rbuf[0]==0xBA)
		{
			ssd1306_string(0,63,"      ");
			ssd1306_num(0,63,(rbuf[3]<<8|rbuf[4]));
			ssd1306_string(0,54,"      ");
			ssd1306_num(0,54,(rbuf[1]<<8|rbuf[2]));
			ssd1306_string(0,45,"      ");
			ssd1306_num(0,45,(rbuf[5]<<8|rbuf[6]));
			TIM1->CCR1=(rbuf[3]<<8|rbuf[4]);
	   	TIM1->CCR2=(rbuf[1]<<8|rbuf[2]);
			TIM3->CCR4=(rbuf[5]<<8|rbuf[6]);
		}
			nrf_writereg(STATUS,112);
			GPIOA_BSRR |= GPIO_BSRR_4_S;
			delay_ms(2);
		
	
	}
}
