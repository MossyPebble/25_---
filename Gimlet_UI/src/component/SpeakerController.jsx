import React, { useEffect } from 'react';
import PCMPlayer from 'pcm-player';

function SpeakerController({ esp32MicIP, speakerEnabled }) {
    let ws, player;
    const [volume, setVolume] = React.useState(1);

    useEffect(() => {
        if (speakerEnabled == 'speaker') {
            connectWebSocket();
        } else {
            disconnectWebSocket();
        }

        return () => {
            disconnectWebSocket();
        };
    }, [speakerEnabled]);

    function connectWebSocket() {
        console.log("Connecting to WebSocket...");
        const WS_URL = `ws://${esp32MicIP}:81`; // ESP32의 WebSocket 주소
        ws = new WebSocket(WS_URL);
        ws.binaryType = 'arraybuffer';

        // PCMPlayer 초기화
        player = new PCMPlayer({
            inputCodec: 'Int16',
            channels: 1,
            sampleRate: 16000,
        });

        // WebSocket 이벤트 처리
        ws.addEventListener('message', function (event) {
            if (speakerEnabled == 'speaker') {
                const pcmData = new Int16Array(event.data);
                console.log("PCM Data Length:", pcmData.length); // 샘플 개수 확인
                console.log("PCM Data:", pcmData); // PCM 데이터 내용 확인
                player.feed(event.data);
            }
        });
    }

    function disconnectWebSocket() {
        console.log("Disconnecting WebSocket...");
        if (ws) {
            ws.close();
            ws = null;
        }
        if (player) {
            player.destroy();
            player = null;
        }
    }

    function handleVolumeChange(e) {
        const newVolume = parseFloat(e.target.value);
        setVolume(newVolume);
        if (player) {
            player.volume(newVolume); // PCMPlayer의 볼륨 설정
        }
    }

    return (
        <>
            {speakerEnabled === 'speaker' && (
                <>
                    <label htmlFor="volume">Volume: {Math.round(volume * 100)}%</label>
                    <input
                        id="volume"
                        type="range"
                        min="0"
                        max="1"
                        step="0.01"
                        value={volume}
                        onChange={handleVolumeChange}
                    />
                </>
            )}
        </>
    );
}

export default SpeakerController;