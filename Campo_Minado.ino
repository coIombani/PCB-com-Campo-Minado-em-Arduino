#include <Adafruit_NeoPixel.h>

int numBombas;
bool noMenu = true;
int dificuldade = 1;

Adafruit_NeoPixel m(64, 8, NEO_GRB + NEO_KHZ800);

const int up = 4;
const int down = 6;
const int leftBtn = 2;
const int rightBtn = 5;
const int ok = 3;
const int buzz = 7;

bool bombas[8][8];
bool aberto[8][8];
bool flag[8][8];
int nums[8][8];

int xCur = 3;
int yCur = 3;

bool acabou = false;
bool segurando = false;
bool fezLongo = false;

unsigned long tempo = 0;

uint32_t fechado, vazio, bombaCor, cursor, bandeira;
uint32_t n1,n2,n3,n4,n5,n6,n7,n8;

int pos(int x, int y){
  return (7 - y) * 8 + (7 - x);
}

void desenhaMenu() {
  m.clear();
  uint32_t cor = m.Color(255, 255, 255);

  if (dificuldade == 1) {
    for(int y=1; y<7; y++) m.setPixelColor(pos(3, y), cor);
    m.setPixelColor(pos(2, 2), cor);
  } else if (dificuldade == 2) {
    m.setPixelColor(pos(2,1), cor); m.setPixelColor(pos(3,1), cor); m.setPixelColor(pos(4,1), cor);
    m.setPixelColor(pos(5,2), cor); m.setPixelColor(pos(4,3), cor); m.setPixelColor(pos(3,4), cor);
    m.setPixelColor(pos(2,5), cor);
    m.setPixelColor(pos(2,6), cor); m.setPixelColor(pos(3,6), cor); m.setPixelColor(pos(4,6), cor); m.setPixelColor(pos(5,6), cor);
  } else if (dificuldade == 3) {
    m.setPixelColor(pos(2,1), cor); m.setPixelColor(pos(3,1), cor); m.setPixelColor(pos(4,1), cor);
    m.setPixelColor(pos(5,2), cor); m.setPixelColor(pos(4,3), cor); m.setPixelColor(pos(3,3), cor);
    m.setPixelColor(pos(5,4), cor); m.setPixelColor(pos(5,5), cor);
    m.setPixelColor(pos(2,6), cor); m.setPixelColor(pos(3,6), cor); m.setPixelColor(pos(4,6), cor);
  }
  m.show();
}

