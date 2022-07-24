#define _CRT_SECURE_NO_WARNINGS

#include <bangtal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SceneID gamemain;

ObjectID cardStack, movingC;

ObjectID initC[4]; //맨 처음의 카드
ObjectID playerC[7]; //플레이어 카드 덱 목록
ObjectID DealerC[7]; //딜러 카드 덱 목록

TimerID moveT, moveYT;

int movingC_y = 465; //움직이는 카드 y좌표
int CardUsed[52] = { -1, }; //사용된 카드숫자 저장하는 배열
int CardCount = 0; //뽑은 카드의 숫자

//점수_카드합산
int DealerScore;
int PlayerScore;

char image[40]; //카드 문자열 저장용
char init1[40], init2[40], init3[40], init4[40];
char* imageinit[4] = { &init1[40], &init2[40], &init3[40], &init4[40] };//맨 초기 카드 문자열 저장

bool canclick = true;
bool playerCardIsExist[7] = { true, true, true, true, true, true, true }; //카드가 뽑혔는가? 에 대한 변수
bool dealerCardIsExist[7] = { true, true, true, true, true, true, true }; //카드가 뽑혔는가? 에 대한 변수
bool initCard[4] = { true, true, true, true }; //초기 카드 판별을 위한 변수

/* 블랙잭 룰 - 시작하면 카드를 2장 뽑는다.랜덤으로 하나씩 뽑힌다.
*
그리고 딜러에게 2장의 카드가 가고, 한 장은 보이지 않는다.(1장만 공개) -> 여기까지 완료....

* 카드의 합이 21에 더 가까우면 이긴다.
* 카드를 보고, 21에 못 미치면 히트를 외치면 1장 더 받을 수 있다. 스탠드를 외치면 그만둔다.
* 딜러의 카드의 합이 17 이상이면 딜러는 카드를 덜 받아야 한다.
* 전체 52가지의 카드가 있는데......
* 
* 이제 카드뽑는거.... 히트, 스탠드 버튼 만들고... 딜러는 점수 16 미만일시 한장 더 뽑게시킴.
* 이제 딜러보다 점수가 작거나, 점수가 21을 넘겼거나 하면 패배.
* 딜러가 버스트하거나, 점수가 더 작거나 하면 승리.
* 블랙잭의 경우 1.5배를 받는다. (스코어 정확히 21일 경우)
* 그렇게 해서... 초기자금으로 주어진 토큰을 일정 이상으로 불리면 게임에서 승리한다.
*/

//오브젝트 배치용 함수 선언(show여부는 제외함)
ObjectID CreateObject1(const char* Obimage, int x, int y) {
    ObjectID object = createObject(Obimage);
    locateObject(object, gamemain, x, y);
    return object;
}

//카드 더미에서 뽑히는 애니메이션 재생 함수
void cardMoving() {
    setObjectImage(movingC, "images/back.png");
    setTimer(moveYT, 0.01f);
    setTimer(moveT, 0.3f);
    startTimer(moveT);
    startTimer(moveYT);
}

void matchC() { //랜덤으로 생성된 숫자에 카드 맞추고, 생성해주는 함수

    
    int N = CardUsed[CardCount-1];

    if (N < 13) { //0~12라면
        //스페이드
        sprintf(image, "images/S%d.png", N+1);
    }
    else if (N < 26) { //13~25라면
        //클로버
        sprintf(image, "images/C%d.png", (N % 13)+1);
    }
    else if (N < 39) { //26~38이라면
        //다이아
        sprintf(image, "images/D%d.png", (N % 13)+1);
    }
    else if (N < 52) { //39~51이라면
        //하트
        sprintf(image, "images/S%d.png", (N % 13)+1);
    }

    //위에서 생성한 문자열에 따라, 카드를 생성한다.
    if (CardCount <= 4) { //init일때, 뽑힌 카드가 4장이면....
        sprintf(imageinit[CardCount - 1], image); //문자열에 image를 저장한다.
        setObjectImage(initC[CardCount-1], "images/back.png"); //뒷면표시
        if (CardCount < 4) cardMoving(); //4보다 작으면 카드 재생성, 4장 뽑으면 그만둔다...
    }
    else { //4장부터는....

    }

}

