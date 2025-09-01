import { useState, useEffect } from 'react';
import { supabase } from '@/integrations/supabase/client';
import { useAuth } from '@/hooks/useAuth';
import { useToast } from '@/hooks/use-toast';

export interface FileSystemItem {
  id: string;
  user_id: string;
  path: string;
  name: string;
  type: 'file' | 'directory';
  size?: number;
  content?: string;
  mime_type?: string;
  parent_id?: string;
  permissions: any;
  metadata: any;
  created_at: string;
  updated_at: string;
}

export const useFileSystem = () => {
  const { user } = useAuth();
  const { toast } = useToast();
  const [items, setItems] = useState<FileSystemItem[]>([]);
  const [currentPath, setCurrentPath] = useState('/home');
  const [loading, setLoading] = useState(false);

  const loadDirectory = async (path: string = '/home') => {
    if (!user) return;
    
    setLoading(true);
    try {
      const { data, error } = await supabase
        .from('filesystem')
        .select('*')
        .eq('user_id', user.id)
        .like('path', `${path}%`)
        .neq('path', path)
        .order('type', { ascending: false })
        .order('name');

      if (error) throw error;
      
      // Filter to show only direct children and type cast
      const directChildren = (data || []).filter(item => {
        const relativePath = item.path.replace(path, '').replace(/^\//, '');
        return relativePath && !relativePath.includes('/');
      }).map(item => ({
        ...item,
        type: item.type as 'file' | 'directory',
        permissions: item.permissions as any,
        metadata: item.metadata as any,
      }));
      
      setItems(directChildren);
      setCurrentPath(path);
    } catch (error: any) {
      toast({
        title: "Error Loading Directory",
        description: error.message,
        variant: "destructive",
      });
    } finally {
      setLoading(false);
    }
  };

  const createFile = async (name: string, content: string = '', mimeType: string = 'text/plain') => {
    if (!user) return null;
    
    const path = currentPath === '/' ? `/${name}` : `${currentPath}/${name}`;
    
    try {
      const { data, error } = await supabase
        .from('filesystem')
        .insert({
          user_id: user.id,
          path,
          name,
          type: 'file',
          content,
          mime_type: mimeType,
          size: content.length,
          permissions: { read: true, write: true, execute: false },
          metadata: {}
        })
        .select()
        .single();

      if (error) throw error;

      toast({
        title: "File Created",
        description: `${name} created successfully`,
      });
      
      await loadDirectory(currentPath);
      return data;
    } catch (error: any) {
      toast({
        title: "Error Creating File",
        description: error.message,
        variant: "destructive",
      });
      return null;
    }
  };

  const createDirectory = async (name: string) => {
    if (!user) return null;
    
    const path = currentPath === '/' ? `/${name}` : `${currentPath}/${name}`;
    
    try {
      const { data, error } = await supabase
        .from('filesystem')
        .insert({
          user_id: user.id,
          path,
          name,
          type: 'directory',
          permissions: { read: true, write: true, execute: true },
          metadata: {}
        })
        .select()
        .single();

      if (error) throw error;

      toast({
        title: "Directory Created",
        description: `${name} created successfully`,
      });
      
      await loadDirectory(currentPath);
      return data;
    } catch (error: any) {
      toast({
        title: "Error Creating Directory",
        description: error.message,
        variant: "destructive",
      });
      return null;
    }
  };

  const deleteItem = async (itemId: string, itemName: string) => {
    if (!user) return;
    
    try {
      const { error } = await supabase
        .from('filesystem')
        .delete()
        .eq('id', itemId)
        .eq('user_id', user.id);

      if (error) throw error;

      toast({
        title: "Item Deleted",
        description: `${itemName} deleted successfully`,
      });
      
      await loadDirectory(currentPath);
    } catch (error: any) {
      toast({
        title: "Error Deleting Item",
        description: error.message,
        variant: "destructive",
      });
    }
  };

  const updateFile = async (itemId: string, content: string) => {
    if (!user) return;
    
    try {
      const { error } = await supabase
        .from('filesystem')
        .update({
          content,
          size: content.length,
          updated_at: new Date().toISOString()
        })
        .eq('id', itemId)
        .eq('user_id', user.id);

      if (error) throw error;

      toast({
        title: "File Updated",
        description: "File saved successfully",
      });
      
      await loadDirectory(currentPath);
    } catch (error: any) {
      toast({
        title: "Error Updating File",
        description: error.message,
        variant: "destructive",
      });
    }
  };

  const navigateTo = (path: string) => {
    setCurrentPath(path);
    loadDirectory(path);
  };

  useEffect(() => {
    if (user) {
      loadDirectory(currentPath);
    }
  }, [user]);

  return {
    items,
    currentPath,
    loading,
    loadDirectory,
    createFile,
    createDirectory,
    deleteItem,
    updateFile,
    navigateTo,
  };
};