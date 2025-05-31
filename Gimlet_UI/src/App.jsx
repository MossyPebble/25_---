import React, { useState, useEffect } from 'react';
import WeatherBox from './component/WeatherBox.jsx';
import ControllerBox from './component/ControllerBox.jsx';
import CameraBox from './component/CameraBox.jsx';
import MicAndSpeakerBox from './component/MicAndSpeakerBox.jsx';
import './css/App.css';

function App() {

    // 온도와 습도 state
    const [temp, setTemp] = useState(25);
    const [humidity, setHumidity] = useState(55);

    // IP state
    const [wemosIP, setWemosIP] = useState('192.168.43.150');
    const [esp32CamIP, setEsp32CamIP] = useState('192.168.43.206:81');
    const [esp32MicIP, setEsp32MicIP] = useState('192.168.43.157');
    const [esp32SpeakerIP, setEsp32SpeakerIP] = useState('192.168.43.158');

    // 주기적으로 온도와 습도 업데이트
    useEffect(() => {
        const fetchWeatherData = async () => {
            try {
                // 온도 데이터 가져오기
                const tempResponse = await fetch(`http://${wemosIP}/temperature`);
                const humidityResponse = await fetch(`http://${wemosIP}/humidity`);

                if (!tempResponse.ok || !humidityResponse.ok) {
                    throw new Error('Failed to fetch weather data');
                }

                const tempData = await tempResponse.text();
                const humidityData = await humidityResponse.text();

                // 상태 업데이트
                setTemp(parseFloat(tempData));
                setHumidity(parseFloat(humidityData));
            } catch (error) {
                console.error('Error fetching weather data:', error);
                // 기본값 설정
                setTemp(25);
                setHumidity(55);
            }
        };

        // 5초마다 데이터 업데이트
        const interval = setInterval(fetchWeatherData, 5000);

        // 컴포넌트 언마운트 시 interval 정리
        return () => clearInterval(interval);
    }, [wemosIP]);

    return (
        <WeatherBox temperature={temp} humidity={humidity}>
            <div className='controller-container'>
                <div className="camera-box">
                    <CameraBox esp32CamIP={esp32CamIP} />
                </div>
                <div className="mic-speaker-box">
                    <MicAndSpeakerBox esp32MicIP={esp32MicIP} esp32SpeakerIP={esp32SpeakerIP} />
                </div>
            </div>
            <div className="controller-container">
                <div className="controller-box">
                    <ControllerBox wemos_ip={wemosIP} />
                </div>
            </div>
        </WeatherBox>
    );
};

export default App;