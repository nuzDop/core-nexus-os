import React, { useState } from 'react';
import { Button } from '@/components/ui/button';
import { Card } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { 
  Shield, 
  Cpu, 
  HardDrive, 
  Monitor, 
  Lock, 
  Zap,
  Terminal,
  FolderOpen,
  Network,
  Settings
} from 'lucide-react';
import heroImage from '@/assets/limitless-os-hero.jpg';

import { PolyCoreKernel } from './PolyCoreKernel';
import { InfinityFS } from './InfinityFS';
import { SecurityCenter } from './SecurityCenter';
import { CompatibilityLayer } from './CompatibilityLayer';

interface DesktopProps {
  className?: string;
}

export const Desktop: React.FC<DesktopProps> = ({ className }) => {
  const [activeWindow, setActiveWindow] = useState<string | null>(null);
  const [systemStatus] = useState({
    security: 'SECURE',
    kernel: 'POLYCORE v1.0',
    filesystem: 'InfinityFS',
    uptime: '127d 14h 32m',
    compatibility: 'Windows | Linux | macOS'
  });

  const windows = [
    { id: 'polycore', title: 'PolyCore Kernel', icon: Cpu, component: PolyCoreKernel },
    { id: 'infinityfs', title: 'InfinityFS Manager', icon: HardDrive, component: InfinityFS },
    { id: 'security', title: 'Security Center', icon: Shield, component: SecurityCenter },
    { id: 'compatibility', title: 'Multi-ABI Compatibility', icon: Monitor, component: CompatibilityLayer }
  ];

  const quickActions = [
    { name: 'Terminal', icon: Terminal, status: 'ready' },
    { name: 'File System', icon: FolderOpen, status: 'active' },
    { name: 'Network', icon: Network, status: 'secure' },
    { name: 'Settings', icon: Settings, status: 'ready' }
  ];

  return (
    <div className={`min-h-screen bg-background relative overflow-hidden ${className}`}>
      {/* Background with hero image overlay */}
      <div className="absolute inset-0">
        <div className="absolute inset-0 bg-background/80 z-10" />
        <img 
          src={heroImage} 
          alt="LimitlessOS Interface" 
          className="w-full h-full object-cover opacity-20"
        />
        <div className="absolute top-1/4 left-1/4 w-96 h-96 bg-primary/10 rounded-full blur-3xl z-0" />
        <div className="absolute bottom-1/4 right-1/4 w-96 h-96 bg-accent/10 rounded-full blur-3xl z-0" />
      </div>

      {/* Top status bar */}
      <div className="glass fixed top-0 left-0 right-0 z-50 px-6 py-3 border-b border-border/50">
        <div className="flex items-center justify-between">
          <div className="flex items-center space-x-4">
            <div className="flex items-center space-x-2">
              <Shield className="w-5 h-5 text-secure" />
              <span className="text-gradient font-bold text-xl tracking-wider">LimitlessOS</span>
            </div>
            <Badge variant="secondary" className="glow-secure">
              <Lock className="w-3 h-3 mr-1" />
              {systemStatus.security}
            </Badge>
          </div>
          
          <div className="flex items-center space-x-6 text-sm font-mono">
            <div className="text-muted-foreground">
              Kernel: <span className="text-primary">{systemStatus.kernel}</span>
            </div>
            <div className="text-muted-foreground">
              FS: <span className="text-accent">{systemStatus.filesystem}</span>
            </div>
            <div className="text-muted-foreground">
              Uptime: <span className="text-secure">{systemStatus.uptime}</span>
            </div>
            <Badge variant="outline" className="animate-pulse-glow">
              {systemStatus.compatibility}
            </Badge>
          </div>
        </div>
      </div>

      {/* Main content area */}
      <div className="relative z-20 pt-20 px-6 pb-24">
        {/* Hero section */}
        <div className="text-center py-12 space-y-6">
          <h1 className="text-6xl font-bold text-gradient tracking-tight">
            LimitlessOS
          </h1>
          <p className="text-xl text-muted-foreground max-w-3xl mx-auto">
            Military-grade operating system with PolyCore kernel architecture.
            <br />
            Native execution of Windows, Linux, and macOS applications without compatibility layers.
          </p>
          <div className="flex items-center justify-center space-x-4">
            <Badge className="glow-primary px-4 py-2 text-base">
              <Zap className="w-4 h-4 mr-2" />
              Zero-Copy Performance
            </Badge>
            <Badge className="glow-secure px-4 py-2 text-base">
              <Shield className="w-4 h-4 mr-2" />
              Military-Grade Security
            </Badge>
          </div>
        </div>

        {/* System components grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-12">
          {windows.map((window) => (
            <Card 
              key={window.id}
              className="glass hover:glow-primary transition-all cursor-pointer p-6"
              onClick={() => setActiveWindow(window.id)}
            >
              <div className="flex items-center space-x-3 mb-4">
                <window.icon className="w-8 h-8 text-primary" />
                <h3 className="text-lg font-semibold">{window.title}</h3>
              </div>
              <p className="text-sm text-muted-foreground mb-4">
                {window.id === 'polycore' && 'Polymorphic hybrid microkernel with personality modules'}
                {window.id === 'infinityfs' && 'Next-generation copy-on-write filesystem with encryption'}
                {window.id === 'security' && 'Comprehensive security framework and threat monitoring'}
                {window.id === 'compatibility' && 'Native multi-ABI execution environment'}
              </p>
              <Button variant="military" className="w-full">
                Open Module
              </Button>
            </Card>
          ))}
        </div>
      </div>

      {/* Bottom dock */}
      <div className="glass fixed bottom-0 left-0 right-0 z-50 px-6 py-4 border-t border-border/50">
        <div className="flex items-center justify-center space-x-6">
          {quickActions.map((action) => (
            <Button 
              key={action.name}
              variant="ghost" 
              size="sm"
              className="flex items-center space-x-2"
            >
              <action.icon className="w-4 h-4" />
              <span className="hidden sm:inline">{action.name}</span>
              <Badge 
                variant={action.status === 'secure' ? 'default' : 'secondary'} 
                className="text-xs"
              >
                {action.status}
              </Badge>
            </Button>
          ))}
        </div>
      </div>

      {/* Active window modal */}
      {activeWindow && (
        <div className="fixed inset-0 z-50 bg-background/50 backdrop-blur-sm flex items-center justify-center p-6">
          <Card className="glass w-full max-w-6xl max-h-[90vh] overflow-auto">
            <div className="p-6">
              <div className="flex items-center justify-between mb-6">
                <h2 className="text-2xl font-bold text-gradient">
                  {windows.find(w => w.id === activeWindow)?.title}
                </h2>
                <Button 
                  variant="ghost" 
                  size="sm"
                  onClick={() => setActiveWindow(null)}
                >
                  ✕
                </Button>
              </div>
              {(() => {
                const Window = windows.find(w => w.id === activeWindow)?.component;
                return Window ? <Window /> : null;
              })()}
            </div>
          </Card>
        </div>
      )}
    </div>
  );
};