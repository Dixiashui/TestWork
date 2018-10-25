#===============================================================================
# hiG Object
# 
#-------------------------------------------------------------------------------
# HISTORY
# 2006-04-20 Caio Bruchert     v0.01  Initial coding
# 2007-03-23 Caio Bruchert     v0.02  Fix blades detection
# 2007-06-20 Caio Bruchert     v0.03  - Add get version routine
#                                     - Fix valid card detection             
# 2007-09-13 Samuel Mello      v0.04  Added support to automation
#
#-------------------------------------------------------------------------------
# TODO:
#
#===============================================================================

package hiG;

use strict;
use Net::Telnet;
use Data::Dumper;

## Constants
use constant VERSION => "0.04";

use constant HIG1100 => "hiG1100";
use constant HIG1200 => "hiG1200";


$| = 1;

my $dbg = 0;
my $verbose = 0;
my $auto = 0;
my $continue_on_error = "";

sub getVersion {
  return __PACKAGE__." package v".VERSION;
}

### Constructor
sub new {
  my $class = shift;
  my $type = ref($class) || $class;
  my $self = {
      SESSION => undef,
      HOST => undef,
      HOSTNAME => undef,
      TYPE => undef,
      SLOT_MAX => undef,
      REV => undef,
      STREAM_TYPE => undef,
      RED_TYPE  => undef,
      MGC_PROTO => undef,
      USERNAME => "admin",
      PASSWORD => "pleasechangeme",
      USERPROMPT => undef,
      BLADES => [],
      DUMPFILE => undef,
      ERRMSG => undef,

      # CLI names that can be different depending on the release
      CLI_GW_OBJ => "gateway",
      CLI_BLD_OBJ => "bld",
      CLI_USE_OLD_SYNTAX => 0,
      CLI_FQDN_PARAM => "mg_fqdn",
      CLI_SET_CPE_BROADCAST => 1,

      # hiG Vesrion (V7 or V8?)
      HIG_LOAD_VERSION => "Vx",
  };
  bless $self, $type;

  if( $self->initialize(@_) ) { 
    print "Init Failed in Shelf Object. Improper arguments to the constructor.Args:",@_, "\n";
    $self->DESTROY();
    return undef;
  }

  return $self;
}

sub initialize {
  my $self = shift;
  $self->{HOST} = shift if (@_);
  $self->{DUMPFILE} = shift if (@_);
  $self->{USERPROMPT} = $self->{USERNAME}."#";

  #Create a new session 
  $self->{SESSION} = new Net::Telnet(Errmode => "return");
  $self->{SESSION}->dump_log($self->{DUMPFILE}) if $self->{DUMPFILE};

  return 0;
}

sub connect {
  my $self = shift;
  $self->{HOST} = shift if (@_);
  # Connect
  $self->{SESSION}->open($self->{HOST})
    or $self->errmsg($self->{SESSION}->errmsg) and return 0;
  # Detect gateway type and revision
  my ($pre, $match) = $self->{SESSION}->waitfor('/Siemens [AG|Networks].*$/m')
    or $self->errmsg("Error! This host is not a hiG gateway.") and return 0;
  ($self->{TYPE}, $self->{REV}) = $pre =~ /SURPASS hiG (\d{4}) SCE Revision (\d\d\.\d\d\.\d\d\.\d\d)/m 
    or $self->errmsg("Error! Could not determine the hiG revision.") and return 0;
  $self->{TYPE} =~ /1[12]00/ 
    or $self->errmsg("Error! Gateway ",$self->{TYPE}," is not supported.") and return 0;
  $self->{TYPE} = "hiG".$self->{TYPE};

  if ($self->is1200) {
    $self->{SLOT_MAX} = 19;
  }
  else {
    $self->{SLOT_MAX} = 8;
  }

  return 1;
}

sub login {
  my $self = shift;
  $self->{USERNAME} = shift if (@_);
  $self->{PASSWORD} = shift if (@_);

  $self->{SESSION}->login($self->{USERNAME},$self->{PASSWORD})
    or $self->errmsg($self->{SESSION}->errmsg) and return 0;

  return 1;
}

