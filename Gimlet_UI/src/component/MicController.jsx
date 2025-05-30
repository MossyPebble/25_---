import React, { useState, useEffect, useRef } from 'react';

function MicController({ esp32SpeakerIP, micEnabled }) {
    let ws;
    let audioContext, processor, source;
    let latestBuffer = null;
    let sendInterval;

    useEffect(function () {
        if (micEnabled == 'mic') {
            connectWebSocket();
        } else {
            disconnectWebSocket();
        }
    }, [micEnabled]);

    function connectWebSocket() {
        let WS_URL = 'ws://' + esp32SpeakerIP + ':81';
        ws = new WebSocket(WS_URL);

        ws.onopen = function () {
            console.log('WebSocket connected');
            startMicrophoneStream();
            startSendingLoop();
        };

        ws.onclose = function () {
            console.log('WebSocket closed');
            stopSendingLoop();
            stopMicrophone();
        };

        ws.onerror = function (error) {
            console.error('WebSocket error:', error);
        };
    }

    function Float32ToInt16(float32Array) {
        const int16Array = new Int16Array(float32Array.length);
        for (let i = 0; i < float32Array.length; i++) {
            int16Array[i] = Math.max(-32768, Math.min(32767, float32Array[i] * 32767)) | 0;
        }
        return int16Array;
    }

    async function startMicrophoneStream() {
        const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
        audioContext = new (window.AudioContext || window.webkitAudioContext)({ sampleRate: 16000 });
        source = audioContext.createMediaStreamSource(stream);
        processor = audioContext.createScriptProcessor(1024, 1, 1);

        source.connect(processor);
        processor.connect(audioContext.destination);

        processor.onaudioprocess = (event) => {
            const input = event.inputBuffer.getChannelData(0);
            const int16Buffer = Float32ToInt16(input);
            // 항상 최신 버퍼만 보존
            latestBuffer = int16Buffer.buffer;
        };

        console.log("Microphone stream started");
    }

    function startSendingLoop() {
        sendInterval = setInterval(() => {
            if (ws && ws.readyState === WebSocket.OPEN && latestBuffer) {
                ws.send(latestBuffer);
                latestBuffer = null; // 보내고 나면 비움
            }
        }, 10); // wnrl
    }

    function stopSendingLoop() {
        if (sendInterval) {
            clearInterval(sendInterval);
            sendInterval = null;
        }
    }

    function stopMicrophone() {
        if (processor) processor.disconnect();
        if (source) source.disconnect();
        if (audioContext) audioContext.close();
    }

    function disconnectWebSocket() {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.close(); // WebSocket 연결 종료
            console.log("WebSocket disconnected manually");
        } else {
            console.log("WebSocket is not connected");
        }
    }

    return (
        <></>
    );
}

export default MicController;