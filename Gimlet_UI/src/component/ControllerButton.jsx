import React, { useState } from 'react';

function ControllerButton({ children, id, wemosIP, onMouseDown, onMouseUp }) {
    const [pressed, setPressed] = useState(false);

    return (
        <button
            id={id}
            onMouseDown={() => {
                setPressed(true);
                onMouseDown(wemosIP);
            }}
            onMouseUp={() => {
                setPressed(false);
                onMouseUp(wemosIP);
            }}
            onMouseLeave={() => {
                setPressed(false);
            }}
            style={{
                height: '60px',
                minWidth: '60px',
                fontSize: '1.5rem',
                border: 'none',
                borderRadius: '12px',
                backgroundColor: pressed ? '#dcdcdc' : '#f0f0f0',
                boxShadow: pressed
                    ? 'inset 0 2px 4px rgba(0,0,0,0.2)'
                    : '0 2px 6px rgba(0,0,0,0.15)',
                cursor: 'pointer',
                transform: pressed ? 'scale(0.96)' : 'scale(1)',
                transition: 'all 0.1s ease-in-out',
            }}
        >
            {children}
        </button>
    );
};

export default ControllerButton;