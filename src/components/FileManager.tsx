import React, { useState } from 'react';
import { Card } from '@/components/ui/card';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Badge } from '@/components/ui/badge';
import { 
  Folder, 
  File, 
  Plus, 
  FolderPlus, 
  FilePlus,
  Trash2,
  Edit3,
  Download,
  Upload,
  Home,
  ArrowLeft,
  Search,
  Grid,
  List
} from 'lucide-react';
import { useFileSystem } from '@/hooks/useFileSystem';
import { Dialog, DialogContent, DialogHeader, DialogTitle, DialogTrigger } from '@/components/ui/dialog';
import { Textarea } from '@/components/ui/textarea';

export const FileManager: React.FC = () => {
  const {
    items,
    currentPath,
    loading,
    createFile,
    createDirectory,
    deleteItem,
    updateFile,
    navigateTo,
  } = useFileSystem();

  const [newItemName, setNewItemName] = useState('');
  const [newItemType, setNewItemType] = useState<'file' | 'directory'>('file');
  const [editingFile, setEditingFile] = useState<any>(null);
  const [fileContent, setFileContent] = useState('');
  const [searchTerm, setSearchTerm] = useState('');
  const [viewMode, setViewMode] = useState<'grid' | 'list'>('grid');

  const filteredItems = items.filter(item =>
    item.name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  const handleCreateItem = async () => {
    if (!newItemName.trim()) return;
    
    if (newItemType === 'file') {
      await createFile(newItemName);
    } else {
      await createDirectory(newItemName);
    }
    
    setNewItemName('');
  };

  const handleEditFile = (item: any) => {
    setEditingFile(item);
    setFileContent(item.content || '');
  };

  const handleSaveFile = async () => {
    if (editingFile) {
      await updateFile(editingFile.id, fileContent);
      setEditingFile(null);
      setFileContent('');
    }
  };

  const navigateUp = () => {
    const parentPath = currentPath.split('/').slice(0, -1).join('/') || '/';
    navigateTo(parentPath === '' ? '/home' : parentPath);
  };

  const formatFileSize = (bytes?: number) => {
    if (!bytes) return '0 B';
    const sizes = ['B', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(1024));
    return `${Math.round(bytes / Math.pow(1024, i) * 100) / 100} ${sizes[i]}`;
  };

  const formatDate = (dateString: string) => {
    return new Date(dateString).toLocaleDateString() + ' ' + 
           new Date(dateString).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  };

  return (
    <div className="space-y-4">
      {/* Toolbar */}
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          <Button
            variant="ghost"
            size="sm"
            onClick={() => navigateTo('/home')}
            disabled={currentPath === '/home'}
          >
            <Home className="w-4 h-4" />
          </Button>
          <Button
            variant="ghost"
            size="sm"
            onClick={navigateUp}
            disabled={currentPath === '/home'}
          >
            <ArrowLeft className="w-4 h-4" />
          </Button>
          <div className="font-mono text-sm bg-secondary px-3 py-1 rounded">
            {currentPath}
          </div>
        </div>
        
        <div className="flex items-center space-x-2">
          <Button
            variant={viewMode === 'grid' ? 'default' : 'ghost'}
            size="sm"
            onClick={() => setViewMode('grid')}
          >
            <Grid className="w-4 h-4" />
          </Button>
          <Button
            variant={viewMode === 'list' ? 'default' : 'ghost'}
            size="sm"
            onClick={() => setViewMode('list')}
          >
            <List className="w-4 h-4" />
          </Button>
        </div>
      </div>

      {/* Search and Actions */}
      <div className="flex items-center space-x-4">
        <div className="flex-1 relative">
          <Search className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
          <Input
            placeholder="Search files and folders..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="pl-10"
          />
        </div>
        
        <div className="flex items-center space-x-2">
          <Button
            variant="outline"
            size="sm"
            onClick={() => setNewItemType('directory')}
          >
            <FolderPlus className="w-4 h-4 mr-1" />
            New Folder
          </Button>
          <Button
            variant="outline"
            size="sm"
            onClick={() => setNewItemType('file')}
          >
            <FilePlus className="w-4 h-4 mr-1" />
            New File
          </Button>
        </div>
      </div>

      {/* New Item Creation */}
      {(newItemType === 'file' || newItemType === 'directory') && (
        <Card className="glass p-4">
          <div className="flex items-center space-x-2">
            <Input
              placeholder={`Enter ${newItemType} name...`}
              value={newItemName}
              onChange={(e) => setNewItemName(e.target.value)}
              onKeyDown={(e) => e.key === 'Enter' && handleCreateItem()}
              className="flex-1"
            />
            <Button onClick={handleCreateItem} disabled={!newItemName.trim()}>
              Create
            </Button>
            <Button
              variant="ghost"
              onClick={() => {
                setNewItemName('');
                setNewItemType('file');
              }}
            >
              Cancel
            </Button>
          </div>
        </Card>
      )}

      {/* File Grid/List */}
      {loading ? (
        <div className="text-center py-8">
          <div className="animate-spin w-8 h-8 border-2 border-primary border-t-transparent rounded-full mx-auto mb-2" />
          <p className="text-muted-foreground">Loading files...</p>
        </div>
      ) : (
        <div className={
          viewMode === 'grid' 
            ? "grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-6 gap-4"
            : "space-y-2"
        }>
          {filteredItems.map((item) => (
            <Card
              key={item.id}
              className={`glass p-4 cursor-pointer hover:glow-primary transition-all ${
                viewMode === 'list' ? 'flex items-center justify-between' : ''
              }`}
              onClick={() => {
                if (item.type === 'directory') {
                  navigateTo(item.path);
                }
              }}
            >
              <div className={`flex items-center space-x-3 ${viewMode === 'list' ? 'flex-1' : 'flex-col text-center'}`}>
                {item.type === 'directory' ? (
                  <Folder className={`text-accent ${viewMode === 'grid' ? 'w-8 h-8' : 'w-5 h-5'}`} />
                ) : (
                  <File className={`text-primary ${viewMode === 'grid' ? 'w-8 h-8' : 'w-5 h-5'}`} />
                )}
                
                <div className={`${viewMode === 'list' ? 'flex-1' : 'w-full'}`}>
                  <div className={`font-medium text-sm ${viewMode === 'grid' ? 'mt-2' : ''} truncate`}>
                    {item.name}
                  </div>
                  
                  {viewMode === 'list' && (
                    <div className="flex items-center space-x-4 text-xs text-muted-foreground">
                      <span>{item.type}</span>
                      {item.size !== undefined && <span>{formatFileSize(item.size)}</span>}
                      <span>{formatDate(item.updated_at)}</span>
                    </div>
                  )}
                  
                  {viewMode === 'grid' && (
                    <div className="text-xs text-muted-foreground mt-1">
                      {item.size !== undefined && <div>{formatFileSize(item.size)}</div>}
                    </div>
                  )}
                </div>
              </div>

              <div className={`flex items-center space-x-1 ${viewMode === 'grid' ? 'mt-2 justify-center' : ''}`}>
                {item.type === 'file' && (
                  <Dialog>
                    <DialogTrigger asChild>
                      <Button
                        variant="ghost"
                        size="sm"
                        onClick={(e) => {
                          e.stopPropagation();
                          handleEditFile(item);
                        }}
                      >
                        <Edit3 className="w-3 h-3" />
                      </Button>
                    </DialogTrigger>
                    <DialogContent className="glass max-w-2xl">
                      <DialogHeader>
                        <DialogTitle>Edit {item.name}</DialogTitle>
                      </DialogHeader>
                      <div className="space-y-4">
                        <Textarea
                          value={fileContent}
                          onChange={(e) => setFileContent(e.target.value)}
                          className="min-h-[300px] font-mono"
                          placeholder="File content..."
                        />
                        <div className="flex justify-end space-x-2">
                          <Button variant="ghost" onClick={() => setEditingFile(null)}>
                            Cancel
                          </Button>
                          <Button onClick={handleSaveFile}>
                            Save File
                          </Button>
                        </div>
                      </div>
                    </DialogContent>
                  </Dialog>
                )}
                
                <Button
                  variant="ghost"
                  size="sm"
                  onClick={(e) => {
                    e.stopPropagation();
                    deleteItem(item.id, item.name);
                  }}
                  className="text-destructive hover:text-destructive"
                >
                  <Trash2 className="w-3 h-3" />
                </Button>
              </div>
            </Card>
          ))}
          
          {filteredItems.length === 0 && !loading && (
            <div className="col-span-full text-center py-8 text-muted-foreground">
              {searchTerm ? `No items found matching "${searchTerm}"` : 'This directory is empty'}
            </div>
          )}
        </div>
      )}
    </div>
  );
};