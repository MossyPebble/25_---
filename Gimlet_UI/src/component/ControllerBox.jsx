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
        .then(response => response.text())
        .then(data => console.log('Servo command response:', data))
        .catch((err) => console.error('Servo command error:', err));
};

function setMotorSpeed(wemos_ip, speed) {
    fetch(`http://${wemos_ip}/Speed=${speed}`)
        .then(response => response.text())
        .then(data => console.log('Motor speed response:', data))
        .catch(err => console.error('Error setting motor speed:', err));
}

function goForward(wemos_ip) {
    fetch(`http://${wemos_ip}/forward`)
        .then(response => response.text())
        .then(data => console.log('Response:', data))
        .catch(err => console.error('Error:', err));
}

function goBackward(wemos_ip) {
    fetch(`http://${wemos_ip}/backward`)
        .then(response => response.text())
        .then(data => console.log('Response:', data))
        .catch(err => console.error('Error:', err));
}

function stop(wemos_ip) {
    fetch(`http://${wemos_ip}/stop`)
        .then(response => response.text())
        .then(data => console.log('Response:', data))
        .catch(err => console.error('Error:', err));
    
    // 50ms 이후, 한번 더 stop 명령을 보내서 확실히 정지
    setTimeout(() => {
        fetch(`http://${wemos_ip}/stop`)
            .then(response => response.text())
            .then(data => console.log('Response:', data))
            .catch(err => console.error('Error:', err));
    }, 50);
}

function turnLeft(wemos_ip) {
    fetch(`http://${wemos_ip}/left`)
        .then(response => response.text())
        .then(data => console.log('Response:', data))
        .catch(err => console.error('Error:', err));
}

function turnRight(wemos_ip) {
    fetch(`http://${wemos_ip}/right`)
        .then(response => response.text())
        .then(data => console.log('Response:', data))
        .catch(err => console.error('Error:', err));
}

export default ControllerBox;
