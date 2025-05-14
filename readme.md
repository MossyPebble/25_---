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

1. 추후에 추가해야 할 것
    - 카메라 조정용 servo API
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