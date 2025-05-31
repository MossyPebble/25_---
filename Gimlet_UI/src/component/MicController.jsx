import React, { useEffect, useRef } from 'react';

function MicController({ esp32SpeakerIP, micEnabled }) {
    const ws = useRef(null);
    const audioContext = useRef(null);
    const processor = useRef(null);
    const source = useRef(null);
    const latestBuffer = useRef(null);
    const sendInterval = useRef(null);

    useEffect(() => {
        console.log("MicController mounted with micEnabled:", micEnabled);
        if (micEnabled === 'mic') {
            connectWebSocket();
        } else {
            disconnectWebSocket();
        }

        return () => {
            disconnectWebSocket(); // cleanup on unmount
        };
    }, [micEnabled]);

    function connectWebSocket() {
        const WS_URL = 'ws://' + esp32SpeakerIP + ':81';
        ws.current = new WebSocket(WS_URL);

        ws.current.onopen = () => {
            console.log('WebSocket connected');
            startMicrophoneStream();
            startSendingLoop();
        };

        ws.current.onclose = () => {
            console.log('WebSocket closed');
            stopSendingLoop();
            stopMicrophone();
        };

        ws.current.onerror = (error) => {
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
        audioContext.current = new (window.AudioContext || window.webkitAudioContext)({ sampleRate: 16000 });
        source.current = audioContext.current.createMediaStreamSource(stream);
        processor.current = audioContext.current.createScriptProcessor(1024, 1, 1);

        source.current.connect(processor.current);
        processor.current.connect(audioContext.current.destination);

        processor.current.onaudioprocess = (event) => {
            const input = event.inputBuffer.getChannelData(0);
            const int16Buffer = Float32ToInt16(input);
            latestBuffer.current = int16Buffer.buffer; // 항상 최신 버퍼만 보존
        };

        console.log("Microphone stream started");
    }

    function startSendingLoop() {
        stopSendingLoop(); // 중복 방지

        sendInterval.current = setInterval(() => {
            if (ws.current && ws.current.readyState === WebSocket.OPEN && latestBuffer.current) {
                ws.current.send(latestBuffer.current);
                console.log("Sent audio data to WebSocket");
                latestBuffer.current = null; // 보내고 나면 비움
            }
        }, 10);
    }

    function stopSendingLoop() {
        if (sendInterval.current) {
            clearInterval(sendInterval.current);
            sendInterval.current = null;
            console.log("Sending loop stopped");
        }
    }

    function stopMicrophone() {
        if (processor.current) processor.current.disconnect();
        if (source.current) source.current.disconnect();
        if (audioContext.current) audioContext.current.close();

        // MediaStream 트랙 중지
        if (source.current && source.current.mediaStream) {
            source.current.mediaStream.getTracks().forEach((track) => track.stop());
        }

        processor.current = null;
        source.current = null;
        audioContext.current = null;

        console.log("Microphone stopped");
    }

    function disconnectWebSocket() {
        if (ws.current && ws.current.readyState === WebSocket.OPEN) {
        
            console.log("Sending silence before closing WebSocket");

            latestBuffer.current = new Int16Array(0).buffer; // 빈 버퍼로 초기화

            // 무음 16-bit PCM 버퍼 (예: 1024 샘플)
            const silentBuffer = new Int16Array(1024).fill(0);
            const silentArrayBuffer = silentBuffer.buffer;

            // 1~2회 정도 전송 (재생 장치가 남은 잔향을 처리하도록)
            ws.current.send(silentArrayBuffer);
            setTimeout(() => {
                ws.current.send(silentArrayBuffer);
                ws.current.close();
                console.log("WebSocket closed after sending silence");
            }, 20); // 약간의 시간 여유 (20ms)
        } else {
            console.log("WebSocket is not connected");
        }

        stopSendingLoop();
        stopMicrophone();
    }


    return null;
}

export default MicController;
