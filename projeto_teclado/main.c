#include "stm32f10x.h"
#include "lcd.h"

//teclas
#define SW1 12 
#define SW2 13
#define SW3 14
#define SW4 15
#define SW5 5
#define SW6 4
#define SW7 3
#define SW8 3
#define SW9 4
#define SW10 8
#define SW11 9
#define SW12 11
#define SW13 10
#define SW14 7
#define SW15 15
#define SW16 14
#define SW17 13

//perifericos
#define BUZZER 0 
#define POTENCIOMETRO 1

//notas
#define C			132
#define Csust	140
#define D			148
#define Dsust	157
#define E			166
#define F			176
#define Fsust	187
#define G			189
#define Gsust	209
#define A			222
#define Asust	235
#define B			249

//funcoes desenvolvidas
void inicializar(void);
void som(uint16_t nota);
void timbre(void);
void print(uint16_t valor);
void tecla(uint32_t GPIOA_IDR, uint32_t GPIOB_IDR, uint32_t GPIOC_IDR);
void att_ciclo(uint32_t GPIOB_IDR);
void att_pot(void);
void att_lcd(void);
void clear(void);

//variaveis de controle
uint8_t oitava = 1;
uint8_t ciclo = 25;
uint32_t pot = 0;

int main() {
	inicializar();
	
	while(1) {
		att_pot();
		tecla(~(GPIOA->IDR), ~(GPIOB->IDR), ~(GPIOC->IDR));
	}
}

void inicializar(void) {
	//desativar JTAG
	RCC -> APB2ENR |= RCC_APB2ENR_AFIOEN ;
	AFIO -> MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE ;
	
	//Ativação dos clocks
	RCC->APB2ENR |= 0xFC | (1<<9);
	RCC->APB1ENR |= (1<<1);
	
	//Configuração do GPIOA
	GPIOA->CRL = 0x43344444 ; //lcd output
	GPIOA->CRH = 0x34433443 ; //o resto input
	
	//Configuração do GPIOB
	GPIOB->CRL = 0x3344430B; //analog input PA1 e PB0 push-pull
	GPIOB->CRH = 0x44444444; //input SW1~SW7 e SW10~SW13
	
	//Configuração do GPIOC
	GPIOC->CRH = 0x44433333; //input PC15 PC14 e PC13
		
	//Configuração do ADC
	ADC1->CR2 = 1; 			// ADON
	ADC1->SMPR2 = 1<<3; //SMP1
	
	//Configuração do buzzer
	TIM3->CCMR2 = 0x0060;	 //configura pwm
	TIM3->CCER = 0x1 << 8; //CC3P=0 e CC3E=1
	TIM3->PSC = 10-1;
	
	//Configuração do LCD
	lcd_init();
	att_lcd();
}

void tecla(uint32_t GPIOA_IDR, uint32_t GPIOB_IDR, uint32_t GPIOC_IDR) {
	att_ciclo(GPIOB_IDR);																	//verifica mudanca de ciclo
	if(GPIOB_IDR & (1<<SW5))				som(oitava * C);			//verifica teclas
	else if(GPIOB_IDR & (1<<SW13))	som(oitava * Csust);
	else if(GPIOB_IDR & (1<<SW6))		som(oitava * D);
	else if(GPIOA_IDR & (1<<SW14))	som(oitava * Dsust);
	else if(GPIOB_IDR & (1<<SW7))		som(oitava * E);
	else if(GPIOA_IDR & (1<<SW8)) 	som(oitava * F);
	else if(GPIOC_IDR & (1<<SW15))	som(oitava * Fsust);
	else if(GPIOA_IDR & (1<<SW9)) 	som(oitava * G);
	else if(GPIOC_IDR & (1<<SW16))	som(oitava * Gsust);
	else if(GPIOB_IDR & (1<<SW10))	som(oitava * A);
	else if(GPIOC_IDR & (1<<SW17))	som(oitava * Asust);
	else if(GPIOB_IDR & (1<<SW11))	som(oitava * B);
	else if(GPIOB_IDR & (1<<SW12))	som(oitava * C);
	else														TIM3->CCR3 = 0;
}

void som(uint16_t nota) {
	//calculo de ARR
	TIM3->ARR = (72000000/((nota + (pot/25))*(TIM3->PSC+1)))-1;  
	TIM3->CCR3 = ((TIM3->ARR+1)*ciclo)/100;
	TIM3->CR1 = 1;	//inicia timer
}

void att_pot(void) {
	ADC1->SQR3 = 9; //canal 9 input
	ADC1->CR2 = 1;  //conversao
	while((ADC1->SR & (1<<1)) == 0); //aguarda flag
	pot = ADC1->DR;	//salva valor
}

void att_ciclo(uint32_t GPIOB_IDR) {
	if(GPIOB_IDR & (1<<SW1)) { //primeira oitava
		oitava = 1;
		att_lcd();
	}
	
	else if(GPIOB_IDR & (1<<SW2)) { //segunda oitava
		oitava = 2; 
		att_lcd();
	}
	
	if(GPIOB_IDR & (1<<SW3)) { //mudar ciclo
		ciclo += 25;
		if(ciclo > 75) ciclo = 25;
		att_lcd();
	}
}

void att_lcd(void) {
	clear();
	lcd_print("Oitava: ");
	print(oitava);
	lcd_command(0xC0); //pula linha
	lcd_print("Ciclo: ");
	print(ciclo);
}

void print(uint16_t valor) {
	lcd_data((valor/100)%10 + 0x30); //centena
	lcd_data((valor/10)%10 + 0x30);  //dezena
	lcd_data(valor%10 + 0x30);       //unidade
}

void clear(void) {
	lcd_command (0x01); //limpa
	lcd_command (0x02); //volta linha 1
}
