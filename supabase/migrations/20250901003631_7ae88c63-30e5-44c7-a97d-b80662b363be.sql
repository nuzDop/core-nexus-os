-- LimitlessOS Core Database Schema

-- Enable necessary extensions
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- User profiles table
CREATE TABLE public.profiles (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE UNIQUE,
  username TEXT NOT NULL UNIQUE,
  display_name TEXT,
  avatar_url TEXT,
  security_clearance TEXT DEFAULT 'user' CHECK (security_clearance IN ('admin', 'operator', 'user')),
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  updated_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  last_login TIMESTAMP WITH TIME ZONE,
  preferences JSONB DEFAULT '{}'::jsonb
);

-- Virtual file system
CREATE TABLE public.filesystem (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE,
  path TEXT NOT NULL,
  name TEXT NOT NULL,
  type TEXT NOT NULL CHECK (type IN ('file', 'directory')),
  size BIGINT DEFAULT 0,
  content TEXT,
  mime_type TEXT,
  parent_id UUID REFERENCES public.filesystem(id) ON DELETE CASCADE,
  permissions JSONB DEFAULT '{"read": true, "write": true, "execute": false}'::jsonb,
  metadata JSONB DEFAULT '{}'::jsonb,
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  updated_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  UNIQUE(user_id, path)
);

-- System processes
CREATE TABLE public.processes (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE,
  pid INTEGER NOT NULL,
  name TEXT NOT NULL,
  command TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'running' CHECK (status IN ('running', 'paused', 'stopped', 'zombie')),
  cpu_usage DECIMAL(5,2) DEFAULT 0,
  memory_usage BIGINT DEFAULT 0,
  priority INTEGER DEFAULT 0,
  parent_pid INTEGER,
  started_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  metadata JSONB DEFAULT '{}'::jsonb
);

-- Installed applications
CREATE TABLE public.applications (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  name TEXT NOT NULL,
  display_name TEXT NOT NULL,
  version TEXT NOT NULL,
  description TEXT,
  category TEXT NOT NULL,
  executable_path TEXT NOT NULL,
  icon_url TEXT,
  size BIGINT DEFAULT 0,
  permissions JSONB DEFAULT '{}'::jsonb,
  metadata JSONB DEFAULT '{}'::jsonb,
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  is_system_app BOOLEAN DEFAULT false
);

-- User installed applications
CREATE TABLE public.user_applications (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE,
  application_id UUID NOT NULL REFERENCES public.applications(id) ON DELETE CASCADE,
  installed_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  settings JSONB DEFAULT '{}'::jsonb,
  UNIQUE(user_id, application_id)
);

-- System logs
CREATE TABLE public.system_logs (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID REFERENCES auth.users(id) ON DELETE SET NULL,
  level TEXT NOT NULL CHECK (level IN ('debug', 'info', 'warning', 'error', 'critical')),
  category TEXT NOT NULL,
  message TEXT NOT NULL,
  metadata JSONB DEFAULT '{}'::jsonb,
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now()
);

-- Terminal sessions
CREATE TABLE public.terminal_sessions (
  id UUID NOT NULL DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE,
  session_name TEXT NOT NULL,
  current_directory TEXT DEFAULT '/home',
  history JSONB DEFAULT '[]'::jsonb,
  environment_vars JSONB DEFAULT '{}'::jsonb,
  created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
  updated_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now()
);

-- Enable Row Level Security
ALTER TABLE public.profiles ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.filesystem ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.processes ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.applications ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.user_applications ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.system_logs ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.terminal_sessions ENABLE ROW LEVEL SECURITY;

-- RLS Policies for profiles
CREATE POLICY "Users can view their own profile" ON public.profiles
  FOR SELECT USING (auth.uid() = user_id);

CREATE POLICY "Users can update their own profile" ON public.profiles
  FOR UPDATE USING (auth.uid() = user_id);

CREATE POLICY "Users can insert their own profile" ON public.profiles
  FOR INSERT WITH CHECK (auth.uid() = user_id);

-- RLS Policies for filesystem
CREATE POLICY "Users can access their own files" ON public.filesystem
  FOR ALL USING (auth.uid() = user_id);

-- RLS Policies for processes
CREATE POLICY "Users can view their own processes" ON public.processes
  FOR ALL USING (auth.uid() = user_id);

-- RLS Policies for applications (public read, admin write)
CREATE POLICY "Anyone can view applications" ON public.applications
  FOR SELECT USING (true);

