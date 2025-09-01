import React, { useState } from 'react';
import { Card } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { 
  Cpu, 
  MemoryStick, 
  Zap, 
  Shield, 
  Network, 
  HardDrive,
  Activity,
  Clock,
  Users,
  Settings
} from 'lucide-react';

export const PolyCoreKernel: React.FC = () => {
  const [activePersona, setActivePersona] = useState('nt');
  
  const kernelStats = {
    uptime: '127d 14h 32m',
    processes: 1247,
    memory: { used: '4.2GB', total: '32GB', percent: 13 },
    cpu: { usage: 12, cores: 16, frequency: '3.8GHz' },
    ipc: { messages: '2.1M/sec', latency: '0.3µs' },
    interrupts: '847K/sec'
  };

  const personalities = [
    {
      id: 'nt',
      name: 'NT-Persona',
      description: 'Windows NT API compatibility layer',
      status: 'Active',
      processes: 342,
      syscalls: '1.2M/sec',
      apis: ['Win32', 'WinRT', 'DirectX', 'NTDLL'],
      color: 'text-blue-400'
    },
    {
      id: 'linux',
      name: 'Linux-Persona',
      description: 'Linux syscall ABI implementation',
      status: 'Active',
      processes: 128,
      syscalls: '890K/sec',
      apis: ['POSIX', 'glibc', 'systemd', 'eBPF'],
      color: 'text-yellow-400'
    },
    {
      id: 'xnu',
      name: 'XNU-Persona',
      description: 'macOS Mach/XNU compatibility',
      status: 'Active',
      processes: 67,
      syscalls: '450K/sec',
      apis: ['Mach', 'BSD', 'CoreFoundation', 'IOKit'],
      color: 'text-purple-400'
    }
  ];

  const microservices = [
    { name: 'Memory Manager', status: 'Running', cpu: 2.1, memory: '128MB' },
    { name: 'Process Scheduler', status: 'Running', cpu: 1.8, memory: '64MB' },
    { name: 'IPC Router', status: 'Running', cpu: 3.2, memory: '256MB' },
    { name: 'Device Manager', status: 'Running', cpu: 0.9, memory: '92MB' },
    { name: 'Security Monitor', status: 'Running', cpu: 1.4, memory: '186MB' },
    { name: 'VFS Layer', status: 'Running', cpu: 2.7, memory: '312MB' }
  ];

  return (
    <div className="space-y-6">
      {/* Kernel overview */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <Card className="glass p-4">
          <div className="flex items-center space-x-3 mb-2">
            <Cpu className="w-5 h-5 text-primary" />
            <h3 className="font-semibold">CPU Usage</h3>
          </div>
          <div className="text-2xl font-mono">{kernelStats.cpu.usage}%</div>
          <div className="text-sm text-muted-foreground">
            {kernelStats.cpu.cores} cores @ {kernelStats.cpu.frequency}
          </div>
        </Card>

        <Card className="glass p-4">
          <div className="flex items-center space-x-3 mb-2">
            <MemoryStick className="w-5 h-5 text-accent" />
            <h3 className="font-semibold">Memory</h3>
          </div>
          <div className="text-2xl font-mono">{kernelStats.memory.used}</div>
          <div className="text-sm text-muted-foreground">
            of {kernelStats.memory.total} ({kernelStats.memory.percent}%)
          </div>
        </Card>

        <Card className="glass p-4">
          <div className="flex items-center space-x-3 mb-2">
            <Zap className="w-5 h-5 text-secure" />
            <h3 className="font-semibold">IPC Throughput</h3>
          </div>
          <div className="text-2xl font-mono">{kernelStats.ipc.messages}</div>
          <div className="text-sm text-muted-foreground">
            {kernelStats.ipc.latency} avg latency
          </div>
        </Card>
      </div>

      {/* Personality modules */}
      <Card className="glass p-6">
        <h3 className="text-xl font-semibold mb-4 flex items-center">
          <Users className="w-5 h-5 mr-2" />
          Personality Modules
        </h3>
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-4">
          {personalities.map((persona) => (
            <Card 
              key={persona.id}
              className={`glass cursor-pointer transition-all border-2 ${
                activePersona === persona.id ? 'border-primary glow-primary' : 'border-border'
              }`}
              onClick={() => setActivePersona(persona.id)}
            >
              <div className="p-4">
                <div className="flex items-center justify-between mb-2">
                  <h4 className={`font-semibold ${persona.color}`}>{persona.name}</h4>
                  <Badge variant="secondary">{persona.status}</Badge>
                </div>
                <p className="text-sm text-muted-foreground mb-3">{persona.description}</p>
                <div className="space-y-2 text-xs font-mono">
                  <div className="flex justify-between">
                    <span>Processes:</span>
                    <span className="text-primary">{persona.processes}</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Syscalls/sec:</span>
                    <span className="text-accent">{persona.syscalls}</span>
                  </div>
                </div>
                <div className="mt-3">
                  <div className="text-xs text-muted-foreground mb-1">APIs:</div>
                  <div className="flex flex-wrap gap-1">
                    {persona.apis.map((api) => (
                      <Badge key={api} variant="outline" className="text-xs">
                        {api}
                      </Badge>
                    ))}
                  </div>
                </div>
              </div>
            </Card>
          ))}
        </div>
      </Card>

      {/* Microservices status */}
      <Card className="glass p-6">
        <h3 className="text-xl font-semibold mb-4 flex items-center">
          <Activity className="w-5 h-5 mr-2" />
          Core Microservices
        </h3>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {microservices.map((service) => (
            <Card key={service.name} className="glass p-4">
              <div className="flex items-center justify-between mb-2">
                <h4 className="font-medium">{service.name}</h4>
                <Badge className="glow-secure text-xs">{service.status}</Badge>
              </div>
              <div className="space-y-1 text-sm font-mono">
                <div className="flex justify-between">
                  <span className="text-muted-foreground">CPU:</span>
                  <span className="text-primary">{service.cpu}%</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-muted-foreground">Memory:</span>
                  <span className="text-accent">{service.memory}</span>
                </div>
              </div>
            </Card>
          ))}
        </div>
      </Card>

      {/* Real-time kernel metrics */}
      <Card className="glass p-6">
        <h3 className="text-xl font-semibold mb-4 flex items-center">
          <Clock className="w-5 h-5 mr-2" />
          Real-Time Metrics
        </h3>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 text-center font-mono">
          <div>
            <div className="text-2xl font-bold text-primary">{kernelStats.uptime}</div>
            <div className="text-sm text-muted-foreground">System Uptime</div>
          </div>
          <div>
            <div className="text-2xl font-bold text-accent">{kernelStats.processes}</div>
            <div className="text-sm text-muted-foreground">Active Processes</div>
          </div>
          <div>
            <div className="text-2xl font-bold text-secure">{kernelStats.interrupts}</div>
            <div className="text-sm text-muted-foreground">Interrupts/sec</div>
          </div>
          <div>
            <div className="text-2xl font-bold text-warning">0.3µs</div>
            <div className="text-sm text-muted-foreground">Context Switch</div>
          </div>
        </div>
      </Card>
    </div>
  );
};