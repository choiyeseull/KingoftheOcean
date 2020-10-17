#include <bangtal>
#include <stdio.h>
#include <ctime>
#define FISH_NUM 20
#define CRUSH 10

using namespace bangtal;

ObjectID startButton, endButton, endButton1, restartButton, restartButton1, player, fish[3][FISH_NUM];
SceneID start_scene, scene, gameover,gameclear;
TimerID fish0Timer, fish1Timer, fish2Timer, checkTimer;
SoundID bgm, levelup, crushsound;

int playerX = 30, playerY = 300, meX = 0, meY = 0;
int fishX[3][FISH_NUM] = { 0, }, fishY[3][FISH_NUM] = { 0, };
int score[6][FISH_NUM] = { 0, }, finishScore = 0, bestScore = 0;

int playerlevel = 0;
int fishlevel[3] = { 0,1,2 };
int fast = 1;

int pass = 0, crush = 0, pass1 = 0, crush1 = 0, pass2 = 0, crush2 = 0;

bool isVisuable[3][FISH_NUM];
bool is_finish = false;

void soundCallback(SoundID sound) {
	if (sound == bgm) {
		playSound(bgm);
	}
}

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown)
{
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);
	if (shown) {
		showObject(object);
	}
	return object;
}

void createFish(int n) {	
	int x, y;
	int k = n % 3;
	char name[50];
	sprintf_s(name, "image/%d.png", n);

	srand((unsigned int)time(NULL));

	for (int i = 0; i < FISH_NUM; i++) {
		x = rand() % (5000 + (1000 * n));
		y = rand() % 600;

		fishX[k][i] = 1280 + x;
		fishY[k][i] = 10 + y;

		fish[k][i] = createObject(name, scene, fishX[k][i], fishY[k][i], true);
		isVisuable[k][i] = true;
	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {

	if (object == startButton) {
		hideObject(startButton);
		enterScene(scene);

		playSound(bgm);
		startTimer(fish0Timer);	
		startTimer(fish1Timer);
		startTimer(fish2Timer);
		startTimer(checkTimer);
	}
	if (object == endButton || object == endButton1) {
		endGame();
	}
	if (object == restartButton || object == restartButton1) {
		playerX = 30, playerY = 300;
		meX = 0, meY = 0;
		finishScore = 0;
		playerlevel = 0;
		pass = 0, crush = 0, pass1 = 0, crush1 = 0, pass2 = 0, crush2 = 0;
		is_finish = false;
		scaleObject(player, (float)0.2);
		setSceneLight(scene, 1.0f);
		playSound(bgm);

		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < FISH_NUM; j++) {
				score[i][j] = 0;
			}
		}
		for (int i = 0; i < 3; i++) {
			fishlevel[i] = i;
			for (int j = 0; j < FISH_NUM; j++) {
				hideObject(fish[i][j]);
				fish[i][j] = 0;
				isVisuable[i][j] = false;
				fishX[i][j] = 0; fishY[i][j] = 0;
			}
		}
		createFish(0); createFish(1); createFish(2);

		setTimer(fish0Timer, 0.01f); startTimer(fish0Timer);
		setTimer(fish1Timer, 5.0f); startTimer(fish1Timer);
		setTimer(fish2Timer, 20.0f); startTimer(fish2Timer);
		setTimer(checkTimer, 0.01f); startTimer(checkTimer);

		enterScene(scene);
	}
}

void check_move() {		//player가 화면 밖에서 움직이지 않도록 체크하는 함수
	if (playerX + meX < 0 || playerY + meY < 0-(playerlevel*20) || playerX + meX > 1190-(playerlevel*40) || playerY + meY > 660-(playerlevel*20)) {}
	else {
		playerX += meX; playerY += meY;
		locateObject(player, scene, playerX, playerY);
	}
}

void keyboardCallback(KeyCode code, KeyState state) {
	if (code == 83) { //right
        meX += (state == KeyState::KEYBOARD_PRESSED ? 20 : -20);
		check_move();
	}
    else if (code == 82) {  //left
        meX -= (state == KeyState::KEYBOARD_PRESSED ? 20 : -20);
		check_move();
    }
    else if (code == 84) {   //up
        meY += (state == KeyState::KEYBOARD_PRESSED ? 20 : -20);
		check_move();
    }
    else if (code == 85) {  //down
        meY -= (state == KeyState::KEYBOARD_PRESSED ? 20 : -20);
		check_move();
    }
}

void calculScore() {		//점수 계산하는 함수
	char buf[256];
	char buf1[256];
	stopSound(bgm);

	stopTimer(checkTimer);
	stopTimer(fish0Timer);
	stopTimer(fish1Timer);
	stopTimer(fish2Timer);

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < FISH_NUM; j++) {
			finishScore += score[i][j];
		}
	}
	if (finishScore >= bestScore) {
		bestScore = finishScore;
	}
	sprintf_s(buf, "★ Best score ★ : %d \n score : %d ", bestScore, finishScore);
	showMessage(buf);
}



