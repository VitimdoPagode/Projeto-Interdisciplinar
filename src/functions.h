#include <Arduino.h>
#include "U8glib.h"
#include "bitmaps.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// ---- Declaração de cada pino ----
#define btn_cima 2						//Botão de seleção para subir
#define btn_baixo 4						//Botão de seleção para descer
#define btn_volta 3						//Botão para voltar
#define btn_confirma 7					//Botão para confirmar
#define btn_inicia_contagem 5			//Botão para iniciar o motor/separação/contagem

#define led_motor 6						//Led para indicar a ativação do motor

#define btn_005 8						//Notão para simular o sensor das moedas de 5c
#define btn_010 9						//Notão para simular o sensor das moedas de 10c
#define btn_025 10						//Notão para simular o sensor das moedas de 25c
#define btn_050 11						//Notão para simular o sensor das moedas de 50c
#define btn_1 12						//Notão para simular o sensor das moedas de 1 real

// ---- Declaraão das variáveis globais ----

int indice = 0;							//Variável de controla para alterar a posição e a seta selecionada
int confirma = 0;						//Variável de controle para identificar qual opção foi selecionada
bool variavel = true;					//Variável de controle para criar um laço dentro de cada "submenu"

// Array dos bitmaps das telas de moedas (Total bytes used to store images in PROGMEM = 6240)
const unsigned char* bitmap_tela_moedas[6] = {
	epd_bitmap_total_de_moedas,
	epd_bitmap_moeda_005,
	epd_bitmap_moeda_010,
	epd_bitmap_moeda_025,
	epd_bitmap_moeda_050,
	epd_bitmap_moeda_1	
};

// Array dos bitmaps das telas principais (Total bytes used to store images in PROGMEM = 2080)
const unsigned char* bitmap_pagina_inicial[3] = {
	epd_bitmap_primeira_p_gina,
	epd_bitmap_segunda_p_gina,
	epd_bitmap_Terceira_p_gina
};


// Array dos bitmaps das setas (Total bytes used to store images in PROGMEM = 288)
const unsigned char* bitmap_seta[3] = {
	epd_bitmap_seta_maior,
	epd_bitmap_seta_menor,
	epd_bitmap_seta_bem_maior
};

// Array dos bitmaps do momento de contagem
const unsigned char* epd_bitmap_telas_contagem[2] = {
	epd_bitmap_contagem_finalizada,
	epd_bitmap_contando
	
};

int pagina_selecionada = 0;				//Variável de controle para alterar a tela atual
int controle = 0;						//Variável para controlar pulling (não utilizada)

int qtd_005 = 0;						//Variável para contar a quantidade de moedas de 5c
int qtd_010 = 0;						//Variável para contar a quantidade de moedas de 10c
int qtd_025 = 0;						//Variável para contar a quantidade de moedas de 25c
int qtd_050 = 0;						//Variável para contar a quantidade de moedas de 50c
int qtd_1 = 0;							//Variável para contar a quantidade de moedas de 1 real

float valor_005 = 0;					//Variável para contar o valor total de moedas de 5c
float valor_010 = 0;					//Variável para contar o valor total de moedas de 10c
float valor_025 = 0;					//Variável para contar o valor total de moedas de 25c
float valor_050 = 0;					//Variável para contar o valor total de moedas de 50c
float valor_1 = 0;						//Variável para contar o valor total de moedas de 1 real

int cont = 0;
bool controle_005 = true;
bool controle_010 = false;
bool controle_025 = true;
bool controle_050 = true;
bool controle_1 = true;
int espera = 0;

