import React, { useState } from 'react';

function formatDate(ts) {
  if (!ts) return new Date().toLocaleDateString('en-US', { month: 'short', day: 'numeric', year: 'numeric' });
  const d = new Date(typeof ts === 'number' ? ts * 1000 : ts);
  return d.toLocaleDateString('en-US', { month: 'short', day: 'numeric', year: 'numeric' });
}

export function StormTimeline({ storm, selectedIndex = 0, onSelectIndex }) {
  const path = storm?.predictedPath;
  const hasPath = path && Array.isArray(path) && path.length > 0;
  const labels = hasPath
    ? ['Now', ...path.map((p) => `${p.hours || 0}h`)]
    : ['Now'];

  return (
    <div className="absolute bottom-4 left-1/2 -translate-x-1/2 z-10 rounded-lg bg-gray-900/90 backdrop-blur border border-gray-700 shadow-xl px-4 py-3 min-w-[320px]">
      <div className="text-gray-400 text-xs font-medium mb-2">
        Storm Tracking: {formatDate(storm?.timestamp)}
      </div>
      <div className="flex items-center gap-2">
        <button
          type="button"
          className="w-8 h-8 rounded-full bg-cyan-600 hover:bg-cyan-500 flex items-center justify-center text-white"
          aria-label="Play"
        >
          <svg className="w-4 h-4 ml-0.5" fill="currentColor" viewBox="0 0 24 24">
            <path d="M8 5v14l11-7z" />
          </svg>
        </button>
        <div className="flex-1 flex items-center gap-1">
          {labels.map((label, i) => (
            <button
              key={i}
              type="button"
              onClick={() => onSelectIndex && onSelectIndex(i)}
              className={`flex-1 min-w-0 py-1.5 rounded text-xs font-medium transition-colors ${i === selectedIndex
                  ? 'bg-cyan-600 text-white'
                  : 'bg-gray-700/80 text-gray-300 hover:bg-gray-600'
                }`}
            >
              {label}
            </button>
          ))}
        </div>
      </div>
    </div>
  );
}

export default StormTimeline;
