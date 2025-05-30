import React, { useState } from 'react';
import ControllerButton from './ControllerButton.jsx';

function ControllerBox({ wemos_ip }) {
    return (
        <div
            style={{
                display: 'flex',
                flexDirection: 'row', // 가로로 정렬
                justifyContent: 'center',
                alignItems: 'center',
                gap: '24px', // 두 그룹 간 간격
                padding: '1rem',
                borderRadius: '16px',
                backgroundColor: '#ffffffcc',
                boxShadow: '0 4px 12px rgba(0,0,0,0.1)',
                backdropFilter: 'blur(8px)',
                width: '100%',
                padding: 0
            }}
        >
            {/* 컨트롤러 버튼 그룹 */}
            <div
                style={{
                    display: 'flex',
                    flexDirection: 'column',
                    alignItems: 'center',
                    gap: '12px',
                }}
            >
                <ControllerButton
                    id="upButton"
                    wemosIP={wemos_ip}
                    onMouseDown={() => goForward(wemos_ip)}
                    onMouseUp={() => stop(wemos_ip)}
                >
                    ▲
                </ControllerButton>

                <div
                    style={{
                        display: 'flex',
                        justifyContent: 'center',
                        alignItems: 'center',
                        gap: '12px',
                    }}
                >
                    <ControllerButton
                        id="leftButton"
                        wemosIP={wemos_ip}
                        onMouseDown={() => turnLeft(wemos_ip)}
                        onMouseUp={() => stop(wemos_ip)}
                    >
                        ◀
                    </ControllerButton>
                    <div style={{ width: '60px', height: '60px' }}></div>
                    <ControllerButton
                        id="rightButton"
                        wemosIP={wemos_ip}
                        onMouseDown={() => turnRight(wemos_ip)}
                        onMouseUp={() => stop(wemos_ip)}
                    >
                        ▶
                    </ControllerButton>
                </div>

                <ControllerButton
                    id="downButton"
                    wemosIP={wemos_ip}
                    onMouseDown={() => goBackward(wemos_ip)}
                    onMouseUp={() => stop(wemos_ip)}
                >
                    ▼
                </ControllerButton>
            </div>

            {/* Motor Speed 버튼 그룹 */}
            <div
                style={{
                    display: 'flex',
                    flexDirection: 'column',
                    alignItems: 'center',
                    gap: '12px',
                    paddingBottom: '20px',
                }}
            >
                <h3 style={{ marginBottom: '10px' }}>Motor Speed</h3>
                <ControllerButton
                    id="motorSpeedHighButton"
                    wemosIP={wemos_ip}
                    onMouseDown={() => setMotorSpeed(wemos_ip, 'HIGH')}
                    onMouseUp={() => {}}
                >
                    HIGH
                </ControllerButton>
                <ControllerButton
                    id="motorSpeedMediumButton"
                    wemosIP={wemos_ip}
                    onMouseDown={() => setMotorSpeed(wemos_ip, 'MEDIUM')}
                    onMouseUp={() => {}}
                >
                    MEDIUM
                </ControllerButton>
                <ControllerButton
                    id="motorSpeedLowButton"
                    wemosIP={wemos_ip}
                    onMouseDown={() => setMotorSpeed(wemos_ip, 'LOW')}
                    onMouseUp={() => {}}
                >
                    LOW
                </ControllerButton>
            </div>

            {/* Servo Control 컴포넌트 */}
            <ServoControl wemos_ip={wemos_ip} />
        </div>
    );
}

function ServoControl({ wemos_ip }) {
    return (
        <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
            <h3>Dual Servo Control</h3>
            <div>
                <ControllerButton
                    id="servoUpButton"
                    onMouseDown={() => sendServoCommand(wemos_ip, 'UP_START')}
                    onMouseUp={() => sendServoCommand(wemos_ip,'UP_STOP')}
                >
                    ▲
                </ControllerButton>
            </div>
            <div style={{ display: 'flex', justifyContent: 'center', marginTop: '10px' }}>
                <ControllerButton
                    id="servoLeftButton"
                    onMouseDown={() => sendServoCommand(wemos_ip,'LEFT_START')}
                    onMouseUp={() => sendServoCommand(wemos_ip, 'LEFT_STOP')}
                >
                    ◀
                </ControllerButton>
                <div style={{ width: '10px' }}></div>
                <ControllerButton
                    id="servoRightButton"
                    onMouseDown={() => sendServoCommand(wemos_ip, 'RIGHT_START')}
                    onMouseUp={() => sendServoCommand(wemos_ip, 'RIGHT_STOP')}
                >
                    ▶
                </ControllerButton>
            </div>
            <div style={{ marginTop: '10px' }}>
                <ControllerButton
                    id="servoDownButton"
                    onMouseDown={() => sendServoCommand(wemos_ip, 'DOWN_START')}
                    onMouseUp={() => sendServoCommand(wemos_ip, 'DOWN_STOP')}
                >
                    ▼
                </ControllerButton>
            </div>
        </div>
    );
}

function sendServoCommand(wemos_ip, cmd) {
    if (!wemos_ip) {
        alert('IP 주소를 먼저 입력하세요.');
        return;
    }
    fetch(`http://${wemos_ip}/${cmd}`)
        .then(() => console.log('Sent:', cmd))
        .catch((err) => console.error('Servo command error:', err));
};

function setMotorSpeed(wemos_ip, speed) {
    fetch(`http://${wemos_ip}/Speed=${speed}`);
    console.log(`Motor Speed set to: ${speed}`);
}

function goForward(wemos_ip) {
    fetch('http://' + wemos_ip + '/LMotor=ON');
    fetch('http://' + wemos_ip + '/RMotor=ON');
    fetch('http://' + wemos_ip + '/LMotor=FORWARD');
    fetch('http://' + wemos_ip + '/RMotor=FORWARD');
    console.log('UP');
}

function goBackward(wemos_ip) {
    fetch('http://' + wemos_ip + '/LMotor=ON');
    fetch('http://' + wemos_ip + '/RMotor=ON');
    fetch('http://' + wemos_ip + '/LMotor=BACKWARD');
    fetch('http://' + wemos_ip + '/RMotor=BACKWARD');
    console.log('DOWN');
}

function stop(wemos_ip) {
    fetch('http://' + wemos_ip + '/LMotor=OFF');
    fetch('http://' + wemos_ip + '/RMotor=OFF');
    console.log('STOP');
}

function turnLeft(wemos_ip) {
    fetch('http://' + wemos_ip + '/LMotor=ON');
    fetch('http://' + wemos_ip + '/RMotor=OFF');
    fetch('http://' + wemos_ip + '/LMotor=FORWARD');
    console.log('LEFT');
}

function turnRight(wemos_ip) {
    fetch('http://' + wemos_ip + '/LMotor=OFF');
    fetch('http://' + wemos_ip + '/RMotor=ON');
    fetch('http://' + wemos_ip + '/RMotor=FORWARD');
    console.log('RIGHT');
}

export default ControllerBox;
