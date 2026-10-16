# Campo Minado Light

Trabalho de Sistemas Embarcados: um campo minado para Arduino, com matriz NeoPixel 8×8, cinco botões e buzzer. As cores indicam a quantidade de bombas vizinhas, o cursor e as bandeiras. Há três dificuldades, com 5, 10 ou 15 bombas.

## Como usar

Instale a biblioteca **Adafruit NeoPixel** na Arduino IDE e abra `Campo_Minado/Campo_Minado.ino`. Selecione a placa e a porta usadas na montagem e envie o código.

A matriz usa o pino D8, o buzzer usa D7 e os botões usam D2 (esquerda), D3 (confirmar), D4 (cima), D5 (direita) e D6 (baixo), com `INPUT_PULLUP`.

No menu, escolha a dificuldade com cima/baixo e confirme com o botão central. Durante o jogo, os direcionais movem o cursor, um toque no botão central abre a casa e segurá-lo por meio segundo coloca ou remove uma bandeira.

Para ver o esquema e a PCB, importe `Campo_Minado.epro2` no EasyEDA como projeto **EasyEDA Professional**.
