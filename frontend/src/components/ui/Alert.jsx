import React from 'react';
import './Alert.css';

export function Alert({ className = '', children }) {
  return (
    <div className={`alert ${className}`}>
      {children}
    </div>
  );
}

export function AlertTitle({ children }) {
  return (
    <div className="alert-title">
      {children}
    </div>
  );
}

export function AlertDescription({ children }) {
  return (
    <div className="alert-description">
      {children}
    </div>
  );
}
