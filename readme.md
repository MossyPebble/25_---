```
:'######:::'####:'##::::'##:'##:::::::'########:'########:
'##... ##::. ##:: ###::'###: ##::::::: ##.....::... ##..::
 ##:::..:::: ##:: ####'####: ##::::::: ##:::::::::: ##::::
 ##::'####:: ##:: ## ### ##: ##::::::: ######:::::: ##::::
 ##::: ##::: ##:: ##. #: ##: ##::::::: ##...::::::: ##::::
 ##::: ##::: ##:: ##:.:: ##: ##::::::: ##:::::::::: ##::::
. ######:::'####: ##:::: ##: ########: ########:::: ##::::
:......::::....::..:::::..::........::........:::::..:::::
```

# Gimlet: 반려동물 홈캠 용 Arduino App + HTML/JS GUI Interface

## main.ino에서 생성한 서버의 API 목록

1. `/LED=ON`, `/LED=OFF`
테스트용 LED를 켜고 끌 수 있는 API.

1. `/LMotor=ON`, `/LMotor=OFF`, `/RMotor=ON`, `/RMotor=OFF`
왼쪽, 오른쪽 모터를 켜고 끌 수 있는 API.

1. `/LMotor=FORWARD`, `/LMotor=BACKWARD`, `/RMotor=FORWARD`, `/RMotor=BACKWARD`
왼쪽, 오른쪽 모터의 방향을 조절할 수 있는 API.

1. `/Speed=LOW`, `/Speed=MEDIUM`, `/Speed=HIGH`
    모터의 속도를 조절할 수 있는 API.

1. `/Servo1={숫자}`, `/Servo2={숫자}`
    서보모터의 각도를 조절할 수 있는 API.
    숫자는 0~180 사이의 값으로, 서보모터의 각도를 의미함.

1. `/Speaker=ON`, `/Speaker=OFF`
    스피커를 켜고 끌 수 있는 API.
    이 API의 호출을 받는다면 즉시 서버측의 스피커를 켜서 음성을 재생한 후 streaming 형식으로 웹 측에서 재생.

1. `/Mic=ON`, `/Mic=OFF`
    마이크를 켜고 끌 수 있는 API.
    이 API의 호출을 받는다면 즉시 서버측의 마이크를 켜서 음성을 수집한 후 streaming 형식으로 웹 측에서 재생.

1. 지금 할것
    - 마이크 - 웹 음성 stream API, stream이면 가장 좋겠지만, 그게 구현될 수 있을지는 잘 모르겠음.
    - 웹 - 스피커 음성 stream API, 이하 동문.
    - servo 1/2 작동 API

1. 추후에 추가해야 할 것
    - 온습도 센서 API
    - 현재 아마 카메라가 바로 작동하지 않을 것임. 카메라 전송을 시작하는 API를 추가해야 함.
    - 배터리 잔량 API

## index.html에서 사용하는 버튼 및 조작 모음

1. LED ON/OFF (테스트용)
    - LED ON: LED ON
    - LED OFF: LED OFF

1. 전진/후진/좌회전/우회전
    - 전진: LMotor ON, RMotor ON, LMotor FORWARD, RMotor FORWARD
    - 후진: LMotor ON, RMotor ON, LMotor BACKWARD, RMotor BACKWARD
    - 좌회전: LMotor ON, RMotor OFF, RMotor FORWARD
    - 우회전: LMotor OFF, RMotor ON, RMotor FORWARD

1. 속도 조절
    - LOW: LMotor LOW, RMotor LOW
    - MEDIUM: LMotor MEDIUM, RMotor MEDIUM
    - HIGH: LMotor HIGH, RMotor HIGH

1. 서버 측 마이크 ON/OFF
    - 서버 측 마이크 ON: Mic ON
    - 서버 측 마이크 OFF: Mic OFF 이후 html 측에서 다운로드 하여 재생

1. 서버 측 스피커 ON/OFF
    - 서버 측 스피커 ON: Speaker ON
    - 서버 측 스피커 OFF: Speaker OFF 이후 server 측에서 다운로드 하여 재생

## 주의 사항

1. 마이크 모듈은 뒤에 /audio 타고 들어가야함.