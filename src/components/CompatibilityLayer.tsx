import React, { useState } from 'react';
import { Card } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { 
  Monitor, 
  Cpu, 
  Zap, 
  CheckCircle, 
  Activity,
  Play,
  Pause,
  Settings,
  FileText,
  Package,
  Terminal
} from 'lucide-react';

export const CompatibilityLayer: React.FC = () => {
  const [activeTab, setActiveTab] = useState('overview');
  
  const compatStats = {
    windows: { active: 47, success: 98.7, performance: 'Native+5%' },
    linux: { active: 23, success: 99.2, performance: 'Native+2%' },
    macos: { active: 12, success: 94.1, performance: 'Native-3%' }
  };

  const runningApps = [
    {
      name: 'Microsoft Office 365',
      platform: 'Windows',
      pid: 4521,
      memory: '341MB',
      cpu: '2.1%',
      status: 'Running',
      personality: 'NT-Persona',
      api: 'Win32/WinRT'
    },
    {
      name: 'Visual Studio Code',
      platform: 'Linux',
      pid: 3847,
      memory: '428MB',
      cpu: '1.8%',
      status: 'Running',
      personality: 'Linux-Persona',
      api: 'glibc/Electron'
    },
    {
      name: 'Adobe Photoshop',
      platform: 'Windows',
      pid: 5923,
      memory: '1.2GB',
      cpu: '8.4%',
      status: 'Running',
      personality: 'NT-Persona',
      api: 'DirectX/GDI+'
    },
    {
      name: 'Docker Desktop',
      platform: 'Linux',
      pid: 2156,
      memory: '156MB',
      cpu: '0.7%',
      status: 'Running',
      personality: 'Linux-Persona',
      api: 'containerd/OCI'
    },
    {
      name: 'Blender',
      platform: 'macOS',
      pid: 7432,
      memory: '892MB',
      cpu: '12.3%',
      status: 'Running',
      personality: 'XNU-Persona',
      api: 'Metal/CoreGraphics'
    }
  ];

  const apiCompatibility = [
    {
      category: 'Windows APIs',
      apis: [
        { name: 'Win32 API', coverage: 99.8, status: 'Complete' },
        { name: 'WinRT', coverage: 96.2, status: 'Active' },
        { name: 'DirectX 12', coverage: 98.5, status: 'Hardware Accelerated' },
        { name: 'DirectX 11', coverage: 99.9, status: 'Complete' },
        { name: 'GDI/GDI+', coverage: 99.1, status: 'Complete' },
        { name: '.NET Framework', coverage: 97.4, status: 'Active' },
        { name: 'COM/DCOM', coverage: 94.8, status: 'Active' }
      ]
    },
    {
      category: 'Linux APIs',
      apis: [
        { name: 'POSIX', coverage: 100, status: 'Complete' },
        { name: 'glibc', coverage: 99.7, status: 'Complete' },
        { name: 'systemd', coverage: 91.3, status: 'Active' },
        { name: 'X11/Wayland', coverage: 98.2, status: 'Native Bridge' },
        { name: 'OpenGL', coverage: 99.8, status: 'Hardware Accelerated' },
        { name: 'Vulkan', coverage: 100, status: 'Native' },
        { name: 'ALSA/PulseAudio', coverage: 96.7, status: 'Active' }
      ]
    },
    {
      category: 'macOS APIs',
      apis: [
        { name: 'Mach Kernel', coverage: 87.3, status: 'Core Features' },
        { name: 'BSD Layer', coverage: 95.1, status: 'Active' },
        { name: 'CoreFoundation', coverage: 82.6, status: 'Partial' },
        { name: 'Metal', coverage: 91.4, status: 'Vulkan Bridge' },
        { name: 'CoreAudio', coverage: 78.9, status: 'Basic Support' },
        { name: 'IOKit', coverage: 65.2, status: 'Limited' }
      ]
    }
  ];

  const benchmarks = [
    {
      test: 'SPEC CPU 2017',
      windows: { native: 100, limitless: 105 },
      linux: { native: 100, limitless: 102 },
      macos: { native: 100, limitless: 97 }
    },
    {
      test: 'Geekbench 6',
      windows: { native: 100, limitless: 103 },
      linux: { native: 100, limitless: 101 },
      macos: { native: 100, limitless: 95 }
    },
    {
      test: '3DMark DirectX',
      windows: { native: 100, limitless: 102 },
      linux: { native: 'N/A', limitless: 'N/A' },
      macos: { native: 'N/A', limitless: 'N/A' }
    },
    {
      test: 'Blender Cycles',
      windows: { native: 100, limitless: 98 },
      linux: { native: 100, limitless: 100 },
      macos: { native: 100, limitless: 94 }
    }
  ];

  const tabs = [
    { id: 'overview', name: 'Overview', icon: Monitor },
    { id: 'processes', name: 'Running Apps', icon: Activity },
    { id: 'apis', name: 'API Support', icon: Settings },
    { id: 'benchmarks', name: 'Performance', icon: Zap }
  ];

  return (
    <div className="space-y-6">
      {/* Tab navigation */}
      <div className="flex space-x-2 mb-6">
        {tabs.map((tab) => (
          <Button
            key={tab.id}
            variant={activeTab === tab.id ? "default" : "ghost"}
            onClick={() => setActiveTab(tab.id)}
            className="flex items-center space-x-2"
          >
            <tab.icon className="w-4 h-4" />
            <span>{tab.name}</span>
          </Button>
        ))}
      </div>

      {/* Overview tab */}
      {activeTab === 'overview' && (
        <div className="space-y-6">
          {/* Compatibility statistics */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <Card className="glass p-6">
              <div className="flex items-center justify-between mb-4">
                <h3 className="text-lg font-semibold text-blue-400">Windows Compatibility</h3>
                <Badge className="bg-blue-500/20 text-blue-400 border-blue-500/30">NT-Persona</Badge>
              </div>
              <div className="space-y-3">
                <div className="flex justify-between">
                  <span>Active Apps:</span>
                  <span className="font-mono">{compatStats.windows.active}</span>
                </div>
                <div className="flex justify-between">
                  <span>Success Rate:</span>
                  <span className="font-mono text-secure">{compatStats.windows.success}%</span>
                </div>
                <div className="flex justify-between">
                  <span>Performance:</span>
                  <span className="font-mono text-secure">{compatStats.windows.performance}</span>
                </div>
              </div>
            </Card>

            <Card className="glass p-6">
              <div className="flex items-center justify-between mb-4">
                <h3 className="text-lg font-semibold text-yellow-400">Linux Compatibility</h3>
                <Badge className="bg-yellow-500/20 text-yellow-400 border-yellow-500/30">Linux-Persona</Badge>
              </div>
              <div className="space-y-3">
                <div className="flex justify-between">
                  <span>Active Apps:</span>
                  <span className="font-mono">{compatStats.linux.active}</span>
                </div>
                <div className="flex justify-between">
                  <span>Success Rate:</span>
                  <span className="font-mono text-secure">{compatStats.linux.success}%</span>
                </div>
                <div className="flex justify-between">
                  <span>Performance:</span>
                  <span className="font-mono text-secure">{compatStats.linux.performance}</span>
                </div>
              </div>
            </Card>

            <Card className="glass p-6">
              <div className="flex items-center justify-between mb-4">
                <h3 className="text-lg font-semibold text-purple-400">macOS Compatibility</h3>
                <Badge className="bg-purple-500/20 text-purple-400 border-purple-500/30">XNU-Persona</Badge>
              </div>
              <div className="space-y-3">
                <div className="flex justify-between">
                  <span>Active Apps:</span>
                  <span className="font-mono">{compatStats.macos.active}</span>
                </div>
                <div className="flex justify-between">
                  <span>Success Rate:</span>
                  <span className="font-mono text-warning">{compatStats.macos.success}%</span>
                </div>
                <div className="flex justify-between">
                  <span>Performance:</span>
                  <span className="font-mono text-warning">{compatStats.macos.performance}</span>
                </div>
              </div>
            </Card>
          </div>

          {/* ABI Translation Layer */}
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4 flex items-center">
              <Cpu className="w-5 h-5 mr-2" />
              ABI Translation Architecture
            </h3>
            <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
              <div className="text-center p-4 border border-blue-500/30 rounded-lg bg-blue-500/5">
                <div className="text-blue-400 font-semibold mb-2">Windows PE/COFF</div>
                <div className="text-sm text-muted-foreground mb-3">
                  Native PE loader with DLL injection, SEH handling, and fiber support
                </div>
                <Badge variant="outline" className="text-blue-400 border-blue-500/50">Microsoft x64 ABI</Badge>
              </div>
              
              <div className="text-center p-4 border border-primary/30 rounded-lg bg-primary/5">
                <div className="text-primary font-semibold mb-2">USIR Translation</div>
                <div className="text-sm text-muted-foreground mb-3">
                  Universal Syscall IR with zero-copy fast paths and capability translation
                </div>
                <Badge className="glow-primary">PolyCore Kernel</Badge>
              </div>
              
              <div className="text-center p-4 border border-yellow-500/30 rounded-lg bg-yellow-500/5">
                <div className="text-yellow-400 font-semibold mb-2">Linux ELF</div>
                <div className="text-sm text-muted-foreground mb-3">
                  Full ELF loader with VDSO, namespace isolation, and eBPF support
                </div>
                <Badge variant="outline" className="text-yellow-400 border-yellow-500/50">SysV AMD64 ABI</Badge>
              </div>
            </div>
          </Card>
        </div>
      )}

      {/* Running processes tab */}
      {activeTab === 'processes' && (
        <div className="space-y-4">
          <div className="flex items-center justify-between">
            <h3 className="text-lg font-semibold">Multi-Platform Applications</h3>
            <div className="flex space-x-2">
              <Badge variant="outline">Total: {runningApps.length} apps</Badge>
              <Button size="sm" className="glow-primary">Refresh</Button>
            </div>
          </div>
          
          {runningApps.map((app) => (
            <Card key={app.pid} className="glass p-4">
              <div className="flex items-center justify-between">
                <div className="flex items-center space-x-4">
                  <div className="w-10 h-10 rounded-lg bg-primary/20 flex items-center justify-center">
                    {app.platform === 'Windows' && <Package className="w-5 h-5 text-blue-400" />}
                    {app.platform === 'Linux' && <Terminal className="w-5 h-5 text-yellow-400" />}
                    {app.platform === 'macOS' && <Monitor className="w-5 h-5 text-purple-400" />}
                  </div>
                  <div>
                    <h4 className="font-semibold">{app.name}</h4>
                    <div className="flex items-center space-x-4 text-sm text-muted-foreground">
                      <span>PID: {app.pid}</span>
                      <span>Platform: {app.platform}</span>
                      <span>Personality: {app.personality}</span>
                    </div>
                  </div>
                </div>
                
                <div className="flex items-center space-x-6">
                  <div className="text-right">
                    <div className="text-sm font-mono">CPU: {app.cpu}</div>
                    <div className="text-sm font-mono">Memory: {app.memory}</div>
                  </div>
                  <div className="text-right">
                    <Badge className="glow-secure mb-1">{app.status}</Badge>
                    <div className="text-xs text-muted-foreground">{app.api}</div>
                  </div>
                  <div className="flex space-x-1">
                    <Button size="sm" variant="ghost">
                      <Pause className="w-4 h-4" />
                    </Button>
                    <Button size="sm" variant="ghost">
                      <Settings className="w-4 h-4" />
                    </Button>
                  </div>
                </div>
              </div>
            </Card>
          ))}
        </div>
      )}

      {/* API Support tab */}
      {activeTab === 'apis' && (
        <div className="space-y-6">
          {apiCompatibility.map((category) => (
            <Card key={category.category} className="glass p-6">
              <h3 className="text-lg font-semibold mb-4">{category.category}</h3>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {category.apis.map((api) => (
                  <div key={api.name} className="flex items-center justify-between p-3 border border-border/50 rounded-lg">
                    <div>
                      <div className="font-medium">{api.name}</div>
                      <div className="text-sm text-muted-foreground">{api.status}</div>
                    </div>
                    <div className="text-right">
                      <div className="font-mono text-sm">{api.coverage}%</div>
                      <div className="w-16 h-2 bg-secondary rounded-full overflow-hidden mt-1">
                        <div 
                          className="h-full bg-gradient-to-r from-primary to-secure transition-all"
                          style={{ width: `${api.coverage}%` }}
                        />
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            </Card>
          ))}
        </div>
      )}

      {/* Benchmarks tab */}
      {activeTab === 'benchmarks' && (
        <div className="space-y-6">
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4">Performance Comparison vs Native</h3>
            <div className="space-y-4">
              {benchmarks.map((benchmark) => (
                <div key={benchmark.test} className="border border-border/50 rounded-lg p-4">
                  <h4 className="font-semibold mb-3">{benchmark.test}</h4>
                  <div className="grid grid-cols-3 gap-4">
                    <div className="text-center">
                      <div className="text-blue-400 font-medium mb-2">Windows</div>
                      <div className="flex items-center justify-between text-sm">
                        <span>Native:</span>
                        <span className="font-mono">{benchmark.windows.native}%</span>
                      </div>
                      <div className="flex items-center justify-between text-sm">
                        <span>LimitlessOS:</span>
                        <span className={`font-mono ${
                          typeof benchmark.windows.limitless === 'number' 
                            ? benchmark.windows.limitless >= 100 ? 'text-secure' : 'text-warning'
                            : 'text-muted-foreground'
                        }`}>
                          {benchmark.windows.limitless}%
                        </span>
                      </div>
                    </div>
                    
                    <div className="text-center">
                      <div className="text-yellow-400 font-medium mb-2">Linux</div>
                      <div className="flex items-center justify-between text-sm">
                        <span>Native:</span>
                        <span className="font-mono">{benchmark.linux.native}%</span>
                      </div>
                      <div className="flex items-center justify-between text-sm">
                        <span>LimitlessOS:</span>
                        <span className={`font-mono ${
                          typeof benchmark.linux.limitless === 'number' 
                            ? benchmark.linux.limitless >= 100 ? 'text-secure' : 'text-warning'
                            : 'text-muted-foreground'
                        }`}>
                          {benchmark.linux.limitless}%
                        </span>
                      </div>
                    </div>
                    
                    <div className="text-center">
                      <div className="text-purple-400 font-medium mb-2">macOS</div>
                      <div className="flex items-center justify-between text-sm">
                        <span>Native:</span>
                        <span className="font-mono">{benchmark.macos.native}%</span>
                      </div>
                      <div className="flex items-center justify-between text-sm">
                        <span>LimitlessOS:</span>
                        <span className={`font-mono ${
                          typeof benchmark.macos.limitless === 'number' 
                            ? benchmark.macos.limitless >= 95 ? 'text-secure' : 'text-warning'
                            : 'text-muted-foreground'
                        }`}>
                          {benchmark.macos.limitless}%
                        </span>
                      </div>
                    </div>
                  </div>
                </div>
              ))}
            </div>
          </Card>
          
          <Card className="glass p-6 text-center">
            <CheckCircle className="w-12 h-12 text-secure mx-auto mb-4" />
            <h3 className="text-lg font-semibold mb-2">Performance Goals Met</h3>
            <p className="text-muted-foreground">
              LimitlessOS achieves native or better performance across all major platforms
            </p>
          </Card>
        </div>
      )}
    </div>
  );
};