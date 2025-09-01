import React, { useState } from 'react';
import { Navigate, Link } from 'react-router-dom';
import { Button } from '@/components/ui/button';
import { Card } from '@/components/ui/card';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs';
import { Shield, Lock, User, Mail } from 'lucide-react';
import { useAuth } from '@/hooks/useAuth';
import heroImage from '@/assets/limitless-os-hero.jpg';

const Auth: React.FC = () => {
  const { signIn, signUp, user, loading } = useAuth();
  const [isLoading, setIsLoading] = useState(false);
  
  const [signInForm, setSignInForm] = useState({
    email: '',
    password: ''
  });
  
  const [signUpForm, setSignUpForm] = useState({
    email: '',
    password: '',
    confirmPassword: '',
    username: ''
  });

  // Redirect if already authenticated
  if (user && !loading) {
    return <Navigate to="/" replace />;
  }

  const handleSignIn = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);
    
    await signIn(signInForm.email, signInForm.password);
    
    setIsLoading(false);
  };

  const handleSignUp = async (e: React.FormEvent) => {
    e.preventDefault();
    
    if (signUpForm.password !== signUpForm.confirmPassword) {
      return;
    }
    
    setIsLoading(true);
    
    await signUp(signUpForm.email, signUpForm.password, signUpForm.username);
    
    setIsLoading(false);
  };

  if (loading) {
    return (
      <div className="min-h-screen bg-background flex items-center justify-center">
        <div className="text-center space-y-4">
          <Shield className="w-12 h-12 text-primary mx-auto animate-pulse" />
          <p className="text-muted-foreground">Initializing LimitlessOS...</p>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-background relative overflow-hidden">
      {/* Background with hero image */}
      <div className="absolute inset-0">
        <div className="absolute inset-0 bg-background/90 z-10" />
        <img 
          src={heroImage} 
          alt="LimitlessOS Security Interface" 
          className="w-full h-full object-cover opacity-30"
        />
        <div className="absolute top-1/4 left-1/4 w-96 h-96 bg-primary/10 rounded-full blur-3xl z-0" />
        <div className="absolute bottom-1/4 right-1/4 w-96 h-96 bg-secure/10 rounded-full blur-3xl z-0" />
      </div>

      <div className="relative z-20 min-h-screen flex items-center justify-center px-6">
        <div className="w-full max-w-md space-y-6">
          {/* Header */}
          <div className="text-center space-y-4">
            <div className="flex items-center justify-center space-x-3">
              <Shield className="w-10 h-10 text-primary glow-primary" />
              <h1 className="text-3xl font-bold text-gradient">LimitlessOS</h1>
            </div>
            <p className="text-muted-foreground">
              Secure Access to Military-Grade Operating System
            </p>
          </div>

          {/* Auth Form */}
          <Card className="glass p-6">
            <Tabs defaultValue="signin" className="space-y-4">
              <TabsList className="grid w-full grid-cols-2 glass">
                <TabsTrigger value="signin" className="flex items-center space-x-2">
                  <Lock className="w-4 h-4" />
                  <span>Sign In</span>
                </TabsTrigger>
                <TabsTrigger value="signup" className="flex items-center space-x-2">
                  <User className="w-4 h-4" />
                  <span>Sign Up</span>
                </TabsTrigger>
              </TabsList>

              {/* Sign In Tab */}
              <TabsContent value="signin">
                <form onSubmit={handleSignIn} className="space-y-4">
                  <div className="space-y-2">
                    <Label htmlFor="signin-email">Email Address</Label>
                    <div className="relative">
                      <Mail className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signin-email"
                        type="email"
                        placeholder="user@limitlessos.mil"
                        value={signInForm.email}
                        onChange={(e) => setSignInForm({ ...signInForm, email: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                      />
                    </div>
                  </div>

                  <div className="space-y-2">
                    <Label htmlFor="signin-password">Password</Label>
                    <div className="relative">
                      <Lock className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signin-password"
                        type="password"
                        placeholder="••••••••••"
                        value={signInForm.password}
                        onChange={(e) => setSignInForm({ ...signInForm, password: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                      />
                    </div>
                  </div>

                  <Button 
                    type="submit" 
                    className="w-full glow-primary" 
                    disabled={isLoading}
                  >
                    {isLoading ? 'Authenticating...' : 'Access System'}
                  </Button>
                </form>
              </TabsContent>

              {/* Sign Up Tab */}
              <TabsContent value="signup">
                <form onSubmit={handleSignUp} className="space-y-4">
                  <div className="space-y-2">
                    <Label htmlFor="signup-username">Username</Label>
                    <div className="relative">
                      <User className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signup-username"
                        type="text"
                        placeholder="operator_001"
                        value={signUpForm.username}
                        onChange={(e) => setSignUpForm({ ...signUpForm, username: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                      />
                    </div>
                  </div>

                  <div className="space-y-2">
                    <Label htmlFor="signup-email">Email Address</Label>
                    <div className="relative">
                      <Mail className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signup-email"
                        type="email"
                        placeholder="user@limitlessos.mil"
                        value={signUpForm.email}
                        onChange={(e) => setSignUpForm({ ...signUpForm, email: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                      />
                    </div>
                  </div>

                  <div className="space-y-2">
                    <Label htmlFor="signup-password">Password</Label>
                    <div className="relative">
                      <Lock className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signup-password"
                        type="password"
                        placeholder="••••••••••"
                        value={signUpForm.password}
                        onChange={(e) => setSignUpForm({ ...signUpForm, password: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                        minLength={6}
                      />
                    </div>
                  </div>

                  <div className="space-y-2">
                    <Label htmlFor="signup-confirm">Confirm Password</Label>
                    <div className="relative">
                      <Lock className="absolute left-3 top-3 w-4 h-4 text-muted-foreground" />
                      <Input
                        id="signup-confirm"
                        type="password"
                        placeholder="••••••••••"
                        value={signUpForm.confirmPassword}
                        onChange={(e) => setSignUpForm({ ...signUpForm, confirmPassword: e.target.value })}
                        className="pl-10 glass border-primary/30"
                        required
                      />
                    </div>
                    {signUpForm.password !== signUpForm.confirmPassword && signUpForm.confirmPassword && (
                      <p className="text-sm text-destructive">Passwords do not match</p>
                    )}
                  </div>

                  <Button 
                    type="submit" 
                    className="w-full glow-secure" 
                    disabled={isLoading || signUpForm.password !== signUpForm.confirmPassword}
                  >
                    {isLoading ? 'Creating Account...' : 'Create Account'}
                  </Button>
                </form>
              </TabsContent>
            </Tabs>
          </Card>

          {/* Footer */}
          <div className="text-center text-sm text-muted-foreground space-y-2">
            <p>Secure • Military-Grade • Zero-Trust Architecture</p>
            <Link to="/" className="text-primary hover:underline">
              Return to Main System
            </Link>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Auth;