void pagina_atual(int indice)			//Função que controla a posição da seta e as telas do menu principal
{
	int local = 47;
	if(indice == 1 || indice == 4 || indice == 6)
	{
		local = 26;
	}else if (indice == 5)
	{
		local = 44;
	}else if (indice == 3)
	{
		local = 10;
	};

	
	if(indice == 6)
	{
		u8g.firstPage();
		do
		{
			u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_pagina_inicial[2]);
			u8g.drawBitmapP(16, local, 96/8, 8, bitmap_seta[0]);
			
		}while(u8g.nextPage());

	}
	else if(indice > 2)
	{
		u8g.firstPage();
		do
		{
			u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_pagina_inicial[1]);
			u8g.drawBitmapP(16, local, 96/8, 8, bitmap_seta[0]);
			
		}while(u8g.nextPage());

	}
	else
	{
		if(indice == 0)
		{
			u8g.firstPage();
			do
			{
				u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_pagina_inicial[0]);
				u8g.drawBitmapP(15, 16, 104/8, 8, bitmap_seta[2]);
			}while(u8g.nextPage());

		}else if(indice == 1)
		{
			u8g.firstPage();
			do
			{
				u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_pagina_inicial[0]);
				u8g.drawBitmapP(28, 31, 72/8, 8, bitmap_seta[1]);

			}while(u8g.nextPage());
		}else
		{
			u8g.firstPage();
			do
			{
				u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_pagina_inicial[0]);
				u8g.drawBitmapP(16, local, 96/8, 8, bitmap_seta[0]);

			}while(u8g.nextPage());			
		}
		
	};
}

void comecar_contagem()					//Função para contar as moedas que passam em casa sensor (desativada ao desligar
{										//														  o motor)
	digitalWrite(led_motor, HIGH);
	int tela = 1;

	while(variavel == false)
	{
		u8g.firstPage();
		do
		{
			u8g.drawBitmapP(0, 0, 128/8, 64, epd_bitmap_telas_contagem[tela]);
	
			u8g.setPrintPos(00, 12);
			u8g.print(qtd_005);
	
			u8g.setPrintPos(00, 24);
			u8g.print(qtd_010);
	
			u8g.setPrintPos(00, 36);
			u8g.print(qtd_025);
	
			u8g.setPrintPos(00, 48);
			u8g.print(qtd_050);
	
		}while(u8g.nextPage());
	
		
		while(!controle_010)
		{
			if(digitalRead(btn_volta) == LOW)
			{
				digitalWrite(led_motor, LOW);
				tela = 0;
			}
			if(digitalRead(btn_confirma) == LOW)
			{
				variavel = true;
			}
		  if(digitalRead(btn_010) == LOW && cont == 0)
		  {
			Serial.print("a");
			controle_010 = true;
			controle_005 = false;
			cont = 1;
		  }
		}

		while(!controle_005)
		{
			if(digitalRead(btn_volta) == LOW)
			{
				digitalWrite(led_motor, LOW);
				tela = 0;
			}
			if(digitalRead(btn_confirma) == LOW)
			{
				variavel = true;
			}
		  	if(digitalRead(btn_005) == LOW && cont == 1)
		  	{
				Serial.print("b");
				controle_005 = true;
				controle_050 = false;
				cont = 2;
		  	}
			if(digitalRead(btn_010) == LOW && cont == 1 && espera > 10000)
			{
				controle_005 = true;
			}
			espera +=1;
		}
	  
		while(!controle_050)
		{
			if(digitalRead(btn_volta) == LOW)
			{
				digitalWrite(led_motor, LOW);
				tela = 0;
			}
			if(digitalRead(btn_confirma) == LOW)
			{
				variavel = true;
			}
		  if(digitalRead(btn_050) == LOW && cont == 2)
		  {
			Serial.print("c");
			controle_050 = true;
			controle_025 = false;
			cont = 3;
		  }
		  if(digitalRead(btn_010) == LOW && cont == 2)
		  {
			controle_050 = true;
		  }
		}
		
		while(!controle_025)
		{
			if(digitalRead(btn_volta) == LOW)
			{
				digitalWrite(led_motor, LOW);
				tela = 0;
			}
			if(digitalRead(btn_confirma) == LOW)
			{
				variavel = true;
			}
		  if(digitalRead(btn_025) == LOW && cont == 3)
		  {
			Serial.print("d");
			controle_025 = true;
			controle_1 = false;
			cont = 4;
		  }
		  if(digitalRead(btn_010) == LOW && cont == 3)
		  {
			controle_025 = true;
		  }
		}


		while(!controle_1)
		{
			if(digitalRead(btn_volta) == LOW)
			{
				digitalWrite(led_motor, LOW);
				tela = 0;
			}
			if(digitalRead(btn_confirma) == LOW)
			{
				variavel = true;
			}
		  if(digitalRead(btn_1) == LOW && cont == 4)
		  {
			Serial.print("e");
			controle_1 = true;
			cont = 5;
		  }
		  if(digitalRead(btn_010) == LOW && cont == 4)
		  {
			controle_1 = true;
		  }
		}
	
		Serial.print(cont);
		espera = 0;
		controle_010 = false;
		switch(cont)
		{
			case 1:
			qtd_010 +=1;
			break;

			case 2:
			qtd_005 +=1;
			break;
			
			case 3:
			qtd_050 +=1;
			break;

			case 4:
			qtd_025 +=1;
			break;

			case 5:
			qtd_1 +=1;
			break;

		}
		cont = 0;
	}

}

