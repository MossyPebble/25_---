import React, { useState } from 'react';

function CameraBox({ esp32CamIP }) {

    // ESP32-CAM의 IP 주소를 사용하여 스트림 URL을 설정
    const streamURL = `http://${esp32CamIP}/stream`;

    // 연결 상태를 관리하는 state
    const [isError, setIsError] = useState(false);

    return (
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', marginTop: '20px' }}>
            {isError ? (

                // 연결 불가 메시지 표시
                <p style={{ color: 'red', fontSize: '1.2rem' }}>카메라 연결 불가!</p>
            ) : (
                    
                // 이미지 스트림 표시
                <img
                    id="stream"
                    src={streamURL}
                    crossOrigin=""
                    style={{ maxWidth: '100%', height: 'auto' }}
                        
                     // 이미지 로드 실패 시 isError를 true로 설정
                    onError={() => setIsError(true)} 
                />
            )}
        </div>
    );
}

export default CameraBox;