#-------------------------------------------------------------------------------
# cliCmd($cmd [, $reboot])
#-------------------------------------------------------------------------------
sub cliCmd {
  my $self = shift;
  my $cmdArg = shift;
  my $rebootArg = shift;
  my $key;
  my $needReboot=0;
  my $question_prompt = '(Y\/N)';
  my $ipmp_silly_prompt = 'change to take affect';
  my $rebooting_prompt = 'rebooting';
  my $telnet = $self->{SESSION};
  my $userprompt = $self->{USERPROMPT};
  my $host = $self->{HOST};
  
  if($self->debug()) {
    print("  $cmdArg\n");
    return 0; 
  }
  if ($self->verbose()) {
    print("\n$cmdArg\n");
  }
#  if ($stepOpt) {
#    print("\nCLI cmd:\n  ".$cmdArg."\nSend command? (y/n) "); 
#    $key = <STDIN>;
#    return 0 if $key =~ /n/i;
#  }

  $telnet->buffer_empty;
  $telnet->print($cmdArg); 
  $telnet->getline; #discard echoed line
  my($pre,$match) = $telnet->waitfor(Match => "/$userprompt/mi", 
                                     Match => "/$question_prompt/mi",
				     Match => "/$ipmp_silly_prompt/",
                                     Match => "/$rebooting_prompt/mi")
                     or die "\n".$host.": Error! CLI command response failed.\n";
   
#  print("Output:\n  $pre$match") if $stepOpt; 
    
  if ($match =~ /$userprompt/mi){
    if ($pre =~ /^.*(Error.*)$/im) {
      if ($auto){
	   print $1."\n";
	   $key = $continue_on_error;
      }else{		       
            print "\nCLI Error!\nCommand line: \"".$cmdArg."\"\nResponse: \"".$1."\"\n";
	    print   "\nContinue? (y/n) ";
	    $key = <STDIN>;
      }
      die "Script aborted!\n" if $key =~ /n/i;
      return 0;
    }
    elsif ($pre =~ /reboot/im) {
       $needReboot=1;
    }
    elsif ($pre =~ /^[\x20\f\t\r]*(\w+.*)$/m) {
      if ($auto){
	   $key = $continue_on_error;
	   print $1."\n";
      }else{		       
            print "\nCLI Warning! Unkwnown response.\nCommand line: \"".$cmdArg."\"\nResponse: \"".$1."\"\n";
	    print   "\nContinue? (y/n) ";
	    $key = <STDIN>;
      }
     die "Script aborted!\n" if $key =~ /n/i;
    }
  }
  elsif ($match =~ /$question_prompt/mi){
    # always aswer YES when asked to proceed
    if ($pre =~ /proceed/im || $pre =~ /sure/im) {
      return $self->cliCmd("y", $rebootArg);
    }
    # hiG1200 prompt, just send y without waiting 
    elsif ($pre =~ /requires a gateway reboot/im) {
      $telnet->print("y") or die "Error! Telnet print failed!\n"; 
      $needReboot=0;
    }
    # answer YES or NO depending on 2nd arg
    elsif ($pre =~ /reboot/im) {
       if ($rebootArg) {
        return $self->cliCmd("y", 1);
       }
       else
       {
         $self->cliCmd("n");
         $needReboot=1;
       }
    }
  }
  elsif ($match =~ /$ipmp_silly_prompt/mi){
	  return $self->cliCmd("y", $rebootArg);
  }
  elsif ($match =~ /$rebooting_prompt/mi){
    $needReboot=0;
  }
  
  return $needReboot;
}




sub cliCmd2 {
  my $self = shift;
  my $cmd = shift;
  my $prompt = shift;

  if ($self->debug) {
    print "==cmd==> ", $cmd, "\n";
    return 0;
  }

   return $self->cliCmdInt($cmd, $prompt);
}

