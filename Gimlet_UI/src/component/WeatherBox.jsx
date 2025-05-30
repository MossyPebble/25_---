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

function WeatherBox({ temperature, humidity, children }) {
    const [bgColor, setBgColor] = useState(getInterpolatedBackground(temperature));

    useEffect(() => {
        setBgColor(getInterpolatedBackground(temperature));
    }, [temperature]);

    return (
        <div
            className="weather-box"
            style={{
                background: `linear-gradient(to bottom, ${bgColor}, #ffffff)`, // 위에서 아래로 그라데이션
                transition: 'background-color 1s ease',
            }}
        >
            {/* 온도/습도 카드 */}
            <div className="weather-card">
                <div className="temperature">{temperature}°C</div>
                <div className="humidity">습도 {humidity}%</div>
                <div className="feels-like">체감온도: {Math.round(temperature - (100 - humidity) / 5)}°C</div>
            </div>

            {/* 하단에 추가로 넣고 싶은 children */}
            <div className="weather-children">
                {children}
            </div>
        </div>
    );
};

export default WeatherBox;