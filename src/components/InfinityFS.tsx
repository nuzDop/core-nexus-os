import React, { useState } from 'react';
import { Card } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { 
  HardDrive, 
  Copy, 
  Shield, 
  Zap, 
  Database, 
  FileText,
  Folder,
  Search,
  Clock,
  Lock,
  Activity,
  BarChart3
} from 'lucide-react';

export const InfinityFS: React.FC = () => {
  const [activeTab, setActiveTab] = useState('overview');
  
  const fsStats = {
    capacity: { used: '2.4TB', total: '8TB', percent: 30 },
    performance: { readIops: '145K', writeIops: '89K', throughput: '3.2GB/s' },
    compression: { ratio: '2.3:1', saved: '1.8TB' },
    dedup: { ratio: '1.7:1', saved: '892GB' },
    snapshots: 127,
    scrubStatus: 'Healthy',
    encryption: 'AES-256-XTS'
  };

  const volumes = [
    { 
      name: 'system', 
      path: '/system', 
      size: '120GB', 
      used: '67GB', 
      type: 'System Volume',
      compression: true,
      encryption: true,
      snapshots: 12
    },
    { 
      name: 'home', 
      path: '/home', 
      size: '2TB', 
      used: '890GB', 
      type: 'User Data',
      compression: true,
      encryption: true,
      snapshots: 24
    },
    { 
      name: 'apps', 
      path: '/apps', 
      size: '500GB', 
      used: '312GB', 
      type: 'Applications',
      compression: false,
      encryption: true,
      snapshots: 8
    },
    { 
      name: 'cache', 
      path: '/var/cache', 
      size: '100GB', 
      used: '43GB', 
      type: 'Cache Volume',
      compression: true,
      encryption: false,
      snapshots: 3
    }
  ];

  const recentSnapshots = [
    { id: 'snap-2024-01-15-1200', volume: 'system', created: '2 hours ago', size: '45MB' },
    { id: 'snap-2024-01-15-0800', volume: 'home', created: '6 hours ago', size: '178MB' },
    { id: 'snap-2024-01-15-0000', volume: 'system', created: '12 hours ago', size: '67MB' },
    { id: 'snap-2024-01-14-1800', volume: 'apps', created: '18 hours ago', size: '234MB' }
  ];

  const tabs = [
    { id: 'overview', name: 'Overview', icon: BarChart3 },
    { id: 'volumes', name: 'Volumes', icon: HardDrive },
    { id: 'snapshots', name: 'Snapshots', icon: Clock },
    { id: 'security', name: 'Security', icon: Lock }
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
          {/* Storage metrics */}
          <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <HardDrive className="w-5 h-5 text-primary" />
                <h3 className="font-semibold">Capacity</h3>
              </div>
              <div className="text-2xl font-mono">{fsStats.capacity.used}</div>
              <div className="text-sm text-muted-foreground">
                of {fsStats.capacity.total} ({fsStats.capacity.percent}%)
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <Zap className="w-5 h-5 text-accent" />
                <h3 className="font-semibold">Throughput</h3>
              </div>
              <div className="text-2xl font-mono">{fsStats.performance.throughput}</div>
              <div className="text-sm text-muted-foreground">
                R: {fsStats.performance.readIops} | W: {fsStats.performance.writeIops}
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <Copy className="w-5 h-5 text-secure" />
                <h3 className="font-semibold">Compression</h3>
              </div>
              <div className="text-2xl font-mono">{fsStats.compression.ratio}</div>
              <div className="text-sm text-muted-foreground">
                Saved: {fsStats.compression.saved}
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <Database className="w-5 h-5 text-warning" />
                <h3 className="font-semibold">Deduplication</h3>
              </div>
              <div className="text-2xl font-mono">{fsStats.dedup.ratio}</div>
              <div className="text-sm text-muted-foreground">
                Saved: {fsStats.dedup.saved}
              </div>
            </Card>
          </div>

          {/* System health */}
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4 flex items-center">
              <Activity className="w-5 h-5 mr-2" />
              System Health
            </h3>
            <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
              <div className="space-y-3">
                <h4 className="font-medium text-secure">Storage Pool Status</h4>
                <div className="space-y-2">
                  <div className="flex justify-between">
                    <span>Pool Health:</span>
                    <Badge className="glow-secure">ONLINE</Badge>
                  </div>
                  <div className="flex justify-between">
                    <span>Scrub Status:</span>
                    <Badge variant="secondary">{fsStats.scrubStatus}</Badge>
                  </div>
                  <div className="flex justify-between">
                    <span>Last Scrub:</span>
                    <span className="text-muted-foreground">3 days ago</span>
                  </div>
                </div>
              </div>
              
              <div className="space-y-3">
                <h4 className="font-medium text-primary">Data Protection</h4>
                <div className="space-y-2">
                  <div className="flex justify-between">
                    <span>Encryption:</span>
                    <Badge className="glow-primary">{fsStats.encryption}</Badge>
                  </div>
                  <div className="flex justify-between">
                    <span>Checksums:</span>
                    <Badge className="glow-secure">SHA-256</Badge>
                  </div>
                  <div className="flex justify-between">
                    <span>Redundancy:</span>
                    <Badge variant="secondary">RAID-Z2</Badge>
                  </div>
                </div>
              </div>
              
              <div className="space-y-3">
                <h4 className="font-medium text-accent">Backup Status</h4>
                <div className="space-y-2">
                  <div className="flex justify-between">
                    <span>Snapshots:</span>
                    <Badge variant="outline">{fsStats.snapshots}</Badge>
                  </div>
                  <div className="flex justify-between">
                    <span>Last Backup:</span>
                    <span className="text-muted-foreground">2 hours ago</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Retention:</span>
                    <span className="text-muted-foreground">30 days</span>
                  </div>
                </div>
              </div>
            </div>
          </Card>
        </div>
      )}

      {/* Volumes tab */}
      {activeTab === 'volumes' && (
        <div className="space-y-4">
          {volumes.map((volume) => (
            <Card key={volume.name} className="glass p-6">
              <div className="flex items-center justify-between mb-4">
                <div className="flex items-center space-x-3">
                  <Folder className="w-6 h-6 text-primary" />
                  <div>
                    <h3 className="text-lg font-semibold">{volume.name}</h3>
                    <p className="text-sm text-muted-foreground">{volume.path}</p>
                  </div>
                </div>
                <div className="flex space-x-2">
                  {volume.compression && (
                    <Badge variant="secondary">
                      <Copy className="w-3 h-3 mr-1" />
                      Compressed
                    </Badge>
                  )}
                  {volume.encryption && (
                    <Badge className="glow-secure">
                      <Lock className="w-3 h-3 mr-1" />
                      Encrypted
                    </Badge>
                  )}
                </div>
              </div>
              
              <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
                <div>
                  <div className="text-sm text-muted-foreground">Type</div>
                  <div className="font-medium">{volume.type}</div>
                </div>
                <div>
                  <div className="text-sm text-muted-foreground">Size</div>
                  <div className="font-medium font-mono">{volume.size}</div>
                </div>
                <div>
                  <div className="text-sm text-muted-foreground">Used</div>
                  <div className="font-medium font-mono">{volume.used}</div>
                </div>
                <div>
                  <div className="text-sm text-muted-foreground">Snapshots</div>
                  <div className="font-medium">{volume.snapshots}</div>
                </div>
              </div>
              
              <div className="mt-4 flex space-x-2">
                <Button size="sm" variant="outline">Snapshot</Button>
                <Button size="sm" variant="outline">Clone</Button>
                <Button size="sm" variant="outline">Properties</Button>
              </div>
            </Card>
          ))}
        </div>
      )}

      {/* Snapshots tab */}
      {activeTab === 'snapshots' && (
        <div className="space-y-4">
          <div className="flex items-center justify-between">
            <h3 className="text-lg font-semibold">Recent Snapshots</h3>
            <Button className="glow-primary">Create Snapshot</Button>
          </div>
          
          {recentSnapshots.map((snapshot) => (
            <Card key={snapshot.id} className="glass p-4">
              <div className="flex items-center justify-between">
                <div className="flex items-center space-x-3">
                  <Clock className="w-5 h-5 text-accent" />
                  <div>
                    <h4 className="font-medium font-mono">{snapshot.id}</h4>
                    <p className="text-sm text-muted-foreground">
                      Volume: {snapshot.volume} • Created: {snapshot.created}
                    </p>
                  </div>
                </div>
                <div className="flex items-center space-x-4">
                  <Badge variant="outline">{snapshot.size}</Badge>
                  <Button size="sm" variant="outline">Restore</Button>
                  <Button size="sm" variant="ghost">Clone</Button>
                </div>
              </div>
            </Card>
          ))}
        </div>
      )}

      {/* Security tab */}
      {activeTab === 'security' && (
        <div className="space-y-6">
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4 flex items-center">
              <Shield className="w-5 h-5 mr-2" />
              Encryption & Security
            </h3>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
              <div>
                <h4 className="font-medium mb-3 text-primary">Volume Encryption</h4>
                <div className="space-y-2 text-sm">
                  <div className="flex justify-between">
                    <span>Algorithm:</span>
                    <span className="font-mono">AES-256-XTS</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Key Source:</span>
                    <span>TPM 2.0 + Passphrase</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Key Rotation:</span>
                    <span>Every 90 days</span>
                  </div>
                </div>
              </div>
              
              <div>
                <h4 className="font-medium mb-3 text-secure">Data Integrity</h4>
                <div className="space-y-2 text-sm">
                  <div className="flex justify-between">
                    <span>Checksums:</span>
                    <span className="font-mono">SHA-256</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Verification:</span>
                    <span>Continuous</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Auto-Repair:</span>
                    <span className="text-secure">Enabled</span>
                  </div>
                </div>
              </div>
            </div>
          </Card>
        </div>
      )}
    </div>
  );
};