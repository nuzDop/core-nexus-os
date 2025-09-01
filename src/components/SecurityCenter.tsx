import React, { useState } from 'react';
import { Card } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Button } from '@/components/ui/button';
import { 
  Shield, 
  Lock, 
  Eye, 
  AlertTriangle, 
  CheckCircle, 
  XCircle,
  Key,
  Fingerprint,
  Scan,
  Activity,
  Globe,
  FileX
} from 'lucide-react';

export const SecurityCenter: React.FC = () => {
  const [activeTab, setActiveTab] = useState('overview');
  
  const securityStatus = {
    overall: 'SECURE',
    threats: 0,
    quarantined: 3,
    lastScan: '2 minutes ago',
    bootIntegrity: 'Verified',
    tpmStatus: 'Active'
  };

  const securityModules = [
    {
      name: 'Boot Integrity',
      status: 'Active',
      description: 'Secure Boot + TPM 2.0 Measured Boot',
      level: 'secure',
      details: 'UEFI Secure Boot verified, TPM PCR values validated'
    },
    {
      name: 'Code Signing',
      status: 'Active',
      description: 'All executables cryptographically verified',
      level: 'secure',
      details: 'Certificate chain validation, revocation checking'
    },
    {
      name: 'Sandbox Engine',
      status: 'Active',
      description: 'Application isolation and capability enforcement',
      level: 'secure',
      details: 'Per-process namespaces, capability-based access control'
    },
    {
      name: 'Memory Protection',
      status: 'Active',
      description: 'Hardware-enforced exploit mitigations',
      level: 'secure',
      details: 'ASLR, DEP, CET, CFI, Intel MPX, ARM Pointer Auth'
    },
    {
      name: 'Network Monitor',
      status: 'Active',
      description: 'Real-time traffic analysis and threat detection',
      level: 'warning',
      details: '12 suspicious connections blocked in last hour'
    },
    {
      name: 'File Integrity',
      status: 'Active',
      description: 'Continuous filesystem monitoring',
      level: 'secure',
      details: 'Real-time checksum validation, tamper detection'
    }
  ];

  const recentEvents = [
    {
      time: '14:32:15',
      type: 'info',
      event: 'Boot integrity verification completed',
      details: 'All PCR values match expected baseline'
    },
    {
      time: '14:28:42',
      type: 'warning',
      event: 'Suspicious network connection blocked',
      details: 'Connection to known C&C server 192.168.1.100:4444'
    },
    {
      time: '14:25:18',
      type: 'info',
      event: 'Application sandbox created',
      details: 'Process firefox.exe isolated with restricted capabilities'
    },
    {
      time: '14:22:33',
      type: 'secure',
      event: 'Code signature verified',
      details: 'Binary /usr/bin/chrome validated against trusted certificate'
    },
    {
      time: '14:19:07',
      type: 'info',
      event: 'Memory protection enabled',
      details: 'CET and CFI activated for process notepad++.exe'
    }
  ];

  const capabilities = [
    { name: 'Hardware Security Module', enabled: true, description: 'TPM 2.0' },
    { name: 'Secure Enclaves', enabled: true, description: 'Intel SGX / ARM TrustZone' },
    { name: 'Memory Encryption', enabled: true, description: 'Intel TME / AMD SME' },
    { name: 'Control Flow Integrity', enabled: true, description: 'Intel CET / ARM Pointer Auth' },
    { name: 'Kernel Guard', enabled: true, description: 'KPTI, SMEP, SMAP' },
    { name: 'Remote Attestation', enabled: true, description: 'TPM-based device identity' }
  ];

  const tabs = [
    { id: 'overview', name: 'Overview', icon: Shield },
    { id: 'threats', name: 'Threats', icon: AlertTriangle },
    { id: 'integrity', name: 'Integrity', icon: CheckCircle },
    { id: 'capabilities', name: 'Capabilities', icon: Key }
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
          {/* Security status overview */}
          <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <Shield className="w-5 h-5 text-secure" />
                <h3 className="font-semibold">Overall Status</h3>
              </div>
              <div className="text-2xl font-mono text-secure">{securityStatus.overall}</div>
              <div className="text-sm text-muted-foreground">
                All systems protected
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <AlertTriangle className="w-5 h-5 text-warning" />
                <h3 className="font-semibold">Active Threats</h3>
              </div>
              <div className="text-2xl font-mono">{securityStatus.threats}</div>
              <div className="text-sm text-muted-foreground">
                No active threats detected
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <FileX className="w-5 h-5 text-critical" />
                <h3 className="font-semibold">Quarantined</h3>
              </div>
              <div className="text-2xl font-mono">{securityStatus.quarantined}</div>
              <div className="text-sm text-muted-foreground">
                Files isolated safely
              </div>
            </Card>

            <Card className="glass p-4">
              <div className="flex items-center space-x-3 mb-2">
                <Scan className="w-5 h-5 text-primary" />
                <h3 className="font-semibold">Last Scan</h3>
              </div>
              <div className="text-lg font-mono">{securityStatus.lastScan}</div>
              <div className="text-sm text-muted-foreground">
                Real-time monitoring
              </div>
            </Card>
          </div>

          {/* Security modules */}
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4 flex items-center">
              <Activity className="w-5 h-5 mr-2" />
              Security Modules
            </h3>
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
              {securityModules.map((module) => (
                <Card key={module.name} className="glass p-4">
                  <div className="flex items-center justify-between mb-2">
                    <h4 className="font-medium">{module.name}</h4>
                    <Badge 
                      className={
                        module.level === 'secure' ? 'glow-secure' : 
                        module.level === 'warning' ? 'bg-warning text-warning-foreground' : 
                        'bg-critical text-critical-foreground'
                      }
                    >
                      {module.status}
                    </Badge>
                  </div>
                  <p className="text-sm text-muted-foreground mb-2">{module.description}</p>
                  <p className="text-xs text-muted-foreground">{module.details}</p>
                </Card>
              ))}
            </div>
          </Card>

          {/* Recent security events */}
          <Card className="glass p-6">
            <h3 className="text-xl font-semibold mb-4 flex items-center">
              <Eye className="w-5 h-5 mr-2" />
              Recent Security Events
            </h3>
            <div className="space-y-3">
              {recentEvents.map((event, index) => (
                <div key={index} className="flex items-start space-x-3 p-3 rounded-lg border border-border/50">
                  <div className="font-mono text-sm text-muted-foreground">{event.time}</div>
                  <div className="flex-1">
                    <div className="flex items-center space-x-2 mb-1">
                      {event.type === 'secure' && <CheckCircle className="w-4 h-4 text-secure" />}
                      {event.type === 'warning' && <AlertTriangle className="w-4 h-4 text-warning" />}
                      {event.type === 'info' && <Activity className="w-4 h-4 text-primary" />}
                      <span className="font-medium">{event.event}</span>
                    </div>
                    <p className="text-sm text-muted-foreground">{event.details}</p>
                  </div>
                </div>
              ))}
            </div>
          </Card>
        </div>
      )}

      {/* Capabilities tab */}
      {activeTab === 'capabilities' && (
        <div className="space-y-4">
          <h3 className="text-lg font-semibold">Hardware Security Capabilities</h3>
          
          {capabilities.map((capability) => (
            <Card key={capability.name} className="glass p-4">
              <div className="flex items-center justify-between">
                <div className="flex items-center space-x-3">
                  <Fingerprint className="w-5 h-5 text-primary" />
                  <div>
                    <h4 className="font-medium">{capability.name}</h4>
                    <p className="text-sm text-muted-foreground">{capability.description}</p>
                  </div>
                </div>
                <div className="flex items-center space-x-2">
                  {capability.enabled ? (
                    <CheckCircle className="w-5 h-5 text-secure" />
                  ) : (
                    <XCircle className="w-5 h-5 text-critical" />
                  )}
                  <Badge 
                    className={capability.enabled ? 'glow-secure' : 'bg-critical text-critical-foreground'}
                  >
                    {capability.enabled ? 'Enabled' : 'Disabled'}
                  </Badge>
                </div>
              </div>
            </Card>
          ))}
        </div>
      )}

      {/* Placeholder for other tabs */}
      {(activeTab === 'threats' || activeTab === 'integrity') && (
        <Card className="glass p-8 text-center">
          <h3 className="text-xl font-semibold mb-2">
            {activeTab === 'threats' ? 'Threat Monitoring' : 'System Integrity'}
          </h3>
          <p className="text-muted-foreground">
            {activeTab === 'threats' 
              ? 'Real-time threat detection and response dashboard'
              : 'File system and application integrity monitoring'
            }
          </p>
          <Badge className="mt-4 glow-secure">All Systems Secure</Badge>
        </Card>
      )}
    </div>
  );
};