sub cliCmdInt {
  my $self = shift;
  my $cmd = shift;
  my $prompt = shift;
  $prompt = "/".$self->{USERPROMPT}."/" unless $prompt;
  my @output = ();
  my $key;

  @output = $self->{SESSION}->cmd(String=>$cmd,Prompt=>$prompt);
  if ( $#output eq  -1 ) {
    $self->errmsg($cmd."\nError! CLI command returned no data.");
    return 0;
  }

  # CLI returned Error
  if ($output[0] =~ /^.*(Error.*)$/) {
      if ($auto){
	   print $1."\n";
	   $key = $continue_on_error;
      }else{		       
	   print "\nCLI Error!\nCommand line: \"".$cmd."\"\nResponse: \"".$1."\"\n";
	   print "\nContinue? (y/n) ";
	   $key = <STDIN>;
      }
      die "Script aborted!\n" if $key =~ /n/i;
  }
  elsif ($output[0] =~ /^.*(Error.*)$/) {
      if ($auto){
	   print $1."\n";
	   $key = $continue_on_error;
      }else{		       
    	    print "\nCLI Error!\nCommand line: \"".$cmd."\"\nResponse: \"".$1."\"\n";
	    print   "\nContinue? (y/n) ";
	    $key = <STDIN>;
      }
    die "Script aborted!\n" if $key =~ /n/i;
  }

  return @output;
}

sub reboot {
   my $self = shift;
   my $bld = shift;
   
   unless($bld) {
     print "\nRebooting the gateway... ";
     $self->cliCmd("reboot",1); 
   }
   else {
     print "\nRebooting slot ".$bld."... ";
     $self->cliCmd("reboot /".$self->cli_bld_obj."/".$self->hostname."_".$bld.".".$self->cli_bld_obj,1); 
   }

   if ($auto){
	   sleep(3*60);
   }
   
   print "OK!\n";
 }
 


sub setCliReleaseOpts {
  my $self = shift;

  # Older revs have different obj names 
  $self->{REV} =~ /(\d\d)\.(\d\d)\.(\d\d)\.(\d\d)/;
  if (    ($1 <= 10)                  # <= 10.xx.yy.zz
       || ($1==11 && $2==1 && $4<=8)  # <= 11.01.yy.08  MG V5.0
       || ($1==11 && $2==11 && $4<=8) # <= 11.11.yy.08  MRG V1
     ) {
    $self->{CLI_GW_OBJ} = "shelf";
    $self->{CLI_BLD_OBJ} = "slt";
    $self->{CLI_USE_OLD_SYNTAX} = 1;
  }
  
  # don't set CPE broadcast addr
  if (    ($1==11 && $2==22 && $4>=11) # >= 11.22.yy.11  MRG V2
       || ($1==11 && $2>22)            # >= 11.22.yy.zz  MRG V2
     ) {
    $self->{CLI_SET_CPE_BROADCAST} = 0;
  }  

  # use old mg_fqdn param
  if (    ($1 <= 10)                  # <= 10.xx.yy.zz
       || ($1==11 && $2==1 && $4<=33) # <= 11.01.yy.33  MG V5.0
       || ($1==11 && $2==11)          # == 11.11.yy.zz  MRG V1
       || ($1==11 && $2==20)          # == 11.11.yy.zz  MRG V1.2
       || ($1==11 && $2==22 && $4<=5) # <= 11.22.yy.05  MRG V2
     ) {
    $self->{CLI_FQDN_PARAM} = "mg_domain_name";
  }
  if ( $1<=12 && $2 == 2) {
    $self->{HIG_LOAD_VERSION} = 0,
  }
  if ( $1>=13 ) {
    $self->{HIG_LOAD_VERSION} = 1,
  }
}