void menu_atual(int confirma)			//Função para mostrar cada uma das telas que podem ser selecionadas no menu
{

	while(variavel == false)
	{

		if(confirma == 0)
		{
			comecar_contagem();
		}

		u8g.firstPage();
		do
		{
			u8g.drawBitmapP(0, 0, 128/8, 64, bitmap_tela_moedas[confirma-1]);
			if(confirma == 1)
			{

				u8g.setPrintPos(78, 34);
				u8g.print(qtd_005*0.05 + qtd_010*0.10 + qtd_025*0.25 + qtd_050*0.50 + qtd_1*1);

				u8g.setPrintPos(90, 46);
				u8g.print(qtd_005 + qtd_010 + qtd_025 + qtd_050 + qtd_1);
			}
			if(confirma == 2)
			{

				u8g.setPrintPos(100, 28);
				u8g.print(qtd_005*0.05);

				u8g.setPrintPos(77, 43);
				u8g.print(qtd_005);
			}
			if(confirma == 3)
			{

				u8g.setPrintPos(100, 28);
				u8g.print(qtd_010*0.1);

				u8g.setPrintPos(77, 43);
				u8g.print(qtd_010);
			}
			if(confirma == 4)
			{

				u8g.setPrintPos(100, 28);
				u8g.print(qtd_025*0.25);

				u8g.setPrintPos(77, 43);
				u8g.print(qtd_025);
			}
			if(confirma == 5)
			{

				u8g.setPrintPos(100, 28);
				u8g.print(qtd_050*0.5);

				u8g.setPrintPos(77, 43);
				u8g.print(qtd_050);
			}
			if(confirma == 6)
			{

				u8g.setPrintPos(100, 28);
				u8g.print(qtd_1*1.0);

				u8g.setPrintPos(77, 43);
				u8g.print(qtd_1);
			}
			
		}while(u8g.nextPage());

		if (digitalRead(btn_volta) == LOW)
		{
			variavel = true;	
		}
	}
	
}
void setup()										//Função que inicializa e configura os pinos
{
	u8g.setFont(u8g_font_tpssb);
	u8g.setColorIndex(1);

	pinMode(btn_cima, INPUT_PULLUP);
	pinMode(btn_baixo, INPUT_PULLUP);
	pinMode(btn_volta, INPUT_PULLUP);
	pinMode(btn_confirma, INPUT_PULLUP);

	pinMode(led_motor, OUTPUT);

	pinMode(btn_005, INPUT_PULLUP);
	pinMode(btn_010, INPUT_PULLUP);
	pinMode(btn_025, INPUT_PULLUP);
	pinMode(btn_050, INPUT_PULLUP);
	pinMode(btn_1, INPUT_PULLUP);
	pinMode(btn_inicia_contagem, INPUT_PULLUP);

}

void menu_inicial()
{
		if(digitalRead(btn_baixo) == LOW)
	{
		if(indice < 6) 
		{
			indice +=1;
			confirma +=1;
		}
		Serial.print(indice);
	}
	if(digitalRead(btn_cima) == LOW)
	{
		if(indice > 0) 
		{
			indice -=1;
			confirma -=1;
		}
		Serial.print(indice);
	}
	if (digitalRead(btn_confirma) == LOW)
	{
		variavel = false;
		menu_atual(confirma);
	}
	if (variavel == true)
	{
		pagina_atual(indice);
	}
}