void iniciarJogo() {
  Serial.begin(9600);

  memset(bombas, false, sizeof(bombas));
  memset(aberto, false, sizeof(aberto));
  memset(flag, false, sizeof(flag));
  memset(nums, 0, sizeof(nums));

  if(dificuldade == 1) numBombas = 5;
  else if(dificuldade == 2) numBombas = 10;
  else if(dificuldade == 3) numBombas = 15;

  int b = 0;
  while(b < numBombas){
    int x = random(8);
    int y = random(8);

    if(!bombas[x][y]){
      bombas[x][y] = true;
      b++;
    }
  }

  for(int x=0;x<8;x++){
    for(int y=0;y<8;y++){
      if(bombas[x][y]) continue;

      for(int dx=-1;dx<=1;dx++){
        for(int dy=-1;dy<=1;dy++){
          int nx = x + dx;
          int ny = y + dy;

          if(nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && bombas[nx][ny]){
            nums[x][y]++;
          }
        }
      }
    }
  }
  
  Serial.println("MAPA:");
  for(int y=7;y>=0;y--){
    for(int x=7;x>=0;x--){
      if(bombas[x][y]){
        Serial.print("B ");
      } else {
        Serial.print(nums[x][y]);
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  Serial.println();

  xCur = 3;
  yCur = 3;
  acabou = false;
  desenha();
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  m.begin();
  m.setBrightness(7);

  fechado  = m.Color(50,50,50);
  vazio    = m.Color(0,0,0);
  n1 = m.Color(0,160,255);
  n2 = m.Color(0,70,0);
  n3 = m.Color(255,30,20);
  n4 = m.Color(0,0,255);
  n5 = m.Color(255,165,0);
  n6 = m.Color(250,100,100);
  n7 = m.Color(50,255,50);
  n8 = m.Color(35,35,35);
  cursor   = m.Color(255,255,30);
  bombaCor = m.Color(255,0,0);
  bandeira = m.Color(128, 0, 128);

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);

  desenhaMenu();
}

void loop() {
  if(noMenu) {
    if(digitalRead(up) == LOW) {
      dificuldade++;
      if(dificuldade > 3) dificuldade = 3;
      desenhaMenu();
      delay(200);
    }
    else if(digitalRead(down) == LOW) {
      dificuldade--;
      if(dificuldade < 1) dificuldade = 1;
      desenhaMenu();
      delay(200);
    }

    if(digitalRead(ok) == LOW) {
      noMenu = false;
      iniciarJogo();
      delay(300);
    }
    return;
  }

  if(acabou) {
    delay(3000); 
    noMenu = true;
    acabou = false;
    desenhaMenu();
    return;
  }

  int mx = 0, my = 0;
  if(digitalRead(up) == LOW) my = -1;
  else if(digitalRead(down) == LOW) my = 1;
  if(digitalRead(leftBtn) == LOW) mx = -1;
  else if(digitalRead(rightBtn) == LOW) mx = 1;

  if(mx != 0 || my != 0){
    xCur = constrain(xCur + mx, 0, 7);
    yCur = constrain(yCur + my, 0, 7);
    desenha();
    delay(180);
  }

  int estado = digitalRead(ok);

  if(estado == LOW && !segurando){
    segurando = true;
    fezLongo = false;
    tempo = millis();
  }
  else if(estado == LOW && segurando && !fezLongo){
    if(millis() - tempo >= 500){
      if(!aberto[xCur][yCur]){
        flag[xCur][yCur] = !flag[xCur][yCur];
        tone(buzz, 1500, 30);
        desenha();
      }
      fezLongo = true;
    }
  }
  else if(estado == HIGH && segurando){
    segurando = false;
    if(!fezLongo){
      abre(xCur, yCur);
      delay(200);
    }
  }
}

void desenha(){
  for(int x=0;x<8;x++){
    for(int y=0;y<8;y++){
      uint32_t cor;
      if(x == xCur && y == yCur) cor = cursor;
      else if(flag[x][y]) cor = bandeira;
      else if(!aberto[x][y]) cor = fechado;
      else if(bombas[x][y]) cor = bombaCor;
      else {
        switch(nums[x][y]){
          case 0: cor = vazio; break;
          case 1: cor = n1; break;
          case 2: cor = n2; break;
          case 3: cor = n3; break;
          case 4: cor = n4; break;
          case 5: cor = n5; break;
          case 6: cor = n6; break;
          case 7: cor = n7; break;
          default: cor = n8; break;
        }
      }
      m.setPixelColor(pos(x,y), cor);
    }
  }
  m.show();
}

void abre(int x, int y){
  if(aberto[x][y] || flag[x][y]) return;

  if(bombas[x][y]){
    aberto[x][y] = true;
    for(int i=0;i<8;i++){
      for(int j=0;j<8;j++){
        if(bombas[i][j]) m.setPixelColor(pos(i,j), bombaCor);
      }
    }
    m.show();

tone(buzz, 150, 200);
    delay(250);
    tone(buzz, 100, 400);
    delay(400);
    noTone(buzz);
    acabou = true;
    return;
  }

  int fx[64], fy[64];
  int ini = 0, fim = 0;

  fx[fim] = x;
  fy[fim] = y;
  fim++;
  aberto[x][y] = true;

  while(ini < fim){
    int ax = fx[ini];
    int ay = fy[ini];
    ini++;

    if(nums[ax][ay] == 0){
      for(int dx=-1;dx<=1;dx++){
        for(int dy=-1;dy<=1;dy++){
          if(dx == 0 && dy == 0) continue;
          int nx = ax + dx;
          int ny = ay + dy;
          if(nx >= 0 && nx < 8 && ny >= 0 && ny < 8){
            if(!aberto[nx][ny] && !bombas[nx][ny] && !flag[nx][ny]){
              aberto[nx][ny] = true;
              fx[fim] = nx;
              fy[fim] = ny;
              fim++;
            }
          }
        }
      }
    }
  }
  desenha();

  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      if(!bombas[i][j] && !aberto[i][j]) return;
    }
  }

  int musica[] = {523,659,784,1047};
  for(int i=0;i<4;i++){
    tone(buzz, musica[i], 200);
    delay(220);
  }
  noTone(buzz);
  acabou = true;
}