import React, { useState, useEffect, useRef } from 'react';
import { Card } from '@/components/ui/card';
import { Input } from '@/components/ui/input';
import { Button } from '@/components/ui/button';
import { Badge } from '@/components/ui/badge';
import { 
  Terminal as TerminalIcon, 
  Minimize2, 
  Maximize2, 
  X,
  Folder,
  User,
  Settings,
  Trash2
} from 'lucide-react';
import { useTerminal } from '@/hooks/useTerminal';
import { useAuth } from '@/hooks/useAuth';

export const Terminal: React.FC = () => {
  const { profile } = useAuth();
  const { currentDirectory, commandHistory, runCommand } = useTerminal();
  const [currentCommand, setCurrentCommand] = useState('');
  const [isMinimized, setIsMinimized] = useState(false);
  const [isFullscreen, setIsFullscreen] = useState(false);
  const [isProcessing, setIsProcessing] = useState(false);
  const terminalEndRef = useRef<HTMLDivElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);

  const handleCommandSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!currentCommand.trim() || isProcessing) return;
    
    setIsProcessing(true);
    await runCommand(currentCommand.trim());
    setCurrentCommand('');
    setIsProcessing(false);
    
    // Focus input after command execution
    setTimeout(() => inputRef.current?.focus(), 100);
  };

  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === 'ArrowUp') {
      e.preventDefault();
      // Get last command from history
      const lastCommand = commandHistory[commandHistory.length - 1];
      if (lastCommand) {
        setCurrentCommand(lastCommand.command);
      }
    } else if (e.key === 'ArrowDown') {
      e.preventDefault();
      setCurrentCommand('');
    } else if (e.key === 'Tab') {
      e.preventDefault();
      // Simple tab completion for common commands
      const commands = ['ls', 'cd', 'pwd', 'mkdir', 'touch', 'rm', 'cat', 'echo', 'help'];
      const matches = commands.filter(cmd => cmd.startsWith(currentCommand));
      if (matches.length === 1) {
        setCurrentCommand(matches[0] + ' ');
      }
    }
  };

  const clearTerminal = () => {
    runCommand('clear');
  };

  // Auto-scroll to bottom when new output appears
  useEffect(() => {
    terminalEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [commandHistory]);

  // Focus input on mount
  useEffect(() => {
    inputRef.current?.focus();
  }, []);

  const prompt = `${profile?.username || 'user'}@limitless-os:${currentDirectory}$`;

  if (isMinimized) {
    return (
      <Card className="glass fixed bottom-4 left-4 p-2 cursor-pointer" onClick={() => setIsMinimized(false)}>
        <div className="flex items-center space-x-2">
          <TerminalIcon className="w-4 h-4 text-primary" />
          <span className="text-sm font-mono">Terminal</span>
        </div>
      </Card>
    );
  }

  return (
    <Card className={`glass ${
      isFullscreen 
        ? 'fixed inset-0 z-50 rounded-none' 
        : 'fixed bottom-4 right-4 w-[800px] h-[600px]'
    } flex flex-col`}>
      {/* Terminal Header */}
      <div className="flex items-center justify-between p-3 border-b border-border/50">
        <div className="flex items-center space-x-3">
          <TerminalIcon className="w-5 h-5 text-primary" />
          <span className="font-mono text-sm">LimitlessOS Terminal</span>
          <Badge variant="outline" className="text-xs">
            <User className="w-3 h-3 mr-1" />
            {profile?.security_clearance?.toUpperCase() || 'USER'}
          </Badge>
        </div>
        
        <div className="flex items-center space-x-2">
          <Button
            variant="ghost"
            size="sm"
            onClick={clearTerminal}
            className="text-xs"
          >
            <Trash2 className="w-3 h-3 mr-1" />
            Clear
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={() => setIsMinimized(true)}
          >
            <Minimize2 className="w-4 h-4" />
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={() => setIsFullscreen(!isFullscreen)}
          >
            <Maximize2 className="w-4 h-4" />
          </Button>
        </div>
      </div>

      {/* Terminal Content */}
      <div className="flex-1 p-4 overflow-auto bg-background/50 font-mono text-sm">
        {/* Welcome message */}
        {commandHistory.length === 0 && (
          <div className="mb-4 text-muted-foreground">
            <div className="text-primary mb-2">Welcome to LimitlessOS Terminal</div>
            <div>Type 'help' for available commands</div>
            <div className="mt-2">Security Level: <span className="text-secure">{profile?.security_clearance?.toUpperCase() || 'USER'}</span></div>
            <div>System: <span className="text-accent">PolyCore Kernel v1.0</span></div>
            <div className="mb-4">━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</div>
          </div>
        )}

        {/* Command History */}
        {commandHistory.map((cmd, index) => (
          <div key={index} className="mb-2">
            <div className="flex items-center space-x-1 text-primary">
              <span className="text-secure">{prompt}</span>
              <span className="text-foreground">{cmd.command}</span>
            </div>
            {cmd.output && (
              <div className={`mt-1 whitespace-pre-wrap ${
                cmd.exitCode !== 0 ? 'text-destructive' : 'text-muted-foreground'
              }`}>
                {cmd.output}
              </div>
            )}
          </div>
        ))}

        {/* Current Input Line */}
        <form onSubmit={handleCommandSubmit} className="flex items-center space-x-1">
          <span className="text-secure">{prompt}</span>
          <Input
            ref={inputRef}
            value={currentCommand}
            onChange={(e) => setCurrentCommand(e.target.value)}
            onKeyDown={handleKeyDown}
            className="flex-1 border-none bg-transparent p-0 text-foreground font-mono focus:ring-0 focus:outline-none"
            placeholder={isProcessing ? 'Processing...' : 'Enter command...'}
            disabled={isProcessing}
            autoComplete="off"
          />
        </form>

        <div ref={terminalEndRef} />
      </div>

      {/* Terminal Status Bar */}
      <div className="px-4 py-2 border-t border-border/50 flex items-center justify-between text-xs text-muted-foreground">
        <div className="flex items-center space-x-4">
          <span>PWD: {currentDirectory}</span>
          <span>Commands: {commandHistory.length}</span>
        </div>
        <div className="flex items-center space-x-2">
          <div className="w-2 h-2 rounded-full bg-secure animate-pulse" />
          <span>Online</span>
        </div>
      </div>
    </Card>
  );
};