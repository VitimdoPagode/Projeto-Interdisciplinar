#include "headers.h"

// ---- Funções principais do programa ----
void loop()											//Função loop para reconhecer o acontecimento de um evento
{
	menu_inicial();
}

int main()											//Função main
{
	setup();
	while(true)
	{
		loop();
	}
	return 0;
}

// =================================================================================================================== //