//카드 랜덤생성기
void cardRandom() {

    srand((unsigned int)time(NULL)); //시드값 생성
    int N = rand() % 52; //0~51까지 숫자 중 랜덤생성

    for (int j = 0; j <= CardCount; j++) {

        if (N == CardUsed[j]) { //이미 사용된 카드의 번호이면
            j = 0; //초기화
            N = rand() % 52; //0~51까지 숫자 중 랜덤생성
        }
        else {
            if (j == CardCount) { //끝까지 다 비교했는데도 같지 않으면
                CardCount++; //카드 장수 늘리기
                CardUsed[j] = N; //사용된 번호로 추가
                matchC(); //카드를 생성
                break;
            }
        }
    }
}

//타이머콜백
void TimercallBack(TimerID timer) {

    if (timer == moveYT) { //이건 좌표 조금씩옮기는 타이머
        movingC_y -= 10;
        locateObject(movingC, gamemain, 847, movingC_y); //재위치

        setTimer(moveYT, 0.01f);
        startTimer(moveYT);
    }

    if (timer == moveT) { //이건 전체시간타이머
        stopTimer(moveYT);
        setObjectImage(movingC, "images/blank.png");
        movingC_y = 465;
        locateObject(movingC, gamemain, 847, movingC_y);

        cardRandom();
    }

}

void mouseCallack1(ObjectID obj, int x, int y, MouseAction act) {

    if (obj == cardStack) {
        cardMoving();
    }

    for (int i = 0; i < 4; i++) { //초기 덱 선택

        if (obj == initC[i]) {

            int S = (CardUsed[i] % 13) + 1;
            hideObject(initC[i]); //카드는 숨기고

            //플레이어의 덱으로 카드를 옮긴다.
            if (playerCardIsExist[0]) { //0번도 안 채워진상태면
                setObjectImage(playerC[0], imageinit[i]);
                playerCardIsExist[0] = false;
                initCard[i] = false;
                if (S <= 10) PlayerScore += S;
                else PlayerScore += 10; //잭, 킹, 퀸의 경우...

                break;
            }
            else { //1번 채우기
                setObjectImage(playerC[1], imageinit[i]);
                playerCardIsExist[1] = false;
                initCard[i] = false; 
                if (S <= 10) PlayerScore += S;
                else PlayerScore += 10; //잭, 킹, 퀸의 경우...

                for (int k = 0; k < 4; k++) { //1번까지 고르고 나면 딜러가 남은 카드를 가져감

                    int O = (CardUsed[k] % 13) + 1;

                    if (initCard[k]) { //참이면 해당 카드는 딜러에게 가게 됨.
                        hideObject(initC[k]); //남은카드 숨기기
                        
                        if (dealerCardIsExist[0]) {
                            setObjectImage(DealerC[0], "images/back.png"); //카드 하나는 비공개
                            dealerCardIsExist[0] = false;
                            if (O <= 10) DealerScore += O;
                            else DealerScore += 10; //잭, 킹, 퀸의 경우...
                        }
                        else {
                            setObjectImage(DealerC[1], imageinit[k]); //카드 하나는 공개
                            dealerCardIsExist[1] = false; 
                            if (O <= 10) DealerScore += O;
                            else DealerScore += 10; //잭, 킹, 퀸의 경우...
                            break;
                        }
                    }
                }

                printf("%d, %d", PlayerScore, DealerScore);
                break;
            }
        }
    }
}

int main()
{
    //씬 생성
    gamemain = createScene("BlackJack", "images/main.png");

    //오브젝트 생성
    cardStack = CreateObject1("images/card_stack.png", 837, 465); //카드더미
    movingC = CreateObject1("images/blank.png", 847, movingC_y); //뽑힐 때 모션으로 쓸 카드

    for (int i = 0; i < 7; i++) { //반복생성
        playerC[i] = CreateObject1("images/blank.png", 223 + (90 * i), 70);
        DealerC[i] = CreateObject1("images/blank.png", 160 + (90 * i), 470);
        showObject(playerC[i]); showObject(DealerC[i]);

        if (i < 4) {
            initC[i] = CreateObject1("images/blank.png", 221 + (160 * i), 275);
            showObject(initC[i]);
        }
    }

    for (int i = 0; i < 52; i++) CardUsed[i] = -1; //모든번호를 -1로 초기화


    //기본 오브젝트 보이기
    showObject(cardStack);
    showObject(movingC);

    //타이머 생성
    moveT = createTimer(0.3f);
    moveYT = createTimer(0.01f);

    //기본 레이아웃 감추기
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

    //콜백함수 등록
    setTimerCallback(TimercallBack);
    setMouseCallback(mouseCallack1);


    startGame(gamemain);

}
