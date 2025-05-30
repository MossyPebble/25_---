import React, { useState } from "react";
import MicController from "./MicController";
import SpeakerController from "./SpeakerController";

function MicAndSpeakerBox({ esp32MicIP, esp32SpeakerIP }) {
    // 마이크와 스피커 상태 관리
    const [micSpeakerToggle, setMicSpeakerToggle] = useState("none");

    // 마이크 토글 함수
    function handleMicToggle() {
        setMicSpeakerToggle((prevState) => (prevState === "mic" ? "none" : "mic"));
    }

    // 스피커 토글 함수
    function handleSpeakerToggle() {
        setMicSpeakerToggle((prevState) => (prevState === "speaker" ? "none" : "speaker"));
    }

    return (
        <div style={{ display: "flex", flexDirection: "column", alignItems: "center", gap: "10px" }}>
            {/* 마이크 토글 버튼 */}
            <button
                onClick={handleMicToggle}
                style={{
                    padding: "10px 20px",
                    backgroundColor: micSpeakerToggle === "mic" ? "#007bff" : "#f0f0f0",
                    color: micSpeakerToggle === "mic" ? "#fff" : "#000",
                    border: "none",
                    borderRadius: "5px",
                    cursor: "pointer",
                }}
            >
                {micSpeakerToggle === "mic" ? "Turn Off Microphone" : "Turn On Microphone"}
            </button>

            {/* 스피커 토글 버튼 */}
            <button
                onClick={handleSpeakerToggle}
                style={{
                    padding: "10px 20px",
                    backgroundColor: micSpeakerToggle === "speaker" ? "#007bff" : "#f0f0f0",
                    color: micSpeakerToggle === "speaker" ? "#fff" : "#000",
                    border: "none",
                    borderRadius: "5px",
                    cursor: "pointer",
                }}
            >
                {micSpeakerToggle === "speaker" ? "Turn Off Speaker" : "Turn On Speaker"}
            </button>

            {/* 마이크 컨트롤러 */}
            <MicController
                esp32SpeakerIP={esp32SpeakerIP}
                micEnabled={micSpeakerToggle}
            />

            {/* 스피커 컨트롤러 */}
            <SpeakerController
                esp32MicIP={esp32MicIP}
                speakerEnabled={micSpeakerToggle}
            />
        </div>
    );
}

export default MicAndSpeakerBox;