void timerCallback(TimerID timer) {
	if (is_finish == false) {
		if (timer == fish0Timer) {		
			float fast = 1 - (fishlevel[0] * 0.1);
			
			for (int i = 0; i < FISH_NUM; i++) {
				fishX[0][i] -= 100;
				locateObject(fish[0][i], scene, fishX[0][i], fishY[0][i]);

			if (fishX[0][i] < 0 && fishX[0][i] >= -100) {		
				hideObject(fish[0][i]);

				if (isVisuable[0][i] == true) {					
					pass++;
				}
				else if (isVisuable[0][i] == false) {
					crush++;
					if (crush == CRUSH) {
						if (playerlevel == 0 || playerlevel == 3) {
							showMessage("레벨업!");
							playSound(levelup);
							playerlevel++;
							scaleObject(player, (float)(playerlevel + 2) / 10);
						}
					}
				}
				if (pass + crush == FISH_NUM) {			
					pass = 0; crush = 0;
					if (playerlevel == 0) { createFish(0); }
					else {
						createFish(3); 
						fishlevel[0] = 3;	
					}
				}
			}
		}
		setTimer(fish0Timer, fast);
		startTimer(fish0Timer);
	}

	if (timer == fish1Timer) {	
		float fast = 1 - (fishlevel[1] * 0.1);
		for (int i = 0; i < FISH_NUM; i++) {
			fishX[1][i] -= 100;
			locateObject(fish[1][i], scene, fishX[1][i], fishY[1][i]);

			if (fishX[1][i] < 0 && fishX[1][i] >= -100) {
				hideObject(fish[1][i]);
				
				if (isVisuable[1][i] == true) {
					pass1++;
				}
				else if (isVisuable[1][i] == false) {
					crush1++;
					if (crush1 == CRUSH) {
						if (playerlevel == 1 || playerlevel == 4) {
							showMessage("레벨업!");
							playSound(levelup);
							playerlevel++;
							scaleObject(player, (float)(playerlevel + 2) / 10);
						}
					}
				}
				if (pass1 + crush1 == FISH_NUM) {
					pass1 = 0; crush1 = 0;
					if (playerlevel <= 1){ 	createFish(1); }
					else { 	
						createFish(4); 
						fishlevel[1] = 4;
						setSceneLight(scene, 0.6f);
					}
				}
			}
		}
		setTimer(fish1Timer, fast);
		startTimer(fish1Timer);
	}

	if (timer == fish2Timer) {		
		float fast = 1 - (fishlevel[2] * 0.1);
		for (int i = 0; i < FISH_NUM; i++) {
			fishX[2][i] -= 100;
			locateObject(fish[2][i], scene, fishX[2][i], fishY[2][i]);

			if (fishX[2][i] < 0 && fishX[2][i] >= -100) {
				hideObject(fish[2][i]);
				if (isVisuable[2][i] == true) {
					pass2++;
				}
				else if (isVisuable[2][i] == false) {
					crush2++;	
					if (crush2 == CRUSH) {	//GAMEOVER
						if (playerlevel == 5) {
							is_finish = true;
							enterScene(gameclear);
						}
						else {	//레벨 2->3
							if (playerlevel == 2) {
								showMessage("레벨업!");
								playSound(levelup);
								playerlevel++;
								scaleObject(player, (float)(playerlevel + 2) / 10);
							}
						}	
					}
				}
				if (pass2 + crush2 == FISH_NUM) {
					pass2 = 0; crush2 = 0;
					if (playerlevel <= 2){ 	
						createFish(2); 
					}
					else { 
						createFish(5); 
						fishlevel[2] = 5; 
					}
				}
			}
		}
		setTimer(fish2Timer, fast);
		startTimer(fish2Timer);
	}
	
	if (timer == checkTimer) {	

		for (int i = 0; i <3; i++) {
			for (int j = 0; j < FISH_NUM; j++) {
				int playerheight = 18 + 7 * (playerlevel + 1);
				int playerwidth = 40 + 35 * (playerlevel + 1);

				int fishheight = (fishlevel[i]+1) * 10;
				int fishwidth = (fishlevel[i]+1) * 20;

				if (fishX[i][j] > playerX + playerwidth || fishY[i][j] < playerY - playerheight
					|| fishX[i][j] + fishwidth < playerX || fishY[i][j] - fishheight > playerY) {}	//안부딪힌 경우
				else{	//부딪힌 경우
					if (fishlevel[i] <= playerlevel) {	
						fishX[i][j] = 0;
						fishY[i][j] = 0;
							hideObject(fish[i][j]);
							isVisuable[i][j] = false;
							score[fishlevel[i]][j] = 2*(i+1);
							playSound(crushsound);
					}
					else {
						is_finish = true;	
						enterScene(gameover);
					}
				}
			}
		}
		setTimer(checkTimer, 0.01f);
		startTimer(checkTimer);
		}
	}
	else {
		calculScore();
	}
}

int main() {	

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	
	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);
	setKeyboardCallback(keyboardCallback);
	setSoundCallback(soundCallback);

	start_scene = createScene("KingoftheOcean", "image/startscene.png");
	scene = createScene("underwater", "image/background.jpg");
	gameover = createScene("GameOver", "image/gameover.png");
	gameclear = createScene("GameClear", "image/gameclear.png");

	startButton = createObject("image/start.png", start_scene, 600, 300, true);
	endButton = createObject("image/end.png", gameover, 650, 240, true);
	endButton1 = createObject("image/end.png", gameclear, 650, 240, true);
	restartButton = createObject("image/restart.png", gameover, 550, 240, true);
	restartButton1 = createObject("image/restart.png", gameclear, 550, 240, true);

	player = createObject("image/player.png", scene, playerX, playerY, true);

	bgm = createSound("bgm/UndertheSeaMR.mp3");
	levelup = createSound("bgm/levelup.mp3");
	crushsound = createSound("bgm/crush.wav");
	
	scaleObject(player, (float)0.2);

	createFish(0);
	createFish(1);	
	createFish(2);	

	fish0Timer = createTimer(0.01f); 
	fish1Timer = createTimer(5.0f);
	fish2Timer = createTimer(20.0f);
	checkTimer = createTimer(0.01f);

	startGame(start_scene);

}
