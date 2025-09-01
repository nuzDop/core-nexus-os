import { useState, useEffect } from 'react';
import { supabase } from '@/integrations/supabase/client';
import { useAuth } from '@/hooks/useAuth';
import { useToast } from '@/hooks/use-toast';

interface TerminalCommand {
  command: string;
  output: string;
  timestamp: string; // Changed to string for JSON serialization
  exitCode: number;
}

interface SerializedTerminalCommand {
  command: string;
  output: string;
  timestamp: string;
  exitCode: number;
}

export const useTerminal = () => {
  const { user } = useAuth();
  const { toast } = useToast();
  const [currentDirectory, setCurrentDirectory] = useState('/home');
  const [commandHistory, setCommandHistory] = useState<TerminalCommand[]>([]);
  const [environmentVars, setEnvironmentVars] = useState<Record<string, string>>({
    USER: 'limitless_user',
    HOME: '/home',
    PATH: '/system/bin:/usr/bin:/bin',
    SHELL: '/system/shell/limitless',
    TERM: 'limitless-terminal',
    OS: 'LimitlessOS'
  });

  // Simulated file system commands
  const executeCommand = async (command: string): Promise<{ output: string; exitCode: number }> => {
    const args = command.trim().split(/\s+/);
    const cmd = args[0];
    
    switch (cmd) {
      case 'ls':
      case 'dir':
        return await handleListFiles(args.slice(1));
      
      case 'pwd':
        return { output: currentDirectory, exitCode: 0 };
      
      case 'cd':
        return handleChangeDirectory(args[1] || '/home');
      
      case 'mkdir':
        return await handleMakeDirectory(args[1]);
      
      case 'touch':
        return await handleCreateFile(args[1]);
      
      case 'rm':
        return await handleRemoveFile(args[1]);
      
      case 'cat':
        return await handleReadFile(args[1]);
      
      case 'echo':
        return { output: args.slice(1).join(' '), exitCode: 0 };
      
      case 'env':
        return { output: Object.entries(environmentVars).map(([k, v]) => `${k}=${v}`).join('\n'), exitCode: 0 };
      
      case 'whoami':
        return { output: environmentVars.USER, exitCode: 0 };
      
      case 'uname':
        const flags = args[1];
        if (flags === '-a') {
          return { output: 'LimitlessOS polycore 1.0.0 x86_64 GNU/Linux', exitCode: 0 };
        }
        return { output: 'LimitlessOS', exitCode: 0 };
      
      case 'ps':
        return await handleProcessList();
      
      case 'top':
        return { output: 'PID\tCOMMAND\t\tCPU%\tMEM%\n1234\tlimitless-kernel\t2.1\t4.2\n5678\tlimitless-ui\t\t1.8\t8.7', exitCode: 0 };
      
      case 'df':
        return { output: 'Filesystem\t\tSize\tUsed\tAvail\tUse%\n/dev/ifs0\t\t8.0T\t2.4T\t5.6T\t30%', exitCode: 0 };
      
      case 'free':
        return { output: 'total\t\tused\t\tfree\t\tshared\tbuff/cache\tavailable\nMem:\t32768\t\t4096\t\t28672\t\t0\t\t0\t\t28672', exitCode: 0 };
      
      case 'date':
        return { output: new Date().toString(), exitCode: 0 };
      
      case 'uptime':
        return { output: '14:32:15 up 127 days, 14:32, 1 user, load average: 0.12, 0.08, 0.03', exitCode: 0 };
      
      case 'help':
      case '--help':
        return {
          output: `LimitlessOS Terminal Commands:
File System: ls, pwd, cd, mkdir, touch, rm, cat
System Info: uname, whoami, env, ps, top, df, free, date, uptime
Utilities: echo, help, clear, history
Network: ping, wget, curl (simulated)
Security: sudo, chmod, chown (permission system)`,
          exitCode: 0
        };
      
      case 'clear':
        setCommandHistory([]);
        return { output: '', exitCode: 0 };
      
      case 'history':
        return {
          output: commandHistory.map((cmd, i) => `${i + 1}  ${cmd.command}`).join('\n'),
          exitCode: 0
        };
      
      case 'ping':
        const host = args[1] || 'localhost';
        return {
          output: `PING ${host}: 64 bytes from ${host}: icmp_seq=1 ttl=64 time=0.123ms
PING ${host}: 64 bytes from ${host}: icmp_seq=2 ttl=64 time=0.089ms
--- ${host} ping statistics ---
2 packets transmitted, 2 received, 0% packet loss`,
          exitCode: 0
        };
      
      case 'sudo':
        if (args.length > 1) {
          return { output: `[sudo] Executing with elevated privileges: ${args.slice(1).join(' ')}`, exitCode: 0 };
        }
        return { output: 'usage: sudo <command>', exitCode: 1 };
      
      default:
        return { output: `limitless: command not found: ${cmd}`, exitCode: 127 };
    }
  };

  const handleListFiles = async (flags: string[]): Promise<{ output: string; exitCode: number }> => {
    if (!user) return { output: 'Authentication required', exitCode: 1 };
    
    try {
      const { data, error } = await supabase
        .from('filesystem')
        .select('*')
        .eq('user_id', user.id)
        .like('path', `${currentDirectory}%`)
        .neq('path', currentDirectory);

      if (error) throw error;

      const directChildren = data.filter(item => {
        const relativePath = item.path.replace(currentDirectory, '').replace(/^\//, '');
        return relativePath && !relativePath.includes('/');
      });

      if (flags.includes('-l')) {
        const output = directChildren.map(item => {
          const permissions = item.type === 'directory' ? 'drwxrwxrwx' : '-rw-rw-rw-';
          const size = item.size || 0;
          const date = new Date(item.created_at).toLocaleDateString();
          return `${permissions} 1 user user ${size.toString().padStart(8)} ${date} ${item.name}`;
        }).join('\n');
        
        return { output, exitCode: 0 };
      } else {
        const output = directChildren
          .sort((a, b) => {
            if (a.type !== b.type) return a.type === 'directory' ? -1 : 1;
            return a.name.localeCompare(b.name);
          })
          .map(item => item.type === 'directory' ? `${item.name}/` : item.name)
          .join('  ');
        
        return { output, exitCode: 0 };
      }
    } catch (error: any) {
      return { output: `ls: error reading directory: ${error.message}`, exitCode: 1 };
    }
  };

  const handleChangeDirectory = (path: string): { output: string; exitCode: number } => {
    let newPath = path;
    
    if (path === '..') {
      newPath = currentDirectory.split('/').slice(0, -1).join('/') || '/home';
    } else if (path === '~' || path === '') {
      newPath = '/home';
    } else if (!path.startsWith('/')) {
      newPath = `${currentDirectory}/${path}`;
    }
    
    // Normalize path
    newPath = newPath.replace(/\/+/g, '/').replace(/\/$/, '') || '/';
    if (newPath === '') newPath = '/home';
    
    setCurrentDirectory(newPath);
    return { output: '', exitCode: 0 };
  };

  const handleMakeDirectory = async (name: string): Promise<{ output: string; exitCode: number }> => {
    if (!user || !name) return { output: 'mkdir: missing operand', exitCode: 1 };
    
    const path = `${currentDirectory}/${name}`;
    
    try {
      const { error } = await supabase
        .from('filesystem')
        .insert({
          user_id: user.id,
          path,
          name,
          type: 'directory',
          permissions: { read: true, write: true, execute: true },
          metadata: {}
        });

      if (error) throw error;
      return { output: '', exitCode: 0 };
    } catch (error: any) {
      return { output: `mkdir: cannot create directory '${name}': ${error.message}`, exitCode: 1 };
    }
  };

  const handleCreateFile = async (name: string): Promise<{ output: string; exitCode: number }> => {
    if (!user || !name) return { output: 'touch: missing file operand', exitCode: 1 };
    
    const path = `${currentDirectory}/${name}`;
    
    try {
      const { error } = await supabase
        .from('filesystem')
        .insert({
          user_id: user.id,
          path,
          name,
          type: 'file',
          content: '',
          size: 0,
          permissions: { read: true, write: true, execute: false },
          metadata: {}
        });

      if (error) throw error;
      return { output: '', exitCode: 0 };
    } catch (error: any) {
      return { output: `touch: cannot touch '${name}': ${error.message}`, exitCode: 1 };
    }
  };

  const handleRemoveFile = async (name: string): Promise<{ output: string; exitCode: number }> => {
    if (!user || !name) return { output: 'rm: missing operand', exitCode: 1 };
    
    const path = `${currentDirectory}/${name}`;
    
    try {
      const { error } = await supabase
        .from('filesystem')
        .delete()
        .eq('user_id', user.id)
        .eq('path', path);

      if (error) throw error;
      return { output: '', exitCode: 0 };
    } catch (error: any) {
      return { output: `rm: cannot remove '${name}': ${error.message}`, exitCode: 1 };
    }
  };

  const handleReadFile = async (name: string): Promise<{ output: string; exitCode: number }> => {
    if (!user || !name) return { output: 'cat: missing file operand', exitCode: 1 };
    
    const path = `${currentDirectory}/${name}`;
    
    try {
      const { data, error } = await supabase
        .from('filesystem')
        .select('content')
        .eq('user_id', user.id)
        .eq('path', path)
        .eq('type', 'file')
        .maybeSingle();

      if (error) throw error;
      if (!data) return { output: `cat: ${name}: No such file or directory`, exitCode: 1 };
      
      return { output: data.content || '', exitCode: 0 };
    } catch (error: any) {
      return { output: `cat: ${name}: ${error.message}`, exitCode: 1 };
    }
  };

  const handleProcessList = async (): Promise<{ output: string; exitCode: number }> => {
    if (!user) return { output: 'Authentication required', exitCode: 1 };
    
    try {
      const { data, error } = await supabase
        .from('processes')
        .select('*')
        .eq('user_id', user.id)
        .order('pid');

      if (error) throw error;

      const header = 'PID\tCOMMAND\t\t\tSTATUS\t\tCPU%\tMEM';
      const processes = data.map(proc => 
        `${proc.pid}\t${proc.command.padEnd(15)}\t${proc.status.padEnd(8)}\t${proc.cpu_usage}%\t${Math.round((proc.memory_usage || 0) / 1024 / 1024)}M`
      ).join('\n');
      
      return { output: header + '\n' + processes, exitCode: 0 };
    } catch (error: any) {
      return { output: `ps: error: ${error.message}`, exitCode: 1 };
    }
  };

  const runCommand = async (command: string) => {
    const result = await executeCommand(command);
    const newCommand: TerminalCommand = {
      command,
      output: result.output,
      timestamp: new Date().toISOString(),
      exitCode: result.exitCode
    };
    
    setCommandHistory(prev => [...prev, newCommand]);
    
    // Save command to database
    if (user && command !== 'clear') {
      try {
        const serializedHistory = [...commandHistory, newCommand].slice(-100).map(cmd => ({
          command: cmd.command,
          output: cmd.output,
          timestamp: typeof cmd.timestamp === 'string' ? cmd.timestamp : cmd.timestamp,
          exitCode: cmd.exitCode
        }));
        
        await supabase
          .from('terminal_sessions')
          .update({
            history: serializedHistory,
            current_directory: currentDirectory,
            updated_at: new Date().toISOString()
          })
          .eq('user_id', user.id)
          .eq('session_name', 'default');
      } catch (error) {
        console.error('Error saving command history:', error);
      }
    }
    
    return result;
  };

  // Load terminal session on mount
  useEffect(() => {
    if (user) {
      const loadSession = async () => {
        try {
          const { data } = await supabase
            .from('terminal_sessions')
            .select('*')
            .eq('user_id', user.id)
            .eq('session_name', 'default')
            .maybeSingle();

          if (data) {
            setCurrentDirectory(data.current_directory || '/home');
            
            // Parse history with proper type handling
            const historyData = data.history as any;
            if (Array.isArray(historyData)) {
              const parsedHistory: TerminalCommand[] = historyData.map((cmd: any) => ({
                command: cmd.command || '',
                output: cmd.output || '',
                timestamp: cmd.timestamp || new Date().toISOString(),
                exitCode: cmd.exitCode || 0
              }));
              setCommandHistory(parsedHistory);
            }
            
            // Parse environment vars
            const envVars = data.environment_vars as any;
            if (envVars && typeof envVars === 'object') {
              setEnvironmentVars(prev => ({ ...prev, ...envVars }));
            }
          }
        } catch (error) {
          console.error('Error loading terminal session:', error);
        }
      };
      
      loadSession();
    }
  }, [user]);

  return {
    currentDirectory,
    commandHistory,
    environmentVars,
    runCommand,
    setCurrentDirectory,
  };
};