import React, { useState } from 'react';
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