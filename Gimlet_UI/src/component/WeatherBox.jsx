import React, { useEffect, useState } from 'react';
import '../css/WeatherBox.css'; // CSS 파일 추가

function hexToRgb(hex) {
    const cleanHex = hex.replace('#', '');
    const bigint = parseInt(cleanHex, 16);
    return [(bigint >> 16) & 255, (bigint >> 8) & 255, bigint & 255];
};

function rgbToHex(r, g, b) {
    return `#${[r, g, b].map((x) => x.toString(16).padStart(2, '0')).join('')}`;
}

function interpolateColor(c1, c2, t) {
    const [r1, g1, b1] = hexToRgb(c1);
    const [r2, g2, b2] = hexToRgb(c2);
    const r = Math.round(r1 + (r2 - r1) * t);
    const g = Math.round(g1 + (g2 - g1) * t);
    const b = Math.round(b1 + (b2 - b1) * t);
    return rgbToHex(r, g, b);
};

function getInterpolatedBackground(temp) {
    const low = 0, mid = 20, high = 35;
    const cold = '#d0e8f2', mild = '#daf5dc', hot = '#ffe0b2';

    if (temp <= low) return cold;
    if (temp >= high) return hot;

    if (temp <= mid) {
        const t = (temp - low) / (mid - low);
        return interpolateColor(cold, mild, t);
    } else {
        const t = (temp - mid) / (high - mid);
        return interpolateColor(mild, hot, t);
    }
};

async function fetchWeatherData(wemosIP, setTemp, setHumidity) {
    try {
        // 데이터 가져오기
        const envResponse = await fetch(`http://${wemosIP}/env`);

        // 텍스트 데이터 가져오기
        const responseText = await envResponse.text();

        // \n으로 구분된 온도와 습도 데이터 파싱
        const [tempData, humidityData] = responseText.split('\n');
        console.log('Fetched weather data:', { tempData, humidityData });

        // 상태 업데이트
        setTemp(parseFloat(tempData));
        setHumidity(parseFloat(humidityData));
    } catch (error) {
        console.error('Error fetching weather data:', error);

        // 기본값 설정
        setTemp(25);
        setHumidity(50);
    }
};

function WeatherBox({ wemosIP, temperature, humidity, setTemp, setHumidity, children }) {
    const [bgColor, setBgColor] = useState(getInterpolatedBackground(temperature));

    useEffect(() => {
        setBgColor(getInterpolatedBackground(temperature));
    }, [temperature]);

    // 주기적으로 온도와 습도 업데이트
    useEffect(() => {
        const interval = setInterval(() => fetchWeatherData(wemosIP, setTemp, setHumidity), 30000);

        // 컴포넌트 언마운트 시 interval 정리
        return () => clearInterval(interval);
    }, [wemosIP]);

    // 수동 새로고침 핸들러
    const handleRefresh = () => {
        fetchWeatherData(wemosIP, setTemp, setHumidity);
    };

    return (
        <div
            className="weather-box"
            style={{
                background: `linear-gradient(to bottom, ${bgColor}, #ffffff)`, // 위에서 아래로 그라데이션
                transition: 'background-color 1s ease',
                position: 'relative',
            }}
        >
            {/* 온도/습도 카드 */}
            <div className="weather-card" style={{ position: 'relative' }}>
                <div className="temperature">{temperature}°C</div>
                <div className="humidity">습도 {humidity}%</div>
                <div className="feels-like">체감온도: {Math.round(temperature - (100 - humidity) / 5)}°C</div>

                {/* 새로고침 버튼 */}
                <button
                    onClick={handleRefresh}
                    style={{
                        position: 'absolute',
                        top: '10px',
                        right: '10px',
                        background: '#ffffff',
                        border: '1px solid #ccc',
                        borderRadius: '50%',
                        width: '30px',
                        height: '30px',
                        cursor: 'pointer',
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        boxShadow: '0 2px 4px rgba(0, 0, 0, 0.2)',
                    }}
                    title="새로고침"
                >
                    🔄
                </button>
            </div>

            {/* 하단에 추가로 넣고 싶은 children */}
            <div className="weather-children">
                {children}
            </div>
        </div>
    );
}

export default WeatherBox;