sub refreshGwStatus {
  my $self = shift;
  my @output = ();
  
  # system properties
  @output = $self->cliCmdInt("show system")
    or $self->errmsg("Error! Wrong show system response.") and return 0;
  foreach (@output) { 
    ($self->{HOSTNAME}) = $1 if $_ =~ /Hostname:\s*(\S*)/;
  }

  # gateway properties
  @output = $self->cliCmdInt("sh ".$self->{CLI_GW_OBJ})
    or $self->errmsg("Error! Wrong show gateway response.") and return 0;
  foreach (@output) { 
    if ($_ =~ /Hostname:\s*(\S*)/)                {$self->{HOSTNAME}    = $1; next}
    if ($_ =~ /Interface\/Stream Type :\s*(\S*)/) {my $tmp=$1; $tmp=~s/-//; $tmp=~s/\///; $self->{STREAM_TYPE} = $tmp; next}
    if ($_ =~ /Redundancy Scheme :\s*(\S*)/)      {$self->{RED_TYPE}    = $1; next}
    if ($_ =~ /Gateway .*:\s*(\S*)/)              {my $tmp=$1; $tmp=~s/\.//; $self->{MGC_PROTO} = $tmp; next}
  }

  # blades status
  @output = $self->cliCmdInt("stat /$self->{CLI_BLD_OBJ}/*")
    or $self->errmsg("Error! Wrong blade status response.") and return 0;
  

  # remove 2 lines header
  shift @output;
  shift @output;
  
  foreach (@output) {
    s/^\s+//;   # remove leading spaces
    s/\s+$//;   # remove trailing spaces
    push @{$self->{BLADES}}, $_ if $_ ne "";
  }
  
#  $Data::Dumper::Useqq = 1;
#  print Dumper(@{$self->{BLADES}});
#  die;

  return 1;
}


sub getReleaseList {
  my $self = shift;
  my @rel_list;
  my @dir_list = $self->cliCmdInt("dir /ad0");
  
  foreach my $line (@dir_list) {
    if($self->is1200) {
      if( $line =~ /(\d\d\.\d\d\.\d\d\.\d\d)/ ) {
          push @rel_list, $1;
      }
    }
    elsif($self->is1100) {
      if( $line =~ /hiG1100\.(\d\d\.\d\d\.\d\d\.\d\d\.*.*)/ ) {
          push @rel_list, $1;
      }
    }
  }  

#  print @dir_list;
  return sort @rel_list;
}



sub showGwProps {
  my $self = shift;

  print "\nGateway configuration:\n",
        "  Host         = ",$self->{HOSTNAME},"\n",
        "  Stream type  = ".$self->{STREAM_TYPE}."\n",
        "  Redundancy   = ".$self->{RED_TYPE}."\n",
        "  MGC Protocol = ".$self->{MGC_PROTO}."\n";
}

sub showBldStatus {
  my $self = shift;

  print "\nBlade status:\n";

  foreach (@{$self->{BLADES}}) {
    print "$_\n";
  }
}

sub getBladeName {
  my $self = shift;
  my $bld = shift;
  return $self->getBladeStatusCol($bld, 1);
}

sub getBladeAdminState {
  my $self = shift;
  my $bld = shift;
  return $self->getBladeStatusCol($bld, 2);
}

sub getBladeOperState {
  my $self = shift;
  my $bld = shift;
  return $self->getBladeStatusCol($bld, 3);
}

sub getBladeTypeDetected {
  my $self = shift;
  my $bld = shift;
  return $self->getBladeStatusCol($bld, 4);
}

sub getBladeTypeConfigured {
  my $self = shift;
  my $bld = shift;
  return $self->getBladeStatusCol($bld, 5);
}

sub getBladeStatusCol {
  my $self = shift;
  my ($bld,$col) = @_;
  
  $self->{BLADES}[$bld-1] =~ /(\/$self->{CLI_BLD_OBJ}\/$self->{HOSTNAME}\_\d\d.$self->{CLI_BLD_OBJ})\s+(Empty|In Service|Out Of Service)\s+(Empty|In Service|In Standby|Out Of Service|Failed)\s+(\S+)\s+(\S+)/;

  return $1 if ($col==1);
  return $2 if ($col==2);
  return $3 if ($col==3);
  return $4 if ($col==4);
  return $5 if ($col==5);
}

sub isSlotEmpty {
  my $self = shift;
  my $bld = shift;

  if ($self->getBladeOperState($bld) =~ /Empty/i) {return 1;}
  
  return 0;
}

sub getFirstValidCpe {
  my $self = shift;

  for (my $i=3; $i <= 4; $i++) {
    if ($self->getBladeTypeDetected($i) =~ /cpe/i) {
      if ($self->getBladeOperState($i) !~ /Empty/i && $self->getBladeOperState($i) !~ /Failed/i) {
	return $i;
      }
    }
  }
  
  return -1;
}

sub getFirstValidDxm {
  my $self = shift;

  for (my $i=5; $i <= 19; $i++) {
    if ($self->getBladeTypeDetected($i) =~ /dxm/i) {
      if ($self->getBladeOperState($i) !~ /Empty/i && $self->getBladeOperState($i) !~ /Failed/i) {
	return $i;
      }
    }
  }
  return -1;
}

sub getFirstValidIpm {
  my $self = shift;

  for (my $i=5; $i <= 19; $i++) {
    if ($self->getBladeTypeDetected($i) =~ /ipm/i) {
      if ($self->getBladeOperState($i) !~ /Empty/i && $self->getBladeOperState($i) !~ /Failed/i) {
	return $i;
      }
    }
  }
  
  return -1;
}

sub getIpmType {
  my $self = shift;
  my $bld = shift;

  if ($self->getBladeTypeDetected($bld) =~ /ipm(\d)/i) {return $1;}
  
  return -1;
}

sub getTelnetSession {
  my $self = shift;
  return $self->{SESSION}; 
}
sub host {
  my $self = shift;
  return $self->{HOST}; 
}
sub hostname {
  my $self = shift;
  return $self->{HOSTNAME}; 
}
sub type {
  my $self = shift;
  return $self->{TYPE}; 
}
sub rev {
  my $self = shift;
  return $self->{REV}; 
}
sub stream_type {
  my $self = shift;
  return $self->{STREAM_TYPE}; 
}
sub red_type {
  my $self = shift;
  return $self->{RED_TYPE}; 
}
sub mgc_proto {
  my $self = shift;
  return $self->{MGC_PROTO}; 
}

sub getNumBlades {
  my $self = shift;
  return $#{$self->{BLADES}}+1; 
}

sub is1200 {
  my $self = shift;
  return $self->{TYPE} eq HIG1200; 
}
sub is1100 {
  my $self = shift;
  return $self->{TYPE} eq HIG1100; 
}
sub errmsg {
  my $self = shift;
  if (@_) { $self->{ERRMSG} = shift; }
  return $self->{ERRMSG}; 
}

## new/old syntax
sub cli_gw_obj {
  my $self = shift;
  return $self->{CLI_GW_OBJ}; 
}
sub cli_bld_obj {
  my $self = shift;
  return $self->{CLI_BLD_OBJ}; 
}
sub use_old_syntax {
  my $self = shift;
  return $self->{CLI_USE_OLD_SYNTAX}; 
}
sub cli_fqdn_param {
  my $self = shift;
  return $self->{CLI_FQDN_PARAM}; 
}
sub set_cpe_broadcast {
  my $self = shift;
  return $self->{CLI_SET_CPE_BROADCAST}; 
}


sub dump {
  my $self = shift;
  print Dumper($self);    
}

sub debug {
  my $self = shift;
  return $dbg unless (@_);    
  $dbg = shift;
}

sub auto {
  my $self = shift;
  return $auto unless (@_);
  $auto = shift;
}

sub continue_on_error {
  my $self = shift;
  return $continue_on_error unless (@_);
  $continue_on_error = shift;
}

sub verbose {
  my $self = shift;
  return $verbose unless (@_);
  $verbose = shift;
}

sub DESTROY {
  my $self = shift;
  if ($self->{SESSION}) {
      $self->{SESSION}->close();
  }
  $self->{SESSION} = undef;
  $self = undef;
}

# get hig version
sub getHiG1200Version {
  my $self =shift;
  return $self->{HIG_LOAD_VERSION};
}

######################## Exported Constants ##########################


sub TELNET_IAC ()	    {255}; # interpret as command:


1;
__END__
# Stub documentation

=head1 NAME

Gateway - Perl extension for hiG gateways

=head1 SYNOPSIS

  use Gateway;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Gateway, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.


=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Caio Bruchert, E<lt>caio.bruchert.ext@siemens.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2006 by Caio Bruchert

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.6 or,
at your option, any later version of Perl 5 you may have available.


=cut