-- RLS Policies for user applications
CREATE POLICY "Users can manage their installed apps" ON public.user_applications
  FOR ALL USING (auth.uid() = user_id);

-- RLS Policies for system logs
CREATE POLICY "Users can view their own logs" ON public.system_logs
  FOR SELECT USING (auth.uid() = user_id OR user_id IS NULL);

CREATE POLICY "Authenticated users can insert logs" ON public.system_logs
  FOR INSERT WITH CHECK (auth.uid() = user_id OR user_id IS NULL);

-- RLS Policies for terminal sessions
CREATE POLICY "Users can manage their terminal sessions" ON public.terminal_sessions
  FOR ALL USING (auth.uid() = user_id);

-- Create indexes for performance
CREATE INDEX idx_filesystem_user_path ON public.filesystem(user_id, path);
CREATE INDEX idx_filesystem_parent ON public.filesystem(parent_id);
CREATE INDEX idx_processes_user_status ON public.processes(user_id, status);
CREATE INDEX idx_system_logs_user_time ON public.system_logs(user_id, created_at DESC);
CREATE INDEX idx_terminal_sessions_user ON public.terminal_sessions(user_id);

-- Function to handle new user registration
CREATE OR REPLACE FUNCTION public.handle_new_user()
RETURNS TRIGGER AS $$
BEGIN
  -- Create user profile
  INSERT INTO public.profiles (user_id, username, display_name)
  VALUES (
    NEW.id,
    COALESCE(NEW.raw_user_meta_data->>'username', 'user_' || substring(NEW.id::text, 1, 8)),
    COALESCE(NEW.raw_user_meta_data->>'display_name', 'LimitlessOS User')
  );
  
  -- Create home directory
  INSERT INTO public.filesystem (user_id, path, name, type, permissions)
  VALUES (
    NEW.id,
    '/home',
    'home',
    'directory',
    '{"read": true, "write": true, "execute": true}'::jsonb
  );
  
  -- Create default terminal session
  INSERT INTO public.terminal_sessions (user_id, session_name)
  VALUES (NEW.id, 'default');
  
  -- Log user creation
  INSERT INTO public.system_logs (user_id, level, category, message)
  VALUES (NEW.id, 'info', 'auth', 'New user account created');
  
  RETURN NEW;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

-- Trigger for new user creation
CREATE OR REPLACE TRIGGER on_auth_user_created
  AFTER INSERT ON auth.users
  FOR EACH ROW EXECUTE FUNCTION public.handle_new_user();

-- Function to update timestamps
CREATE OR REPLACE FUNCTION public.update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
  NEW.updated_at = now();
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Add triggers for updated_at columns
CREATE TRIGGER update_profiles_updated_at BEFORE UPDATE ON public.profiles
  FOR EACH ROW EXECUTE FUNCTION public.update_updated_at_column();

CREATE TRIGGER update_filesystem_updated_at BEFORE UPDATE ON public.filesystem
  FOR EACH ROW EXECUTE FUNCTION public.update_updated_at_column();

CREATE TRIGGER update_terminal_sessions_updated_at BEFORE UPDATE ON public.terminal_sessions
  FOR EACH ROW EXECUTE FUNCTION public.update_updated_at_column();

-- Insert default system applications
INSERT INTO public.applications (name, display_name, version, description, category, executable_path, is_system_app) VALUES
('terminal', 'Terminal', '1.0.0', 'System terminal emulator', 'System', '/system/apps/terminal', true),
('file-manager', 'File Manager', '1.0.0', 'File system browser and manager', 'System', '/system/apps/file-manager', true),
('process-monitor', 'Process Monitor', '1.0.0', 'System process and performance monitor', 'System', '/system/apps/process-monitor', true),
('security-center', 'Security Center', '1.0.0', 'System security management', 'System', '/system/apps/security-center', true),
('app-store', 'App Store', '1.0.0', 'Application marketplace', 'System', '/system/apps/app-store', true),
('text-editor', 'Text Editor', '1.0.0', 'Simple text editor', 'Productivity', '/system/apps/text-editor', true),
('calculator', 'Calculator', '1.0.0', 'Basic calculator', 'Utilities', '/system/apps/calculator', true),
('system-monitor', 'System Monitor', '1.0.0', 'Real-time system monitoring', 'System', '/system/apps/system-monitor', true);